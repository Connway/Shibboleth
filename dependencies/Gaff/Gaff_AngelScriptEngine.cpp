/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gaff_AngelScriptEngine.h"
#include "Gaff_File.h"

NS_GAFF

AngelScriptEngine::AngelScriptEngine(void)
#ifdef _UNICODE
: _alloc(malloc), _free(free)
#endif
{
}

AngelScriptEngine::~AngelScriptEngine(void)
{
}

bool AngelScriptEngine::init(void)
{
	_engine.set(asCreateScriptEngine(ANGELSCRIPT_VERSION));

	if (!_engine) {
		return false;
	}

	int ret = _engine->SetMessageCallback(asMETHOD(AngelScriptEngine, messageCallback), this, asCALL_THISCALL);

	if (ret < 0) {
		destroy();
		return false;
	}

	return _engine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, true) >= 0;
}

void AngelScriptEngine::destroy(void)
{
	SAFERELEASE(_engine);
}

bool AngelScriptEngine::setMessageCallback(MSGCALLBACK callback, asECallConvTypes call_type)
{
	return _engine->SetMessageCallback(asFUNCTION(callback), nullptr, call_type) >= 0;
}

bool AngelScriptEngine::setMemoryFunctions(asALLOCFUNC_t alloc, asFREEFUNC_t free)
{
	int ret = asSetGlobalMemoryFunctions(alloc, free) >= 0;

	if (ret < 0) {
		return false;
	}

#ifdef _UNICODE
	// this is solely for the purpose of the wchar_t version of addScript() ... ugh
	_alloc = alloc;
	_free = free;
#endif

	return true;
}

bool AngelScriptEngine::startModule(const char* module_name)
{
	assert(module_name && strlen(module_name));
	return _builder.StartNewModule(_engine.get(), module_name) >= 0;
}

bool AngelScriptEngine::addScript(const char* file_name)
{
	assert(file_name && strlen(file_name));
	return _builder.AddSectionFromFile(file_name) >= 0;
}

bool AngelScriptEngine::buildModule(void)
{
	return _builder.BuildModule() >= 0;
}

#ifdef _UNICODE
bool AngelScriptEngine::addScript(const wchar_t* file_name, const char* section_name)
{
	assert(file_name && section_name && wcslen(file_name) && strlen(section_name));
	File script(file_name, File::READ_BINARY);

	if (!script.isOpen()) {
		return false;
	}

	int size = script.getFileSize();
	char* buffer = (char*)_alloc(size);

	if (!buffer) {
		return false;
	}

	if (!script.readEntireFile(buffer)) {
		_free(buffer);
		return false;
	}

	if (_builder.AddSectionFromMemory(section_name, buffer, size) < 0) {
		_free(buffer);
		return false;
	}

	_free(buffer);
	return true;
}
#endif

AngelScriptModule AngelScriptEngine::getModule(const char* module_name)
{
	assert(module_name && strlen(module_name));
	return AngelScriptModule(_engine->GetModule(module_name));
}

bool AngelScriptEngine::registerEnum(const char* type)
{
	assert(type && strlen(type));
	return _engine->RegisterEnum(type) >= 0;
}

bool AngelScriptEngine::registerEnumValue(const char* type, const char* name, int value)
{
	assert(type && name && strlen(type) && strlen(name));
	return _engine->RegisterEnumValue(type, name, value) >= 0;
}

bool AngelScriptEngine::registerGlobalProperty(const char* declaration, void* object)
{
	assert(declaration && object && strlen(declaration));
	return _engine->RegisterGlobalProperty(declaration, object) >= 0;
}

bool AngelScriptEngine::registerObjectType(const char* object, int size_bytes, unsigned long type_flags)
{
	assert(object && strlen(object) && size_bytes > 0 && type_flags != 0);
	return _engine->RegisterObjectType(object, size_bytes, type_flags) >= 0;
}

bool AngelScriptEngine::registerObjectProperty(const char* object, const char* declaration, int byte_offset)
{
	assert(object && declaration && strlen(object) && strlen(declaration) && byte_offset > -1);
	return _engine->RegisterObjectProperty(object, declaration, byte_offset) >= 0;
}

bool AngelScriptEngine::registerInterface(const char* name)
{
	assert(name && strlen(name));
	return _engine->RegisterInterface(name) >= 0;
}

bool AngelScriptEngine::registerInterfaceMethod(const char* interface_name, const char* declaration)
{
	assert(interface_name && declaration && strlen(interface_name) && strlen(declaration));
	return _engine->RegisterInterfaceMethod(interface_name, declaration) >= 0;
}

bool AngelScriptEngine::registerFunctionDefinition(const char* declaration)
{
	assert(declaration && strlen(declaration));
	return _engine->RegisterFuncdef(declaration) >= 0;
}

bool AngelScriptEngine::registerTypedef(const char* type, const char* declaration)
{
	assert(type && declaration && strlen(type) && strlen(declaration));
	return _engine->RegisterTypedef(type, declaration) >= 0;
}

bool AngelScriptEngine::registerDefaultArrayType(const char* type)
{
	assert(type && strlen(type));
	return _engine->RegisterDefaultArrayType(type) >= 0;
}

AngelScriptContext AngelScriptEngine::createContext(void)
{
	assert(_engine);
	return AngelScriptContext(_engine->CreateContext());
}

void AngelScriptEngine::messageCallback(const asSMessageInfo* msg) const
{
	printf(msg->message);
}

bool AngelScriptEngine::registerGlobalFunctionHelper(const char* declaration, const asSFuncPtr& func_ptr, asECallConvTypes call_type, void* this_obj)
{
	assert(declaration && strlen(declaration));
	return _engine->RegisterGlobalFunction(declaration, func_ptr, call_type, this_obj) >= 0;
}

bool AngelScriptEngine::registerObjectMethodHelper(const char* object, const char* declaration, const asSFuncPtr& func_ptr, asECallConvTypes call_type)
{
	assert(object && declaration && strlen(object) && strlen(declaration));
	return _engine->RegisterObjectMethod(object, declaration, func_ptr, call_type) >= 0;
}

bool AngelScriptEngine::registerObjectBehaviourHelper(const char* object, const char* declaration, const asSFuncPtr& func_ptr,
														asEBehaviours behaviour, asECallConvTypes call_type, void* this_obj)
{
	assert(object && declaration && strlen(object) && strlen(declaration));
	return _engine->RegisterObjectBehaviour(object, behaviour, declaration, func_ptr, call_type, this_obj) >= 0;
}

bool AngelScriptEngine::registerStringFactoryHelper(const char* type, const asSFuncPtr& factory_func, asECallConvTypes call_type, void* this_obj)
{
	assert(type && strlen(type));
	return _engine->RegisterStringFactory(type, factory_func, call_type, this_obj) >= 0;
}

NS_END
