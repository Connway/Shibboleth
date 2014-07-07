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

// Enum Reflection Definition
template <class T>
EnumReflectionDefinition<T>::EnumReflectionDefinition(void):
	_defined(false)
{
}

template <class T>
EnumReflectionDefinition<T>::~EnumReflectionDefinition(void)
{
}

template <class T>
EnumReflectionDefinition<T>& EnumReflectionDefinition<T>::addValue(const char* name, T value)
{
	assert(!_values_map.hasElementWithKey(name));
	_values_map.insert(name, value);
	return *this;
}

template <class T>
const char* EnumReflectionDefinition<T>::getName(T value) const
{
	assert(_values_map.hasElementWithValue(value));
	return _values_map.findElementWithValue(value)->first.getBuffer();
}

template <class T>
T EnumReflectionDefinition<T>::getValue(const char* name) const
{
	assert(_values_map.hasElementWithKey(name));
	return _values_map[name];
}

template <class T>
void EnumReflectionDefinition<T>::setAllocator(const ProxyAllocator& allocator)
{
	_values_map.setAllocator(allocator);
}

template <class T>
bool EnumReflectionDefinition<T>::isDefined(void) const
{
	return _defined;
}

template <class T>
void EnumReflectionDefinition<T>::markDefined(void)
{
	_defined = true;
}

VAR_CONTAINER_CONSTRUCTOR(DoubleContainer, double);
VAR_CONTAINER_CONSTRUCTOR(FloatContainer, float);
VAR_CONTAINER_CONSTRUCTOR(UIntContainer, unsigned int);
VAR_CONTAINER_CONSTRUCTOR(IntContainer, int);
VAR_CONTAINER_CONSTRUCTOR(UShortContainer, unsigned short);
VAR_CONTAINER_CONSTRUCTOR(ShortContainer, short);
VAR_CONTAINER_CONSTRUCTOR(UCharContainer, unsigned char);
VAR_CONTAINER_CONSTRUCTOR(CharContainer, char);
VAR_CONTAINER_CONSTRUCTOR(BoolContainer, bool);

VAR_CONTAINER_VAL_TYPE(DoubleContainer, VT_DOUBLE);
VAR_CONTAINER_VAL_TYPE(FloatContainer, VT_FLOAT);
VAR_CONTAINER_VAL_TYPE(UIntContainer, VT_UINT);
VAR_CONTAINER_VAL_TYPE(IntContainer, VT_INT);
VAR_CONTAINER_VAL_TYPE(UShortContainer, VT_USHORT);
VAR_CONTAINER_VAL_TYPE(ShortContainer, VT_SHORT);
VAR_CONTAINER_VAL_TYPE(UCharContainer, VT_UCHAR);
VAR_CONTAINER_VAL_TYPE(CharContainer, VT_CHAR);
VAR_CONTAINER_VAL_TYPE(BoolContainer, VT_BOOL);

VAR_CONTAINER_READ(DoubleContainer)
{
	assert(json[(const char*)_key.getBuffer()].isReal());
	object->*_var = json[(const char*)_key.getBuffer()].getReal();
}

VAR_CONTAINER_WRITE(DoubleContainer)
{
	assert(!json[_key.getBuffer()])
		json.setObject(_key.getBuffer(), Gaff::JSON::createReal(object->*_var));
}

VAR_CONTAINER_READ(FloatContainer)
{
	assert(json[(const char*)_key.getBuffer()].isReal());
	object->*_var = (float)json[(const char*)_key.getBuffer()].getReal();
}

VAR_CONTAINER_WRITE(FloatContainer)
{
	assert(!json[_key.getBuffer()]);
	json.setObject(_key.getBuffer(), Gaff::JSON::createReal(object->*_var));
}

VAR_CONTAINER_READ(UIntContainer)
{
	assert(json[_key.getBuffer()].isReal());
	object->*_var = (unsigned int)json[_key.getBuffer()].getReal();
}

VAR_CONTAINER_WRITE(UIntContainer)
{
	assert(!json[_key.getBuffer()]);
	json.setObject(_key.getBuffer(), Gaff::JSON::createReal(object->*_var));
}

