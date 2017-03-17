/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable : 4307)
#endif

SHIB_REFLECTION_DEFINE(Shibboleth::AngelScriptComponent)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(AngelScriptComponent)
	.BASE(Component)

	.var("Script", &AngelScriptComponent::_res)
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
	ProxyAllocator proxy_allocator;
	eastl::function<void (IResource*)> cb(eastl::allocator_arg, proxy_allocator, [&](IResource* res) -> void
	{
		onScriptLoaded(res);
	});

	_res->addResourceStateCallback(std::move(cb));
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

void AngelScriptComponent::setScript(const AngelScriptResourcePtr& script)
{
	_type_info = nullptr;

	if (_object) {
		_object->Release();
		_object = nullptr;
	}

	if (_context) {
		Gaff::SpinLock& lock = GetApp().getManagerTUnsafe<AngelScriptManager>().getEngineLock();

		lock.lock();
		_context->GetEngine()->ReturnContext(_context);
		lock.unlock();

		_context = nullptr;
	}

	_property_map.clear();
	_method_map.clear();

	_res = script;

	onScriptLoaded(_res.get());
}

const AngelScriptResourcePtr& AngelScriptComponent::getScript(void) const
{
	return _res;
}

const asIScriptObject* AngelScriptComponent::getObject(void) const
{
	return _object;
}

asIScriptObject* AngelScriptComponent::getObject(void)
{
	return _object;
}

int32_t AngelScriptComponent::getPropertyIndex(const char* name) const
{
	return getPropertyIndex(Gaff::FNV1aHash32String(name));
}

int32_t AngelScriptComponent::getPropertyIndex(Gaff::Hash32 name) const
{
	auto it = _property_map.find(name);
	return (it == _property_map.end()) ? -1 : static_cast<int32_t>(it - _property_map.begin());
}

int32_t AngelScriptComponent::getFunctionIndex(const char* name) const
{
	return getFunctionIndex(Gaff::FNV1aHash32String(name));
}

int32_t AngelScriptComponent::getFunctionIndex(Gaff::Hash32 name) const
{
	auto it = _method_map.find(name);
	return (it == _method_map.end()) ? -1 : static_cast<int32_t>(it - _method_map.begin());
}

void AngelScriptComponent::prepareMethod(const char* name)
{
	return prepareMethod(Gaff::FNV1aHash32String(name));
}

void AngelScriptComponent::prepareMethod(Gaff::Hash32 name)
{
	auto it = _method_map.find(name);
	GAFF_ASSERT(it != _method_map.end());
	_context->Prepare(it->second);
	_context->SetObject(_object);
}

void AngelScriptComponent::prepareMethod(int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(_method_map.size()));
	auto it = _method_map.begin() + index;
	_context->Prepare(it->second);
}

void AngelScriptComponent::callMethod(void)
{
	_context->Execute();
}

void AngelScriptComponent::onScriptLoaded(IResource* /*res*/)
{
	// Determine class by either metadata markup or match the file name.
	if (_res && _res->getState() == IResource::RS_LOADED) {
		const asIScriptModule* const module = _res->getModule();
		//CScriptBuilder& builder = _res->getBuilder();

		//// Check for markup.
		//for (asUINT i = 0; i < module->GetObjectTypeCount(); ++i) {
		//	const asITypeInfo* const ti = module->GetObjectTypeByIndex(i);
		//	const char* const metadata = builder.GetMetadataStringForType(ti->GetTypeId());

		//	if (!strcmp(metadata, "ComponentClass")) {
		//		_type_info = ti;
		//		break;
		//	}
		//}

		// Find the first class to derive from ScriptComponent.
		const asITypeInfo * const script_comp_type_info = module->GetTypeInfoByName("ScriptComponent");
		const asUINT num_types = module->GetObjectTypeCount();

		for (asUINT i = 0; i < num_types; ++i) {
			const asITypeInfo * const type_info =  module->GetObjectTypeByIndex(i);

			if (type_info != script_comp_type_info && type_info->DerivesFrom(script_comp_type_info)) {
				_type_info = type_info;
				break;
			}
		}

		//// If no markup, just take the file name minus the extension.
		//if (!_type_info) {
		//	const U8String& path = _res->getFilePath().getString();
		//	size_t begin_index = Gaff::FindLastOf(path.data(), path.size(), '/');
		//	const size_t ext_index = Gaff::FindLastOf(path.data(), path.size(), '.');

		//	if (begin_index == SIZE_T_FAIL) {
		//		begin_index = 0;
		//	}
		//	else {
		//		begin_index += 1;
		//	}

		//	char temp[128] = { 0 };
		//	strncpy_s(temp, path.data(), ext_index - begin_index);

		//	_type_info = module->GetTypeInfoByName(temp);
		//}

		if (!_type_info) {
			LogErrorDefault("Failed to find a component that inherits from 'ScriptComponent' in '%s'!", _res->getFilePath().getBuffer());
			return;
		}

		// Construct the script component.
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

		// Cache properties.
		const asUINT num_props = _object->GetPropertyCount();

		constexpr Gaff::Hash32 comp_hash = Gaff::FNV1aHash32Const("_component");

		for (asUINT i = 0; i < num_props; ++i) {
			const char* const name = _object->GetPropertyName(i);
			const int type_id = _object->GetPropertyTypeId(i);

			const Gaff::Hash32 prop_name = Gaff::FNV1aHash32String(name);

			if (prop_name == comp_hash) {
				*reinterpret_cast<void**>(_object->GetAddressOfProperty(i)) = this;
			}

			PropertyData& data = _property_map[prop_name];
			data.type_id = type_id;
			data.type_info = (type_id <= asTYPEID_DOUBLE) ? nullptr : module->GetEngine()->GetTypeInfoById(type_id);
			data.property = _object->GetAddressOfProperty(i);
		}

		// Cache methods.
		const asUINT num_methods = _type_info->GetMethodCount();

		for (asUINT i = 0; i < num_methods; ++i) {
			asIScriptFunction* const func = _type_info->GetMethodByIndex(i);
			_method_map[Gaff::FNV1aHash32String(func->GetName())] = func;
		}
	}
}

NS_END

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif
