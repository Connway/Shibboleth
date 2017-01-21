/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Shibboleth_AngelScriptComponent.h"
#include "Shibboleth_AngelScriptManager.h"
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(Shibboleth::AngelScriptComponent)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(AngelScriptComponent)
	.BASE(Component)
	//.var("Script", &AngelScriptComponent::_res)
SHIB_REFLECTION_CLASS_DEFINE_END(AngelScriptComponent)

AngelScriptComponent::~AngelScriptComponent(void)
{
	if (_object) {
		_object->Release();
	}

	if (_context) {
		AngelScriptManager& as_mgr = GetApp().getManagerTUnsafe<AngelScriptManager>();
		Gaff::SpinLock& lock = as_mgr.getEngineLock();

		lock.lock();
		as_mgr.getEngine()->ReturnContext(_context);
		lock.unlock();
	}
}

void AngelScriptComponent::allComponentsLoaded(void)
{
	// Determine class by either metadata markup or match the file name.
	if (_res && _res->getState() == IResource::RS_LOADED) {
		const asIScriptModule* const module = _res->getModule();
		CScriptBuilder& builder = _res->getBuilder();

		// Check for markup.
		for (asUINT i = 0; i < module->GetObjectTypeCount(); ++i) {
			const asITypeInfo* const ti = module->GetObjectTypeByIndex(i);
			const char* const metadata = builder.GetMetadataStringForType(ti->GetTypeId());

			if (!strcmp(metadata, "ComponentClass")) {
				_type_info = ti;
				break;
			}
		}

		// If no markup, just take the file name minus the extension.
		if (!_type_info) {
			const U8String& path = _res->getFilePath().getString();
			size_t begin_index = Gaff::FindLastOf(path.data(), path.size(), '/');
			const size_t ext_index = Gaff::FindLastOf(path.data(), path.size(), '.');

			if (begin_index == SIZE_T_FAIL) {
				begin_index = 0;
			}
			else {
				begin_index += 1;
			}

			char temp[128] = { 0 };
			strncpy_s(temp, path.data(), ext_index - begin_index);

			_type_info = module->GetTypeInfoByName(temp);
		}

		// Construct the script component.
		if (_type_info) {
			asIScriptEngine* const engine = module->GetEngine();

			char temp_decl[128] = { 0 };
			snprintf(temp_decl, 128, "%s@ %s()", _type_info->GetName(), _type_info->GetName());
			asIScriptFunction* const factory = _type_info->GetFactoryByDecl(temp_decl);

			if (!factory) {
				LogErrorDefault("Failed to find default constructor for script class '%s'!", _type_info->GetName());
				_type_info = nullptr;
				_res = nullptr;
				return;
			}

			Gaff::SpinLock& lock = GetApp().getManagerTUnsafe<AngelScriptManager>().getEngineLock();

			lock.lock();
			_context = engine->CreateContext();
			lock.unlock();

			_context->Prepare(factory);
			_context->Execute();

			// Get the script object we just made.
			_object = *reinterpret_cast<asIScriptObject**>(_context->GetAddressOfReturnValue());
			_object->AddRef();

			// Call init() if it has one.
			asIScriptFunction* const init_func = _type_info->GetMethodByName("init");

			if (init_func) {
				_context->Prepare(init_func);
				_context->SetObject(_object);
				_context->Execute();
			}
		}
	}
}

void AngelScriptComponent::addToWorld(void)
{
	asIScriptFunction* const func = _type_info->GetMethodByName("onAddToWorld");

	if (func) {
		_context->Prepare(func);
		_context->SetObject(_object);
		_context->Execute();
	}
}

void AngelScriptComponent::removeFromWorld(void)
{
	asIScriptFunction* const func = _type_info->GetMethodByName("removeFromWorld");

	if (func) {
		_context->Prepare(func);
		_context->SetObject(_object);
		_context->Execute();
	}
}

NS_END
