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
template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>::EnumReflectionDefinition(void):
	_defined(false)
{
}

template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>::~EnumReflectionDefinition(void)
{
}

template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>&& EnumReflectionDefinition<T, Allocator>::addValue(const char* name, T value)
{
	assert(!_values_map.hasElementWithKey(name));
	_values_map.insert(name, value);
	return Move(*this);
}

template <class T, class Allocator>
const char* EnumReflectionDefinition<T, Allocator>::getName(T value) const
{
	assert(_values_map.hasElementWithValue(value));
	return _values_map.findElementWithValue(value).getKey().getBuffer();
}

template <class T, class Allocator>
T EnumReflectionDefinition<T, Allocator>::getValue(const char* name) const
{
	assert(_values_map.hasElementWithKey(name));
	return _values_map[name];
}

template <class T, class Allocator>
void EnumReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_values_map.setAllocator(allocator);
}

template <class T, class Allocator>
void EnumReflectionDefinition<T, Allocator>::clear(void)
{
	_values_map.clear();
}

template <class T, class Allocator>
bool EnumReflectionDefinition<T, Allocator>::isDefined(void) const
{
	return _defined;
}

template <class T, class Allocator>
void EnumReflectionDefinition<T, Allocator>::markDefined(void)
{
	_defined = true;
}



