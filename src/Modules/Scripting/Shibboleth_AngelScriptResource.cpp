/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_AngelScriptResource.h"
#include "Shibboleth_AngelScriptManager.h"
#include <Shibboleth_ResourceExtensionAttribute.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(AngelScriptResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(AngelScriptResource)
	.classAttrs(ResExtAttribute(".as"))
	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(AngelScriptResource)

AngelScriptResource::AngelScriptResource(void)
{
}

AngelScriptResource::~AngelScriptResource(void)
{
	if (_module) {
		_module->Discard();
	}
}

void AngelScriptResource::load(void)
{
	_script_file = loadFile(getFilePath().getBuffer());

	if (_script_file) {
		Gaff::JobData job_data = { LoadScript, this };
		GetApp().getJobPool().addJobs(&job_data, 1, nullptr, JPI_SCRIPT);
	}
}

const asIScriptModule* AngelScriptResource::getModule(void) const
{
	return _module;
}

CScriptBuilder& AngelScriptResource::getBuilder(void)
{
	return _builder;
}

void AngelScriptResource::loadScript(void)
{
	AngelScriptManager& as_mgr = GetApp().getManagerTUnsafe<AngelScriptManager>();
	asIScriptEngine* const engine = as_mgr.getEngine();
	Gaff::SpinLock& lock = as_mgr.getEngineLock();

	lock.lock();

	int r = _builder.StartNewModule(engine, getFilePath().getBuffer());
	RES_FAIL_MSG(r < 0, "Failed to create new script module for script '%s'!", getFilePath().getBuffer());

	r = _builder.AddSectionFromMemory(
		"ScriptComponent",
		R"(
			shared abstract class ScriptComponent
			{
				Object@ owner
				{
					get { return _component.owner; }
				}

				bool active
				{
					get { return _component.active; }
					set { _component.active = value; }
				}

				const Component@ opImplCast() const { return _component; }
				Component@ opImplCast() { return _component; }

				// No way to forward this?
				//void opCast(?& out value) { _component.opCast(value); }

				private Component@ _component;
			}
		)"
	);
	RES_FAIL_MSG(r < 0, "Failed to add section for `ScriptComponent`!");

	r = _builder.AddSectionFromMemory("script", _script_file->getBuffer(), static_cast<unsigned int>(_script_file->size()));
	RES_FAIL_MSG(r < 0, "Failed to add section for script '%s'!", getFilePath().getBuffer());

	r = _builder.BuildModule();
	RES_FAIL_MSG(r < 0, "Failed to build module for script '%s'!", getFilePath().getBuffer());

	_module = _builder.GetModule();
	RES_FAIL_MSG(!_module, "Failed to get module for script '%s'!", getFilePath().getBuffer());

	lock.unlock();
	succeeded();
}

void AngelScriptResource::LoadScript(void* data)
{
	AngelScriptResource* const as_res = reinterpret_cast<AngelScriptResource*>(data);
	as_res->loadScript();

	GetApp().getFileSystem()->closeFile(as_res->_script_file);
	as_res->_script_file = nullptr;
}

NS_END