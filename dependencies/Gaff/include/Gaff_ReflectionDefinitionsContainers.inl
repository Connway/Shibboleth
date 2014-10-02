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

VAR_CONTAINER_CONSTRUCTOR(DoubleContainer, double);
VAR_CONTAINER_CONSTRUCTOR(FloatContainer, float);
VAR_CONTAINER_CONSTRUCTOR(UIntContainer, unsigned int);
VAR_CONTAINER_CONSTRUCTOR(IntContainer, int);
VAR_CONTAINER_CONSTRUCTOR(UShortContainer, unsigned short);
VAR_CONTAINER_CONSTRUCTOR(ShortContainer, short);
VAR_CONTAINER_CONSTRUCTOR(UCharContainer, unsigned char);
VAR_CONTAINER_CONSTRUCTOR(CharContainer, char);
VAR_CONTAINER_CONSTRUCTOR(BoolContainer, bool);
VAR_CONTAINER_CONSTRUCTOR(StringContainer, AString<Allocator>);

VAR_CONTAINER_VAL_TYPE(DoubleContainer, VT_DOUBLE);
VAR_CONTAINER_VAL_TYPE(FloatContainer, VT_FLOAT);
VAR_CONTAINER_VAL_TYPE(UIntContainer, VT_UINT);
VAR_CONTAINER_VAL_TYPE(IntContainer, VT_INT);
VAR_CONTAINER_VAL_TYPE(UShortContainer, VT_USHORT);
VAR_CONTAINER_VAL_TYPE(ShortContainer, VT_SHORT);
VAR_CONTAINER_VAL_TYPE(UCharContainer, VT_UCHAR);
VAR_CONTAINER_VAL_TYPE(CharContainer, VT_CHAR);
VAR_CONTAINER_VAL_TYPE(BoolContainer, VT_BOOL);
VAR_CONTAINER_VAL_TYPE(StringContainer, VT_STRING);

VAR_CONTAINER_READ(DoubleContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isReal());
	object->*_var = json[(const char*)IValueContainer::_key.getBuffer()].getReal();
}

VAR_CONTAINER_WRITE(DoubleContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createReal(object->*_var));
}

VAR_CONTAINER_SET_STRING(DoubleContainer)
{
	object->*_var = atof(value);
}

VAR_CONTAINER_SET_UINT(DoubleContainer)
{
	object->*_var = (double)value;
}

VAR_CONTAINER_SET_INT(DoubleContainer)
{
	object->*_var = (double)value;
}

VAR_CONTAINER_SET_DOUBLE(DoubleContainer)
{
	object->*_var = value;
}




VAR_CONTAINER_READ(FloatContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isReal());
	object->*_var = (float)json[(const char*)IValueContainer::_key.getBuffer()].getReal();
}

VAR_CONTAINER_WRITE(FloatContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createReal(object->*_var));
}

VAR_CONTAINER_SET_STRING(FloatContainer)
{
	object->*_var = (float)atof(value);
}

VAR_CONTAINER_SET_UINT(FloatContainer)
{
	object->*_var = (float)value;
}

VAR_CONTAINER_SET_INT(FloatContainer)
{
	object->*_var = (float)value;
}

VAR_CONTAINER_SET_DOUBLE(FloatContainer)
{
	object->*_var = (float)value;
}




VAR_CONTAINER_READ(UIntContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isReal());
	object->*_var = (unsigned int)json[(const char*)IValueContainer::_key.getBuffer()].getReal();
}

VAR_CONTAINER_WRITE(UIntContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createReal(object->*_var));
}

VAR_CONTAINER_SET_STRING(UIntContainer)
{
	object->*_var = strtoul(value, nullptr, 10);
}

VAR_CONTAINER_SET_UINT(UIntContainer)
{
	object->*_var = value;
}

VAR_CONTAINER_SET_INT(UIntContainer)
{
	object->*_var = (unsigned int)value;
}

VAR_CONTAINER_SET_DOUBLE(UIntContainer)
{
	object->*_var = (unsigned int)value;
}