VAR_CONTAINER_READ(IntContainer)
{
	assert(json[_key.getBuffer()].isInteger());
	object->*_var = json[_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(IntContainer)
{
	assert(!json[_key.getBuffer()]);
	json.setObject(_key.getBuffer(), Gaff::JSON::createInteger(object->*_var));
}

VAR_CONTAINER_READ(UShortContainer)
{
	assert(json[_key.getBuffer()].isInteger());
	object->*_var = (unsigned short)json[_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(UShortContainer)
{
	assert(!json[_key.getBuffer()]);
	json.setObject(_key.getBuffer(), Gaff::JSON::createInteger((json_int_t)(object->*_var)));
}

VAR_CONTAINER_READ(ShortContainer)
{
	assert(json[(const char*)_key.getBuffer()].isInteger());
	object->*_var = (short)json[(const char*)_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(ShortContainer)
{
	assert(!json[_key.getBuffer()]);
	json.setObject(_key.getBuffer(), Gaff::JSON::createInteger((json_int_t)(object->*_var)));
}

VAR_CONTAINER_READ(UCharContainer)
{
	assert(json[(const char*)_key.getBuffer()].isInteger());
	object->*_var = (unsigned char)json[(const char*)_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(UCharContainer)
{
	assert(!json[_key.getBuffer()]);
	json.setObject(_key.getBuffer(), Gaff::JSON::createInteger((json_int_t)(object->*_var)));
}

VAR_CONTAINER_READ(CharContainer)
{
	assert(json[(const char*)_key.getBuffer()].isInteger());
	object->*_var = (char)json[(const char*)_key.getBuffer()].getInteger();
}

VAR_CONTAINER_WRITE(CharContainer)
{
	assert(!json[_key.getBuffer()]);
	json.setObject(_key.getBuffer(), Gaff::JSON::createInteger((json_int_t)(object->*_var)));
}

VAR_CONTAINER_READ(BoolContainer)
{
	assert(json[(const char*)_key.getBuffer()].isBoolean());
	object->*_var = json[(const char*)_key.getBuffer()].isTrue();
}

VAR_CONTAINER_WRITE(BoolContainer)
{
	assert(!json[_key.getBuffer()]);
	json.setObject(_key.getBuffer(), Gaff::JSON::createBoolean(object->*_var));
}

// Object
template <class T>
template <class T2>
ReflectionDefinition<T>::ObjectContainer<T2>::ObjectContainer(const char* key, T2 T::* var, ReflectionDefinition<T2> T2::* var_ref_def):
	IValueContainer(key), _var_ref_def(var_ref_def), _var(var)
{
}

template <class T>
template <class T2>
void ReflectionDefinition<T>::ObjectContainer<T2>::read(const Gaff::JSON& json, T* object)
{
	assert(json[(const char*)_key.getBuffer()].isObject());
	((object->*_var)->*_var_ref_def).read(json[(const char*)_key.getBuffer()], _object);
}

template <class T>
template <class T2>
void ReflectionDefinition<T>::ObjectContainer<T2>::write(Gaff::JSON& json, T* object) const
{
	assert(!json[_key.getBuffer()]);
	Gaff::JSON object = Gaff::JSON::createObject();
	((object->*_var)->*_var_ref_def).write(object, _object);
	json.setObject(_key.getBuffer(), object);
}

template <class T>
template <class T2>
typename ReflectionDefinition<T>::IValueContainer::ValueType ReflectionDefinition<T>::ObjectContainer<T2>::getType(void) const
{
	return VT_OBJECT;
}



// Enum
template <class T>
template <class T2>
ReflectionDefinition<T>::EnumContainer<T2>::EnumContainer(const char* key, T2 T::* var, const EnumReflectionDefinition<T2>& var_ref_def):
	IValueContainer(key), _var_ref_def(var_ref_def), _var(var)
{
}

template <class T>
template <class T2>
void ReflectionDefinition<T>::EnumContainer<T2>::read(const Gaff::JSON& json, T* object)
{
	assert(json[(const char*)_key.getBuffer()].isString());
	object->*_var = _var_ref_def.getValue(json[(const char*)_key.getBuffer()].getString());
}

template <class T>
template <class T2>
void ReflectionDefinition<T>::EnumContainer<T2>::write(Gaff::JSON& json, T* object) const
{
	assert(!json[_key.getBuffer()]);
	Gaff::JSON val = Gaff::JSON::createString(_var_ref_def.getName(object->*_var));
	json.setObject(_key.getBuffer(), val);
}

template <class T>
template <class T2>
typename ReflectionDefinition<T>::IValueContainer::ValueType ReflectionDefinition<T>::EnumContainer<T2>::getType(void) const
{
	return VT_ENUM;
}



// Reflection Definition
template <class T>
ReflectionDefinition<T>::ReflectionDefinition(void) :
_defined(false)
{
}

template <class T>
ReflectionDefinition<T>::~ReflectionDefinition(void)
{
}

template <class T>
void ReflectionDefinition<T>::read(const Gaff::JSON& json, T* object)
{
	for (auto it = _value_containers.begin(); it != _value_containers.end(); ++it) {
		(*it)->read(json, object);
	}
}

template <class T>
void ReflectionDefinition<T>::write(Gaff::JSON& json, T* object) const
{
	for (auto it = _value_containers.begin(); it != _value_containers.end(); ++it) {
		(*it)->write(json, object);
	}
}

template <class T>
template <class T2>
ReflectionDefinition<T>& ReflectionDefinition<T>::addObject(const char* key, T2 T::* var, ReflectionDefinition<T2> T2::* var_ref_def)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	ObjectContainer<T2>* container = GetAllocator().allocT< ObjectContainer<T2> >(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
template <class T2>
ReflectionDefinition<T>& ReflectionDefinition<T>::addEnum(const char* key, T2 T::* var, const EnumReflectionDefinition<T2>& ref_def)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	EnumContainer<T2>* container = GetAllocator().allocT< EnumContainer<T2> >(key, var, ref_def);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addDouble(const char* key, double T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	DoubleContainer* container = GetAllocator().allocT<DoubleContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addFloat(const char* key, float T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	FloatContainer* container = GetAllocator().allocT<FloatContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addUInt(const char* key, unsigned int T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	UIntContainer* container = GetAllocator().allocT<UIntContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addInt(const char* key, int T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	IntContainer* container = GetAllocator().allocT<IntContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addUShort(const char* key, unsigned short T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	UShortContainer* container = GetAllocator().allocT<UShortContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addShort(const char* key, short T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	ShortContainer* container = GetAllocator().allocT<ShortContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addUChar(const char* key, unsigned char T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	UCharContainer* container = GetAllocator().allocT<UCharContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addChar(const char* key, char T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	CharContainer* container = GetAllocator().allocT<CharContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addBool(const char* key, bool T::* var)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	BoolContainer* container = GetAllocator().allocT<BoolContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addCustom(const char* key, IValueContainer* container)
{
	assert(key && strlen(key) && _value_containers.indexOf(key) == -1);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
bool ReflectionDefinition<T>::isDefined(void) const
{
	return _defined;
}

template <class T>
void ReflectionDefinition<T>::markDefined(void)
{
	_defined = true;
}

template <class T>
void ReflectionDefinition<T>::setAllocator(const ProxyAllocator& allocator)
{
	_value_containers.setAllocator(allocator);
}
