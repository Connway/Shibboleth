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

template <class T>
bool setMessageCallback(T* object, void (T::*msgCallback)(const asSMessageInfo* msg) const)
{
	assert(object && msgCallback);

	return _engine->SetMessageCallback(
		asSMethodPtr<msgCallback>::Convert(msgCallback),
		object,
		asCALL_THISCALL
		) >= 0;
}

template <class T>
bool setMessageCallback(T* object, void (T::*msgCallback)(const asSMessageInfo* msg))
{
	assert(object && msgCallback);

	return _engine->SetMessageCallback(
		asSMethodPtr<msgCallback>::Convert(msgCallback),
		object,
		asCALL_THISCALL
		) >= 0;
}

template <class Allocator>
INLINE bool startModule(const String<char, Allocator>& module_name)
{
	assert(module_name.size());
	return startModule(module_name.getBuffer());
}

template <class Allocator>
INLINE bool addScript(const String<char, Allocator>& file_name)
{
	assert(file_name.size());
	return addScript(file_name.getBuffer());
}

#ifdef _UNICODE
template <class Allocator>
INLINE bool addScript(const String<wchar_t, Allocator>& file_name, const String<char, Allocator>& section_name)
{
	assert(file_name.size() && section_name.size());
	return addScript(file_name.getBuffer(), section_name.getBuffer());
}

template <class Allocator>
INLINE bool addScript(const String<wchar_t, Allocator>& file_name, const char* section_name)
{
	assert(file_name.size() && section_name && strlen(section_name));
	return addScript(file_name.getBuffer(), section_name);
}
#endif

template <class Allocator>
INLINE AngelScriptModule getModule(const String<char, Allocator>& module_name)
{
	assert(module_name.size());
	return getModule(module_name.getBuffer());
}

template <class Function, class Allocator>
INLINE bool registerGlobalFunction(const String<char, Allocator>& declaration, Function function, asECallConvTypes call_type = asCALL_CDECL, void* this_obj = nullptr)
{
	return registerGlobalFunctionHelper(declaration.getBuffer(), asFUNCTION(function), call_type, this_obj);
}

template <class Function>
INLINE bool registerGlobalFunction(const char* declaration, Function function, asECallConvTypes call_type = asCALL_CDECL, void* this_obj = nullptr)
{
	return registerGlobalFunctionHelper(declaration, asFUNCTION(function), call_type, this_obj);
}

template <class Allocator>
INLINE bool registerEnum(const String<char, Allocator>& type)
{
	assert(type.size());
	return registerEnum(type.getBuffer());
}

template <class Allocator>
INLINE bool registerEnumValue(const String<char, Allocator>& type, const String<char, Allocator>& name, int value)
{
	assert(type.size() && name.size());
	return registerEnumValue(type.getBuffer(), name.getBuffer(), value);
}

template <class Allocator>
INLINE bool registerEnumValue(const String<char, Allocator>& type, const char* name, int value)
{
	assert(type.size() && name && strlen(name));
	return registerEnumValue(type.getBuffer(), name, value);
}

template <class Allocator>
INLINE bool registerGlobalProperty(const String<char, Allocator>& declaration, void* object)
{
	assert(declaration.size() && object);
	return registerEnumValue(declaration.getBuffer(), object);
}

// REGISTER OBJECT TYPE
template <class T, class Allocator>
INLINE bool registerObjectType(const String<char, Allocator>& name, unsigned long type_flags)
{
	assert(name.size() && type_flags != 0);
	return registerObjectType(name.getBuffer(), sizeof(T), type_flags);
}

template <class T>
INLINE bool registerObjectType(const char* name, unsigned long type_flags)
{
	assert(name && strlen(name) && type_flags != 0);
	return registerObjectType(name, sizeof(T), type_flags);
}

// REGISTER OBJECT PROPERTY
template <class Allocator>
INLINE bool registerObjectProperty(const String<char, Allocator>& object, const String<char, Allocator>& declaration, int byte_offset)
{
	assert(object.size() && declaration.size());
	return registerObjectProperty(object.getBuffer(), declaration.getBuffer(), byte_offset);
}

template <class Allocator>
INLINE bool registerObjectProperty(const String<char, Allocator>& object, const char* declaration, int byte_offset)
{
	assert(object.size() && declaration && strlen(declaration));
	return registerObjectProperty(object.getBuffer(), declaration, byte_offset);
}

// REGISTER OBJECT METHOD

// only use for non-method functions
template <class Function, class Allocator>
INLINE bool registerObjectMethod(const String<char, Allocator>& object, const String<char, Allocator>& declaration, Function function, asECallConvTypes call_type)
{
	assert(object.size() && declaration.size());
	return registerObjectMethodHelper(object.getBuffer(), declaration.getBuffer(), asFUNCTION(function), call_type);
}

template <class Function, class Allocator>
INLINE bool registerObjectMethod(const String<char, Allocator>& object, const String<char, Allocator>& declaration, Function function)
{
	assert(object.size() && declaration.size());
	return registerObjectMethodHelper(object.getBuffer(), declaration.getBuffer(), asSMethodPtr<sizeof(Function)>::Convert(function));
}

// only use for non-method functions
template <class Function, class Allocator>
INLINE bool registerObjectMethod(const String<char, Allocator>& object, const char* declaration, Function function, asECallConvTypes call_type)
{
	assert(object.size() && declaration && strlen(declaration));
	return registerObjectMethodHelper(object.getBuffer(), declaration, asFUNCTION(function), call_type);
}