VAR_CONTAINER_READ(IntContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isInteger());
	object->*_var = json[(const char*)IValueContainer::_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(IntContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createInteger(object->*_var));
}

VAR_CONTAINER_SET_STRING(IntContainer)
{
	object->*_var = atoi(value);
}

VAR_CONTAINER_SET_UINT(IntContainer)
{
	object->*_var = (int)value;
}

VAR_CONTAINER_SET_INT(IntContainer)
{
	object->*_var = value;
}

VAR_CONTAINER_SET_DOUBLE(IntContainer)
{
	object->*_var = (int)value;
}




VAR_CONTAINER_READ(UShortContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isInteger());
	object->*_var = (unsigned short)json[(const char*)IValueContainer::_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(UShortContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createInteger((json_int_t)(object->*_var)));
}

VAR_CONTAINER_SET_STRING(UShortContainer)
{
	object->*_var = (unsigned short)atoi(value);
}

VAR_CONTAINER_SET_UINT(UShortContainer)
{
	object->*_var = (unsigned short)value;
}

VAR_CONTAINER_SET_INT(UShortContainer)
{
	object->*_var = (unsigned short)value;
}

VAR_CONTAINER_SET_DOUBLE(UShortContainer)
{
	object->*_var = (unsigned short)value;
}




VAR_CONTAINER_READ(ShortContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isInteger());
	object->*_var = (short)json[(const char*)IValueContainer::_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(ShortContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createInteger((json_int_t)(object->*_var)));
}

VAR_CONTAINER_SET_STRING(ShortContainer)
{
	object->*_var = (short)atoi(value);
}

VAR_CONTAINER_SET_UINT(ShortContainer)
{
	object->*_var = (short)value;
}

VAR_CONTAINER_SET_INT(ShortContainer)
{
	object->*_var = (short)value;
}

VAR_CONTAINER_SET_DOUBLE(ShortContainer)
{
	object->*_var = (short)value;
}




VAR_CONTAINER_READ(UCharContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isInteger());
	object->*_var = (unsigned char)json[(const char*)IValueContainer::_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(UCharContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createInteger((json_int_t)(object->*_var)));
}

VAR_CONTAINER_SET_STRING(UCharContainer)
{
	object->*_var = (unsigned char)atoi(value);
}

VAR_CONTAINER_SET_UINT(UCharContainer)
{
	object->*_var = (unsigned char)value;
}

VAR_CONTAINER_SET_INT(UCharContainer)
{
	object->*_var = (unsigned char)value;
}

VAR_CONTAINER_SET_DOUBLE(UCharContainer)
{
	object->*_var = (unsigned char)value;
}




VAR_CONTAINER_READ(CharContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isInteger());
	object->*_var = (char)json[(const char*)IValueContainer::_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(CharContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createInteger((json_int_t)(object->*_var)));
}

VAR_CONTAINER_SET_STRING(CharContainer)
{
	object->*_var = (char)atoi(value);
}

VAR_CONTAINER_SET_UINT(CharContainer)
{
	object->*_var = (char)value;
}

VAR_CONTAINER_SET_INT(CharContainer)
{
	object->*_var = (char)value;
}

VAR_CONTAINER_SET_DOUBLE(CharContainer)
{
	object->*_var = (char)value;
}




VAR_CONTAINER_READ(BoolContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isBoolean());
	object->*_var = json[(const char*)IValueContainer::_key.getBuffer()].isTrue();
}

VAR_CONTAINER_WRITE(BoolContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createBoolean(object->*_var));
}

VAR_CONTAINER_SET_STRING(BoolContainer)
{
	object->*_var = atoi(value) != 0;
}

VAR_CONTAINER_SET_UINT(BoolContainer)
{
	object->*_var = value != 0;
}

VAR_CONTAINER_SET_INT(BoolContainer)
{
	object->*_var = value != 0;
}

VAR_CONTAINER_SET_DOUBLE(BoolContainer)
{
	object->*_var = value != 0.0;
}




