/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

ARRAY_CONTAINER_CONSTRUCTOR(ArrayDoubleContainer, double);

ARRAY_CONTAINER_VAL_TYPE(ArrayDoubleContainer, VT_DOUBLE);

VAR_CONTAINER_READ(DoubleContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isReal());

	if (_var) {
		object->*_var = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal();
	} else if (_setter) {
		(object->*_setter)(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal());
	}
}

VAR_CONTAINER_WRITE(DoubleContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal(object->*_var));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal((object->*_getter)()));
	}
}

VAR_CONTAINER_GET(DoubleContainer)
{
	if (_var) {
		*reinterpret_cast<double*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<double*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(DoubleContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const double*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const double*>(value));
	}
}





VAR_CONTAINER_READ(FloatContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isReal());

	if (_var) {
		object->*_var = static_cast<float>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal());
	} else if (_setter) {
		(object->*_setter)(static_cast<float>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal()));
	}
}

VAR_CONTAINER_WRITE(FloatContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal(object->*_var));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal((object->*_getter)()));
	}
}

VAR_CONTAINER_GET(FloatContainer)
{
	if (_var) {
		*reinterpret_cast<float*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<float*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(FloatContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const float*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const float*>(value));
	}
}




VAR_CONTAINER_READ(UIntContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isReal());

	if (_var) {
		object->*_var = static_cast<unsigned int>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal());
	} else if (_setter) {
		(object->*_setter)(static_cast<unsigned int>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getReal()));
	}
}

VAR_CONTAINER_WRITE(UIntContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal(object->*_var));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createReal((object->*_getter)()));
	}
}

VAR_CONTAINER_GET(UIntContainer)
{
	if (_var) {
		*reinterpret_cast<unsigned int*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<unsigned int*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(UIntContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const unsigned int*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const unsigned int*>(value));
	}
}




VAR_CONTAINER_READ(IntContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger();
	} else if (_setter) {
		(object->*_setter)(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	}
}

VAR_CONTAINER_WRITE(IntContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(object->*_var));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger((object->*_getter)()));
	}
}

VAR_CONTAINER_GET(IntContainer)
{
	if (_var) {
		*reinterpret_cast<int*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<int*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(IntContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const int*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const int*>(value));
	}
}




VAR_CONTAINER_READ(UShortContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<unsigned short>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<unsigned short>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(UShortContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}

VAR_CONTAINER_GET(UShortContainer)
{
	if (_var) {
		*reinterpret_cast<unsigned short*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<unsigned short*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(UShortContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const unsigned short*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const unsigned short*>(value));
	}
}




VAR_CONTAINER_READ(ShortContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<short>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<short>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(ShortContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}

VAR_CONTAINER_GET(ShortContainer)
{
	if (_var) {
		*reinterpret_cast<short*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<short*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(ShortContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const short*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const short*>(value));
	}
}




VAR_CONTAINER_READ(UCharContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<unsigned char>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<unsigned char>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(UCharContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}

VAR_CONTAINER_GET(UCharContainer)
{
	if (_var) {
		*reinterpret_cast<unsigned char*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<unsigned char*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(UCharContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const unsigned char*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const unsigned char*>(value));
	}
}




VAR_CONTAINER_READ(CharContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isInteger());

	if (_var) {
		object->*_var = static_cast<char>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger());
	} else if (_setter) {
		(object->*_setter)(static_cast<char>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getInteger()));
	}
}

VAR_CONTAINER_WRITE(CharContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>(object->*_var)));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createInteger(static_cast<json_int_t>((object->*_getter)())));
	}
}

VAR_CONTAINER_GET(CharContainer)
{
	if (_var) {
		*reinterpret_cast<char*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<char*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(CharContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const char*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const char*>(value));
	}
}




VAR_CONTAINER_READ(BoolContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isBoolean());

	if (_var) {
		object->*_var = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isTrue();
	} else if (_setter) {
		(object->*_setter)(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isTrue());
	}
}

VAR_CONTAINER_WRITE(BoolContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createBoolean(object->*_var));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createBoolean((object->*_getter)()));
	}
}

VAR_CONTAINER_GET(BoolContainer)
{
	if (_var) {
		*reinterpret_cast<bool*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<bool*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(BoolContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const bool*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const bool*>(value));
	}
}




VAR_CONTAINER_READ(StringContainer)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isString());

	if (_var) {
		object->*_var = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getString();
	} else if (_setter) {
		(object->*_setter)(AString<Allocator>(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getString()));
	}
}

VAR_CONTAINER_WRITE(StringContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createString((object->*_var).getBuffer()));
	} else if (_getter) {
		json.setObject(ValueContainerBase::_key.getBuffer(), JSON::createString((object->*_getter)().getBuffer()));
	}
}

VAR_CONTAINER_GET(StringContainer)
{
	if (_var) {
		*reinterpret_cast<AString<Allocator>*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<AString<Allocator>*>(out) = (object->*_getter)();
	}
}

VAR_CONTAINER_SET(StringContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const AString<Allocator>*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const AString<Allocator>*>(value));
	}
}




ARRAY_CONTAINER_READ(ArrayDoubleContainer)
{
	JSON array = json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())];
	assert(array.isArray());

	if (_var) {
		(object->*_var).resize(array.size());

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isReal());
			(object->*_var)[index] = value.getReal();
			return false;
		});

	} else if (_setter) {
		Array<double, Allocator> temp(array.size(), 0, _allocator);

		array.forEachInArray([&](size_t index, const JSON& value) -> bool
		{
			assert(value.isReal());
			temp[index] = value.getReal();
			return false;
		});

		(object->*_setter)(temp);
	}
}

