/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#pragma once

#include "Shibboleth_ScriptDefines.h"
#include <Shibboleth_IManager.h>

class asIThreadManager;
class asIScriptEngine;
class asIScriptModule;
struct asSMessageInfo;

NS_SHIBBOLETH

class AngelScriptResource;
class IFile;

class AngelScriptManager final : public IManager
{
public:
	enum class CompileResult
	{
		Success,
		Deferred,
		Failed
	};

	static void InitModuleThread(void);

	~AngelScriptManager(void);

	bool initAllModulesLoaded(void) override;
	bool init(void) override;

	void initModuleThread(void);

	CompileResult compile(AngelScriptResource& resource, const IFile& file);

private:
	struct ScriptInfo final
	{
		Vector<U8String> includes{ SCRIPT_ALLOCATOR };
	};

	asIThreadManager* _thread_mgr = nullptr;
	asIScriptEngine* _engine = nullptr;
	asIScriptModule* _main_module = nullptr;

	ScriptInfo modifySource(const char8_t* section_name, U8String& source);

	void messageCallback(const asSMessageInfo* msg, void* param);

	SHIB_REFLECTION_CLASS_DECLARE(AngelScriptManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::AngelScriptManager)
