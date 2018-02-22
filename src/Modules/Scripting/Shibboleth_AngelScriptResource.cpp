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
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(AngelScriptResource, 0)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(AngelScriptResource)
	.classAttrs(ResExtAttribute(".as"), MakeLoadFileCallbackAttribute(&AngelScriptResource::loadScript, JPI_SCRIPT))
	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(AngelScriptResource)

static int HandleInclude(const char* include, const char*, CScriptBuilder* builder, void*)
{
	IFileSystem* const fs = Shibboleth::GetApp().getFileSystem();
	IFile* const file = fs->openFile(include);

	if (!file) {
		LOG_RES_ERROR("Failed to find included script '%s'!", include);
		return -1;
	}

	int r = builder->AddSectionFromMemory(include, file->getBuffer(), static_cast<unsigned int>(file->size()));

	fs->closeFile(file);
	return r;
}


AngelScriptResource::AngelScriptResource(void)
{
}

AngelScriptResource::~AngelScriptResource(void)
{
	if (_module) {
		_module->Discard();
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

void AngelScriptResource::loadScript(IFile* file)
{
	AngelScriptManager& as_mgr = GetApp().getManagerTUnsafe<AngelScriptManager>();
	asIScriptEngine* const engine = as_mgr.getEngine();
	std::mutex& lock = as_mgr.getEngineLock();

	_builder.SetIncludeCallback(HandleInclude, nullptr);

	{
		std::lock_guard<std::mutex> scoped_lock(lock);

		int r = _builder.StartNewModule(engine, getFilePath().getBuffer());
		RES_FAIL_MSG(r < 0, "Failed to create new script module for script '%s'!", getFilePath().getBuffer());

		r = as_mgr.addScriptComponentDefinition(_builder);
		RES_FAIL_MSG(r < 0, "Failed to add section for `ScriptComponent`!");

		r = _builder.AddSectionFromMemory("script", file->getBuffer(), static_cast<unsigned int>(file->size()));
		RES_FAIL_MSG(r < 0, "Failed to add section for script '%s'!", getFilePath().getBuffer());

		r = _builder.BuildModule();
		RES_FAIL_MSG(r < 0, "Failed to build module for script '%s'!", getFilePath().getBuffer());

		_module = _builder.GetModule();
		RES_FAIL_MSG(!_module, "Failed to get module for script '%s'!", getFilePath().getBuffer());
	}

	succeeded();
}

NS_END