ARRAY_CONTAINER_WRITE(ArrayDoubleContainer)
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	JSON array = JSON::createArray();

	if (_var) {
		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createReal((object->*_var)[i]));
		}

	} else if (_getter) {
		const Array<double, Allocator>& value = (object->*_getter)();

		for (size_t i = 0; i < (object->*_var).size(); ++i) {
			array.setObject(i, JSON::createReal(value[i]));
		}
	}

	json.setObject(ValueContainerBase::_key.getBuffer(), array);
}

ARRAY_CONTAINER_GET(ArrayDoubleContainer)
{
	if (_var) {
		*reinterpret_cast<Array<double, Allocator>*>(out) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<Array<double, Allocator>*>(out) = (object->*_getter)();
	}
}

ARRAY_CONTAINER_SET(ArrayDoubleContainer)
{
	if (_var) {
		object->*_var = *reinterpret_cast<Array<double, Allocator>*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<Array<double, Allocator>*>(value));
	}
}




// Object
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::ObjectContainer(const char* key, ReflectionDefinition<T2, Allocator>& var_ref_def, Getter getter, Setter setter, const Allocator& allocator):
	ValueContainerBase(key, allocator), _getter(getter), _setter(setter), _var_ref_def(var_ref_def), _var(nullptr)
{
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::ObjectContainer(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def, const Allocator& allocator) :
	ValueContainerBase(key, allocator), _getter(nullptr), _setter(nullptr), _var_ref_def(var_ref_def), _var(var)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::read(const JSON& json, T* object)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isObject());

	if (_var) {
		_var_ref_def.read(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())], &(object->*_var));
	} else if (_setter) {
		T2 value;
		_var_ref_def.read(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())], &value);
		(object->*_setter)(value);
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::write(JSON& json, T* object) const
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		JSON obj = JSON::createObject();
		_var_ref_def.write(obj, &(object->*_var));
		json.setObject(ValueContainerBase::_key.getBuffer(), obj);
	} else if (_getter) {
		T2* value = &const_cast<T2&>((object->*_getter)());

		JSON obj = JSON::createObject();
		_var_ref_def.write(obj, value);
		json.setObject(ValueContainerBase::_key.getBuffer(), obj);
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::get(void* out, const T* object) const
{
	if (_var) {
		*reinterpret_cast<T2*>(value) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<T2*>(value) = (object->*_getter)();
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::set(const void* value, T* object)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const T2*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const T2*>(value));
	}
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::isFixedArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::isArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::ObjectContainer<T2>::getType(void) const
{
	return VT_OBJECT;
}




// Enum
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::EnumContainer<T2>::EnumContainer(const char* key, const EnumReflectionDefinition<T2, Allocator>& var_ref_def, Getter getter, Setter setter, const Allocator& allocator):
	ValueContainerBase(key, allocator), _getter(getter), _setter(setter), _var_ref_def(var_ref_def), _var(nullptr)
{
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::EnumContainer<T2>::EnumContainer(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& var_ref_def, const Allocator& allocator):
	ValueContainerBase(key, allocator), _getter(nullptr), _setter(nullptr), _var_ref_def(var_ref_def), _var(var)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::read(const JSON& json, T* object)
{
	assert(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].isString());

	if (_var) {
		object->*_var = _var_ref_def.getValue(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getString());
	} else if (_setter) {
		T2 value = _var_ref_def.getValue(json[reinterpret_cast<const char*>(ValueContainerBase::_key.getBuffer())].getString());
		(object->*_setter)(value);
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::write(JSON& json, T* object) const
{
	assert(!json[ValueContainerBase::_key.getBuffer()]);

	if (_var) {
		JSON val = JSON::createString(_var_ref_def.getName(object->*_var));
		json.setObject(ValueContainerBase::_key.getBuffer(), val);
	} else if (_getter) {
		JSON val = JSON::createString(_var_ref_def.getName((object->*_getter)()));
		json.setObject(ValueContainerBase::_key.getBuffer(), val);
	}
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::EnumContainer<T2>::isFixedArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::EnumContainer<T2>::isArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::EnumContainer<T2>::getType(void) const
{
	return VT_ENUM;
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::get(void* out, const T* object) const
{
	if (_var) {
		*reinterpret_cast<T2*>(value) = (object->*_var);
	} else if (_getter) {
		*reinterpret_cast<T2*>(value) = (object->*_getter)();
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::EnumContainer<T2>::set(const void* value, T* object)
{
	if (_var) {
		object->*_var = *reinterpret_cast<const T2*>(value);
	} else if (_setter) {
		(object->*_setter)(*reinterpret_cast<const T2*>(value));
	}
}





// Base
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::BaseValueContainer(const char* key, const typename ReflectionDefinition<T2, Allocator>::ValueContainerPtr& value_ptr, const Allocator& allocator):
	ValueContainerBase(key, allocator), _value_ptr(value_ptr)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::read(const JSON& json, T* object)
{
	_value_ptr->read(json, object);
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::write(JSON& json, T* object) const
{
	_value_ptr->write(json, object);
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::isFixedArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
bool ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::isArray(void) const
{
	return false;
}

template <class T, class Allocator>
template <class T2>
ReflectionValueType ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::getType(void) const
{
	return _value_ptr->getType();
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::get(void* out, const void* object) const
{
	get(out, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::get(void* out, const T* object) const
{
	_value_ptr->get(out, static_cast<const T2*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::set(const void* value, void* object)
{
	set(value, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::BaseValueContainer<T2>::set(const void* value, T* object)
{
	_value_ptr->set(value, static_cast<T2*>(object));
}