// Reflection Definition
template <class T, class Allocator>
ReflectionDefinition<T, Allocator>::ReflectionDefinition(ReflectionDefinition<T, Allocator>&& ref_def):
	_value_containers(Move(ref_def._value_containers)), _base_ids(Move(ref_def._base_ids)),
	_on_complete_callbacks(Move(ref_def._on_complete_callbacks)), _allocator(ref_def._allocator),
	_base_classes_remaining(ref_def._base_classes_remaining)
{
	if (!_base_classes_remaining) {
		markDefined();
	}
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>::ReflectionDefinition(void):
	_base_classes_remaining(0), _defined(false)
{
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>::~ReflectionDefinition(void)
{
}

template <class T, class Allocator>
const ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::operator=(ReflectionDefinition<T, Allocator>&& rhs)
{
	_value_containers = Move(rhs._value_containers);
	_base_ids = Move(rhs._base_ids);
	_on_complete_callbacks = Move(rhs._on_complete_callbacks);
	_allocator = rhs._allocator;
	_base_classes_remaining = rhs._base_classes_remaining;

	if (!_base_classes_remaining) {
		markDefined();
	}

	return *this;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::read(const Gaff::JSON& json, void* object)
{
	read(json, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::write(Gaff::JSON& json, void* object) const
{
	write(json, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
void* ReflectionDefinition<T, Allocator>::getInterface(unsigned int class_id, const void* object) const
{
	auto it = _base_ids.binarySearch(_base_ids.begin(), _base_ids.end(), class_id,
		[](const Gaff::Pair< unsigned int, Gaff::FunctionBinder<void*, const void*> >& lhs, unsigned int rhs) -> bool
		{
			return lhs.first < rhs;
		});

	return (it != _base_ids.end() && it->first == class_id) ? it->second(object) : nullptr;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::read(const Gaff::JSON& json, T* object)
{
	for (auto it = _value_containers.begin(); it != _value_containers.end(); ++it) {
		(*it)->read(json, object);
	}
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::write(Gaff::JSON& json, T* object) const
{
	for (auto it = _value_containers.begin(); it != _value_containers.end(); ++it) {
		(*it)->write(json, object);
	}
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBaseClass(const ReflectionDefinition<T2, Allocator>& base_ref_def, unsigned int class_id)
{
	assert(this != &base_ref_def);

	if (!base_ref_def.isDefined()) {
		addOnCompleteCallback(Gaff::Bind<OnCompleteFunctor<T2>, void>(OnCompleteFunctor<T2>(*this, false)));
		++_base_classes_remaining;
		return Move(*this);
	}

	for (auto it = base_ref_def._value_containers.begin(); it != base_ref_def._value_containers.end(); ++it) {
		assert(!_value_containers.hasElementWithKey(it.getKey()));
		BaseValueContainer<T2>* container = _allocator.template allocT< BaseValueContainer<T2> >(it.getKey().getBuffer(), it.getValue());
		_value_containers.insert(it.getKey(), ValueContainerPtr(container));
	}

	for (auto it_base = base_ref_def._base_ids.begin(); it_base != base_ref_def._base_ids.end(); ++it_base) {
		auto it = _base_ids.binarySearch(_base_ids.begin(), _base_ids.end(), class_id,
			[](const Gaff::Pair< unsigned int, Gaff::FunctionBinder<void*, const void*> >& lhs, unsigned int rhs) -> bool
			{
				return lhs.first < rhs;
			});

		assert(it == _base_ids.end() || it->first != class_id);

		_base_ids.insert(*it_base, it);
	}

	return Move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBaseClass(unsigned int class_id)
{
	auto it = _base_ids.binarySearch(_base_ids.begin(), _base_ids.end(), class_id,
		[](const Gaff::Pair< unsigned int, Gaff::FunctionBinder<void*, const void*> >& lhs, unsigned int rhs) -> bool
		{
			return lhs.first < rhs;
		});

	assert(it == _base_ids.end() || it->first != class_id);

	Gaff::Pair<unsigned int, Gaff::FunctionBinder<void*, const void*> > data(class_id, Gaff::Bind(&BaseClassCast<T2>));
	_base_ids.insert(data, it);

	return Move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBaseClass(void)
{
	return addBaseClass<T2>(T2::g_Ref_Def, T2::g_Hash);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBaseClassInterfaceOnly(void)
{
	if (&T2::g_Ref_Def == this || T2::g_Ref_Def.isDefined()) {
		addBaseClass<T2>(T2::g_Hash);

	} else {
		addOnCompleteCallback(Gaff::Bind<OnCompleteFunctor<T2>, void>(OnCompleteFunctor<T2>(*this, true)));
		++_base_classes_remaining;
	}

	return Move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addObject(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ObjectContainer<T2>* container = _allocator.template allocT< ObjectContainer<T2> >(key, var, var_ref_def);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addEnum(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& ref_def)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	EnumContainer<T2>* container = _allocator.template allocT< EnumContainer<T2> >(key, var, ref_def);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addDouble(const char* key, double T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	DoubleContainer* container = _allocator.template allocT<DoubleContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addFloat(const char* key, float T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	FloatContainer* container = _allocator.template allocT<FloatContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUInt(const char* key, unsigned int T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UIntContainer* container = _allocator.template allocT<UIntContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addInt(const char* key, int T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	IntContainer* container = _allocator.template allocT<IntContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUShort(const char* key, unsigned short T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UShortContainer* container = _allocator.template allocT<UShortContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addShort(const char* key, short T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ShortContainer* container = _allocator.template allocT<ShortContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUChar(const char* key, unsigned char T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UCharContainer* container = _allocator.template allocT<UCharContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addChar(const char* key, char T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	CharContainer* container = _allocator.template allocT<CharContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBool(const char* key, bool T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	BoolContainer* container = _allocator.template allocT<BoolContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addString(const char* key, AString<Allocator> T::* var)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	StringContainer* container = _allocator.template allocT<StringContainer>(key, var);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addEnum(const char* key, const EnumReflectionDefinition<T2, Allocator>& ref_def, T2 (T::*getter)(void) const, void (T::*setter)(T2 value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	EnumContainer<T2>* container = _allocator.template allocT< EnumContainer<T2> >(key, ref_def, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addDouble(const char* key, double (T::*getter)(void) const, void (T::*setter)(double value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	DoubleContainer* container = _allocator.template allocT<DoubleContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addFloat(const char* key, float (T::*getter)(void) const, void (T::*setter)(float value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	FloatContainer* container = _allocator.template allocT<FloatContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUInt(const char* key, unsigned int (T::*getter)(void) const, void (T::*setter)(unsigned int value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UIntContainer* container = _allocator.template allocT<UIntContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addInt(const char* key, int (T::*getter)(void) const, void (T::*setter)(int value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	IntContainer* container = _allocator.template allocT<IntContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this)
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUShort(const char* key, unsigned short (T::*getter)(void) const, void (T::*setter)(unsigned short value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UShortContainer* container = _allocator.template allocT<UShortContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addShort(const char* key, short (T::*getter)(void) const, void (T::*setter)(short value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ShortContainer* container = _allocator.template allocT<ShortContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUChar(const char* key, unsigned char (T::*getter)(void) const, void (T::*setter)(unsigned char value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UCharContainer* container = _allocator.template allocT<UCharContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addChar(const char* key, char (T::*getter)(void) const, void (T::*setter)(char value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	CharContainer* container = _allocator.template allocT<CharContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBool(const char* key, bool (T::*getter)(void) const, void (T::*setter)(bool value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	BoolContainer* container = _allocator.template allocT<BoolContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addString(const char* key, const AString<Allocator>& (T::*getter)(void) const, void (T::*setter)(const AString<Allocator>& value))
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	StringContainer* container = _allocator.template allocT<StringContainer>(key, getter, setter);
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addCustom(const char* key, IValueContainer* container)
{
	assert(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	_value_containers.insert(key, ValueContainerPtr(container));
	return Move(*this);
}

template <class T, class Allocator>
bool ReflectionDefinition<T, Allocator>::isDefined(void) const
{
	return _defined;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::markDefined(void)
{
	_defined = true;

	for (auto it = _on_complete_callbacks.begin(); it != _on_complete_callbacks.end(); ++it) {
		(*it)();
	}

	_on_complete_callbacks.clear();
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_value_containers.setAllocator(allocator);
	_base_ids.setAllocator(allocator);
	_allocator = allocator;
	return Move(*this);
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::clear(void)
{
	_value_containers.clear();
}

// On Complete Callback
template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::addOnCompleteCallback(const Gaff::FunctionBinder<void>& cb)
{
	_on_complete_callbacks.push(cb);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::OnCompleteFunctor<T2>::OnCompleteFunctor(ReflectionDefinition<T, Allocator>& my_ref_def, bool interface_only):
	_my_ref_def(my_ref_def), _interface_only(interface_only)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::OnCompleteFunctor<T2>::operator()(void) const
{
	if (_interface_only) {
		_my_ref_def.addBaseClass<T2>(T2::g_Ref_Def, T2::g_Hash);
	} else {
		_my_ref_def.addBaseClass<T2>(T2::g_Hash);
	}

	--_my_ref_def._base_classes_remaining;

	if (!_my_ref_def._base_classes_remaining) {
		_my_ref_def.markDefined();
	}
}