VAR_CONTAINER_READ(StringContainer)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isString());
	object->*_var = json[(const char*)IValueContainer::_key.getBuffer()].getString();
}

VAR_CONTAINER_WRITE(StringContainer)
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	json.setObject(IValueContainer::_key.getBuffer(), Gaff::JSON::createString((object->*_var).getBuffer()));
}

VAR_CONTAINER_SET_STRING(StringContainer)
{
	object->*_var = value;
}

VAR_CONTAINER_SET_UINT(StringContainer)
{
	char temp[64] = { 0 };
	snprintf(temp, 64, "%u", value);
	object->*_var = temp;
}

VAR_CONTAINER_SET_INT(StringContainer)
{
	char temp[64] = { 0 };
	snprintf(temp, 64, "%i", value);
	object->*_var = temp;
}

VAR_CONTAINER_SET_DOUBLE(StringContainer)
{
	char temp[64] = { 0 };
	snprintf(temp, 64, "%f", value);
	object->*_var = temp;
}




// Object
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::ObjectContainer(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def) :
	IValueContainer(key), _var_ref_def(var_ref_def), _var(var)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::read(const Gaff::JSON& json, T* object)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isObject());
	_var_ref_def.read(json[(const char*)IValueContainer::_key.getBuffer()], object);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::write(Gaff::JSON& json, T* object) const
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	Gaff::JSON obj = Gaff::JSON::createObject();
	_var_ref_def.write(obj, object);
	json.setObject(IValueContainer::_key.getBuffer(), obj);
}

template <class T, class Allocator>
template <class T2>
typename ReflectionDefinition<T, Allocator>::IValueContainer::ValueType ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::getType(void) const
{
	return IValueContainer::VT_OBJECT;
}




// Enum
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::EnumContainer<T2>::EnumContainer(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& var_ref_def):
	IValueContainer(key), _var_ref_def(var_ref_def), _var(var)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::read(const Gaff::JSON& json, T* object)
{
	assert(json[(const char*)IValueContainer::_key.getBuffer()].isString());
	object->*_var = _var_ref_def.getValue(json[(const char*)IValueContainer::_key.getBuffer()].getString());
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::write(Gaff::JSON& json, T* object) const
{
	assert(!json[IValueContainer::_key.getBuffer()]);
	Gaff::JSON val = Gaff::JSON::createString(_var_ref_def.getName(object->*_var));
	json.setObject(IValueContainer::_key.getBuffer(), val);
}

template <class T, class Allocator>
template <class T2>
typename ReflectionDefinition<T, Allocator>::IValueContainer::ValueType ReflectionDefinition<T, Allocator>::EnumContainer<T2>::getType(void) const
{
	return IValueContainer::VT_ENUM;
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::set(const char* value, T* object)
{
	object->*_var = _var_ref_def.getValue(value);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::set(unsigned int value, T* object)
{
	object->*_var = (T2)value;
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::set(int value, T* object)
{
	object->*_var = (T2)value;
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::set(double value, T* object)
{
	// To circumvent compiler error saying it can't convert from
	// double to an enum of type T2.
	object->*_var = (T2)((int)value);
}




// Base
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::BaseValueContainer(const char* key, const typename ReflectionDefinition<T2, Allocator>::ValueContainerPtr& value_ptr):
	IValueContainer(key), _value_ptr(value_ptr)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::read(const Gaff::JSON& json, T* object)
{
	_value_ptr->read(json, object);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::write(Gaff::JSON& json, T* object) const
{
	_value_ptr->write(json, object);
}

template <class T, class Allocator>
template <class T2>
typename ReflectionDefinition<T, Allocator>::IValueContainer::ValueType ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::getType(void) const
{
	return _value_ptr->getType();
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::set(const char* value, T* object)
{
	_value_ptr->set(value, object);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::set(unsigned int value, T* object)
{
	_value_ptr->set(value, object);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::set(int value, T* object)
{
	_value_ptr->set(value, object);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::set(double value, T* object)
{
	_value_ptr->set(value, object);
}
