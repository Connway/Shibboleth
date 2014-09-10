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
	return _values_map.findElementWithValue(value).getKey().getBuffer();
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
ReflectionDefinition<T>& ReflectionDefinition<T>::addBaseClass(const ReflectionDefinition<T2>& base_ref_def)
{
	assert(this != &base_ref_def);

	for (auto it = base_ref_def._value_containers.begin(); it != base_ref_def._value_containers.end(); ++it)
	{
		assert(!_value_containers.hasElementWithKey(it.getKey()));
		BaseValueContainer<T2>* container = GetAllocator()->allocT< BaseValueContainer<T2> >(it.getKey().getBuffer(), it.getValue());
		_value_containers.insert(it.getKey(), ValueContainerPtr(container));
	}

	return *this;
}

template <class T>
template <class T2>
ReflectionDefinition<T>& ReflectionDefinition<T>::addObject(const char* key, T2 T::* var, ReflectionDefinition<T2>& var_ref_def)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ObjectContainer<T2>* container = GetAllocator()->allocT< ObjectContainer<T2> >(key, var, var_ref_def);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
template <class T2>
ReflectionDefinition<T>& ReflectionDefinition<T>::addEnum(const char* key, T2 T::* var, const EnumReflectionDefinition<T2>& ref_def)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	EnumContainer<T2>* container = GetAllocator()->allocT< EnumContainer<T2> >(key, var, ref_def);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addDouble(const char* key, double T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	DoubleContainer* container = GetAllocator()->allocT<DoubleContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addFloat(const char* key, float T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	FloatContainer* container = GetAllocator()->allocT<FloatContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addUInt(const char* key, unsigned int T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UIntContainer* container = GetAllocator()->allocT<UIntContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addInt(const char* key, int T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	IntContainer* container = GetAllocator()->allocT<IntContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addUShort(const char* key, unsigned short T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UShortContainer* container = GetAllocator()->allocT<UShortContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addShort(const char* key, short T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ShortContainer* container = GetAllocator()->allocT<ShortContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addUChar(const char* key, unsigned char T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UCharContainer* container = GetAllocator()->allocT<UCharContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addChar(const char* key, char T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	CharContainer* container = GetAllocator()->allocT<CharContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addBool(const char* key, bool T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	BoolContainer* container = GetAllocator()->allocT<BoolContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addString(const char* key, AString T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	StringContainer* container = GetAllocator()->allocT<StringContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return *this;
}

template <class T>
ReflectionDefinition<T>& ReflectionDefinition<T>::addCustom(const char* key, IValueContainer* container)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
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