template <class Function, class Allocator>
INLINE bool registerObjectMethod(const String<char, Allocator>& object, const char* declaration, Function function)
{
	assert(object.size() && declaration && strlen(declaration));
	return registerObjectMethodHelper(object.getBuffer(), declaration, asSMethodPtr<sizeof(Function)>::Convert(function));
}

// only use for non-method functions
template <class Function>
INLINE bool registerObjectMethod(const char* object, const char* declaration, Function function, asECallConvTypes call_type)
{
	assert(object && declaration && strlen(object) && strlen(declaration));
	return registerObjectMethodHelper(object, declaration, asFUNCTION(function), call_type);
}

template <class Function>
INLINE bool registerObjectMethod(const char* object, const char* declaration, Function function)
{
	assert(object && declaration && strlen(object) && strlen(declaration));
	return registerObjectMethodHelper(object, declaration, asSMethodPtr<sizeof(Function)>::Convert(function), asCALL_THISCALL);
}

// REGISTER OBJECT BEHAVIOUR
template <class Function, class Allocator>
INLINE bool registerObjectBehaviour(const String<char, Allocator>& object, const String<char, Allocator>& declaration,
	Function function, asEBehaviours behaviour, asECallConvTypes call_type, void* this_obj = nullptr)
{
	assert(object.size() && declaration.size());
	return registerObjectBehaviourHelper(object.getBuffer(), declaration.getBuffer(), asFUNCTION(function), behaviour, call_type, this_obj);
}

template <class Function, class Allocator>
INLINE bool registerObjectBehaviour(const String<char, Allocator>& object, const String<char, Allocator>& declaration,
	Function function, asEBehaviours behaviour)
{
	assert(object.size() && declaration.size());
	return registerObjectBehaviourHelper(object.getBuffer(), declaration.getBuffer(), asSMethodPtr<sizeof(Function)>::Convert(function), behaviour, asCALL_THISCALL, nullptr);
}

template <class Function, class Allocator>
INLINE bool registerObjectBehaviour(const String<char, Allocator>& object, const char* declaration, Function function,
	asEBehaviours behaviour, asECallConvTypes call_type, void* this_obj = nullptr)
{
	assert(object.size() && declaration && strlen(declaration));
	return registerObjectBehaviourHelper(object.getBuffer(), declaration, asFUNCTION(function), behaviour, call_type, this_obj);
}

template <class Function, class Allocator>
INLINE bool registerObjectBehaviour(const String<char, Allocator>& object, const char* declaration, Function function, asEBehaviours behaviour)
{
	assert(object.size() && declaration && strlen(declaration));
	return registerObjectBehaviourHelper(object.getBuffer(), declaration, asSMethodPtr<sizeof(Function)>::Convert(function), behaviour, asCALL_THISCALL, nullptr);
}

template <class Function>
INLINE bool registerObjectBehaviour(const char* object, const char* declaration, Function function,
	asEBehaviours behaviour, asECallConvTypes call_type, void* this_obj = nullptr)
{
	assert(object && declaration && strlen(object) && strlen(declaration));
	return registerObjectBehaviourHelper(object, declaration, asFUNCTION(function), behaviour, call_type, this_obj);
}

template <class Function>
INLINE bool registerObjectBehaviour(const char* object, const char* declaration, Function function, asEBehaviours behaviour)
{
	assert(object && declaration && strlen(object) && strlen(declaration));
	return registerObjectBehaviourHelper(object, declaration, asSMethodPtr<sizeof(Function)>::Convert(function), behaviour, asCALL_THISCALL, nullptr);
}

template <class Allocator>
INLINE bool registerInterface(const String<char, Allocator>& name)
{
	assert(name.size());
	return registerInterface(name.getBuffer());
}

template <class Allocator>
INLINE bool registerInterfaceMethod(const String<char, Allocator>& interface_name, const char* declaration)
{
	assert(interface_name.size() && declaration && strlen(declaration));
	return registerInterfaceMethod(interface_name.getBuffer(), declaration);
}

template <class Allocator>
INLINE bool registerInterfaceMethod(const String<char, Allocator>& interface_name, const String<char, Allocator>& declaration)
{
	assert(interface_name.size() && declaration.size());
	return registerInterfaceMethod(interface_name.getBuffer(), declaration.getBuffer());
}

template <class Allocator>
INLINE bool registerFunctionDefinition(const String<char, Allocator>& declaration)
{
	assert(declaration.size());
	return registerFunctionDefinition(declaration.getBuffer());
}

template <class Allocator>
INLINE bool registerTypedef(const String<char, Allocator>& type, const String<char, Allocator>& declaration)
{
	assert(type.size() && declaration.size());
	return registerTypedef(type.getBuffer(), declaration.getBuffer());
}

template <class Allocator>
INLINE bool registerTypedef(const String<char, Allocator>& type, const char* declaration)
{
	assert(type.size() && declaration && strlen(declaration));
	return registerTypedef(type.getBuffer(), declaration);
}

template <class Allocator>
INLINE bool registerDefaultArrayType(const String<char, Allocator>& type)
{
	assert(type.size());
	return registerDefaultArrayType(type.getBuffer());
}

template <class Function, class Allocator>
INLINE bool registerStringFactory(const String<char, Allocator>& type, Function function, asECallConvTypes call_type = asCALL_CDECL, void* this_obj = nullptr)
{
	return registerStringFactoryHelper(type.getBuffer(), asFUNCTION(function), call_type, this_obj);
}

template <class Function>
INLINE bool registerStringFactory(const char* type, Function function, asECallConvTypes call_type = asCALL_CDECL, void* this_obj = nullptr)
{
	return registerStringFactoryHelper(type, asFUNCTION(function), call_type, this_obj);
}
