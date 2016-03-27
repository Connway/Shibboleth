/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

// Base Callback Helper
template <class Allocator>
BaseCallbackHelper<Allocator>::BaseCallbackHelper(const Allocator& allocator):
	_callbacks(allocator), _allocator(allocator)
{
}

template <class Allocator>
BaseCallbackHelper<Allocator>::~BaseCallbackHelper(void)
{
}

template <class Allocator>
BaseCallbackHelper<Allocator>& BaseCallbackHelper<Allocator>::GetInstance(const Allocator& allocator)
{
	static BaseCallbackHelper helper_instance(allocator);
	return helper_instance;
}

template <class Allocator>
void BaseCallbackHelper<Allocator>::addBaseClassCallback(IReflectionDefinition* base_ref_def, Gaff::FunctionBinder<void>* callback)
{
	Array<FunctionBinder<void>*, Allocator>& callbacks = _callbacks[base_ref_def];

	if (callbacks.empty()) {
		callbacks.setAllocator(_allocator);
	}

	callbacks.push(callback);
}

template <class Allocator>
void BaseCallbackHelper<Allocator>::triggerBaseClassCallback(IReflectionDefinition* base_ref_def)
{
	Array<FunctionBinder<void>*, Allocator>* callbacks = &_callbacks[base_ref_def];

	for (unsigned int i = 0; i < callbacks->size(); ++i) {
		(*(*callbacks)[i])();

		// Because the array may have resized and invalidated our reference.
		// This is not optimal, but I'm lazy and it only happens during static initialization.
		callbacks = &_callbacks[base_ref_def];

		GAFF_FREET((*callbacks)[i], _allocator);
	}

	_callbacks.erase(base_ref_def);
}


// Enum Reflection Definition
template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>::EnumReflectionDefinition(EnumReflectionDefinition<T, Allocator>&& ref_def):
	_values_map(std::move(ref_def._values_map)), _name(std::move(ref_def._name)), _defined(true)
{
}

template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>::EnumReflectionDefinition(const char* name, const Allocator& allocator):
	_values_map(allocator), _name(name, allocator), _defined(false)
{
}

template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>::EnumReflectionDefinition(const Allocator& allocator):
	_values_map(allocator), _name(allocator), _defined(false)
{
}

template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>::~EnumReflectionDefinition(void)
{
}

template <class T, class Allocator>
const EnumReflectionDefinition<T, Allocator>& EnumReflectionDefinition<T, Allocator>::operator=(EnumReflectionDefinition<T, Allocator>&& rhs)
{
	_values_map = std::move(rhs._values_map);
	_name = std::move(rhs._name);
	_defined = true;
	return *this;
}

template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>&& EnumReflectionDefinition<T, Allocator>::addValue(const char* name, T value)
{
	GAFF_ASSERT(!_values_map.hasElementWithKey(name));
	_values_map.insert(name, value);
	return std::move(*this);
}

template <class T, class Allocator>
const char* EnumReflectionDefinition<T, Allocator>::getName(T value) const
{
	GAFF_ASSERT(_values_map.hasElementWithValue(value));
	return _values_map.findElementWithValue(value).getKey().getBuffer();
}

template <class T, class Allocator>
T EnumReflectionDefinition<T, Allocator>::getValue(const char* name) const
{
	GAFF_ASSERT(_values_map.hasElementWithKey(name));
	return _values_map[name];
}

template <class T, class Allocator>
bool EnumReflectionDefinition<T, Allocator>::getValue(const char* name, T& value) const
{
	auto it = _values_map.findElementWithKey(name);
	
	if (it == _values_map.end()) {
		return false;
	}

	value = it.getValue();
	return true;
}

template <class T, class Allocator>
Pair<AString<Allocator>, T> EnumReflectionDefinition<T, Allocator>::getEntry(size_t index) const
{
	GAFF_ASSERT(index < _values_map.size());
	auto it = _values_map.begin();

	for (size_t i = 0; i < index; ++i) {
		++it;
	}

	return MakePair(it.getKey().getString(), it.getValue());
}

template <class T, class Allocator>
const char* EnumReflectionDefinition<T, Allocator>::getNameGeneric(unsigned int value) const
{
	return getName(static_cast<T>(value));
}

template <class T, class Allocator>
unsigned int EnumReflectionDefinition<T, Allocator>::getValueGeneric(const char* name) const
{
	return static_cast<unsigned int>(getValue(name));
}

template <class T, class Allocator>
Pair<AString<Allocator>, unsigned int> EnumReflectionDefinition<T, Allocator>::getEntryGeneric(unsigned int index) const
{
	GAFF_ASSERT(index < _values_map.size());
	auto ret = getEntry(index);

	Pair<AString<Allocator>, unsigned int> out;
	out.first = std::move(ret.first);
	out.second = static_cast<unsigned int>(ret.second);

	return out;
}

template <class T, class Allocator>
size_t EnumReflectionDefinition<T, Allocator>::getNumEntries(void) const
{
	return _values_map.size();
}

template <class T, class Allocator>
const char* EnumReflectionDefinition<T, Allocator>::getEnumName(void) const
{
	return _name.getBuffer();
}

template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>&& EnumReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_values_map.setAllocator(allocator);
	return std::move(*this);
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

template <class T, class Allocator>
EnumReflectionDefinition<T, Allocator>&& EnumReflectionDefinition<T, Allocator>::macroFix(void)
{
	return std::move(*this);
}


// Reflection Definition
template <class T, class Allocator>
ReflectionDefinition<T, Allocator>::ReflectionDefinition(ReflectionDefinition<T, Allocator>&& ref_def):
	_value_containers(std::move(ref_def._value_containers)), _base_ids(std::move(ref_def._base_ids)),
	_callback_references(std::move(ref_def._callback_references)), _name(std::move(ref_def._name)),
	_allocator(ref_def._allocator), _base_classes_remaining(ref_def._base_classes_remaining)
{
	for (auto it = _callback_references.begin(); it != _callback_references.end(); ++it) {
		(*it)->setRefDef(this);
	}

	if (!_base_classes_remaining) {
		markDefined();
	}
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>::ReflectionDefinition(const char* name, const Allocator& allocator):
	_value_containers(allocator), _base_ids(allocator), _callback_references(allocator),
	_name(name, allocator), _allocator(allocator), _base_classes_remaining(0), _defined(false)
{
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>::ReflectionDefinition(const Allocator& allocator):
	_value_containers(allocator), _base_ids(allocator), _callback_references(allocator),
	_name(allocator), _base_classes_remaining(0), _defined(false)
{
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>::~ReflectionDefinition(void)
{
}

template <class T, class Allocator>
const ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::operator=(ReflectionDefinition<T, Allocator>&& rhs)
{
	_value_containers = std::move(rhs._value_containers);
	_base_ids = std::move(rhs._base_ids);
	_callback_references = std::move(rhs._callback_references);
	_allocator = rhs._allocator;
	_base_classes_remaining = rhs._base_classes_remaining;

	// Update the ReflectionDefinition references
	for (auto it = _callback_references.begin(); it != _callback_references.end(); ++it) {
		(*it)->setRefDef(this);
	}

	if (!_base_classes_remaining) {
		markDefined();
	}

	return *this;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::read(const JSON& json, void* object)
{
	read(json, reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::write(JSON& json, const void* object) const
{
	write(json, reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
void* ReflectionDefinition<T, Allocator>::getInterface(ReflectionHash class_id, const void* object) const
{
	auto it = _base_ids.binarySearch(_base_ids.begin(), _base_ids.end(), class_id,
		[](const Pair< ReflectionHash, FunctionBinder<void*, const void*> >& lhs, ReflectionHash rhs) -> bool
		{
			return lhs.first < rhs;
		});

	return (it != _base_ids.end() && it->first == class_id) ? it->second(object) : nullptr;
}

template <class T, class Allocator>
const char* ReflectionDefinition<T, Allocator>::getName(void) const
{
	return _name.getBuffer();
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::read(const JSON& json, T* object)
{
	for (auto it = _value_containers.begin(); it != _value_containers.end(); ++it) {
		(*it)->read(json, object);
	}
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::write(JSON& json, const T* object) const
{
	for (auto it = _value_containers.begin(); it != _value_containers.end(); ++it) {
		(*it)->write(json, object);
	}
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBaseClass(ReflectionDefinition<T2, Allocator>& base_ref_def, ReflectionHash class_id)
{
	GAFF_ASSERT(reinterpret_cast<void*>(this) != reinterpret_cast<void*>(&base_ref_def));

	if (!base_ref_def._defined) {
		Functor<OnCompleteFunctor<T2>, void> ftor(OnCompleteFunctor<T2>(this, true));
		FunctionBinder<void>* binder = GAFF_ALLOCT(FunctionBinder<void>, _allocator, &ftor, sizeof(ftor));

		BaseCallbackHelper<Allocator>::GetInstance(_allocator).addBaseClassCallback(&T2::GetReflectionDefinition(), binder);

		OnCompleteFunctor<T2>* functor = &((Functor<OnCompleteFunctor<T2>, void>&)binder->getInterface()).getFunctor();
		_callback_references.push(functor);

		++_base_classes_remaining;
		return std::move(*this);
	}

	for (auto it = base_ref_def._value_containers.begin(); it != base_ref_def._value_containers.end(); ++it) {
		GAFF_ASSERT(!_value_containers.hasElementWithKey(it.getKey()));
		BaseValueContainer<T2>* container = GAFF_ALLOCT(BaseValueContainer<T2>, _allocator, it.getKey().getBuffer(), it.getValue(), _allocator);
		_value_containers.insert(it.getKey(), ValueContainerPtr(container));
	}

	for (auto it_base = base_ref_def._base_ids.begin(); it_base != base_ref_def._base_ids.end(); ++it_base) {
		auto it = _base_ids.binarySearch(_base_ids.begin(), _base_ids.end(), class_id,
			[](const Pair< ReflectionHash, FunctionBinder<void*, const void*> >& lhs, ReflectionHash rhs) -> bool
			{
				return lhs.first < rhs;
			});

		GAFF_ASSERT(it == _base_ids.end() || it->first != class_id);

		_base_ids.insert(it, *it_base);
	}

	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBaseClass(ReflectionHash class_id)
{
	auto it = _base_ids.binarySearch(_base_ids.begin(), _base_ids.end(), class_id,
		[](const Pair< ReflectionHash, FunctionBinder<void*, const void*> >& lhs, ReflectionHash rhs) -> bool
		{
			return lhs.first < rhs;
		});

	GAFF_ASSERT(it == _base_ids.end() || it->first != class_id);

	Pair<ReflectionHash, FunctionBinder<void*, const void*> > data(class_id, Bind(&BaseClassCast<T2>));
	_base_ids.insert(it, data);

	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBaseClass(void)
{
	return addBaseClass<T2>(T2::GetReflectionDefinition(), T2::GetReflectionHash());
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBaseClassInterfaceOnly(void)
{
	if ((reinterpret_cast<void*>(&T::GetReflectionDefinition()) == reinterpret_cast<void*>(&T2::GetReflectionDefinition())) ||
		(reinterpret_cast<void*>(&T2::GetReflectionDefinition()) == reinterpret_cast<void*>(this)) || 
		T2::GetReflectionDefinition()._defined) {

		addBaseClass<T2>(T2::GetReflectionHash());

	} else {
		Functor<OnCompleteFunctor<T2>, void> ftor(OnCompleteFunctor<T2>(this, true));
		FunctionBinder<void>* binder = GAFF_ALLOCT(FunctionBinder<void>, _allocator, &ftor, sizeof(ftor));

		BaseCallbackHelper<Allocator>::GetInstance(_allocator).addBaseClassCallback(&T2::GetReflectionDefinition(), binder);

		OnCompleteFunctor<T2>* functor = &((Functor<OnCompleteFunctor<T2>, void>&)binder->getInterface()).getFunctor();
		_callback_references.push(functor);

		++_base_classes_remaining;
	}

	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addObject(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ObjectContainer<T2>* container = GAFF_ALLOCT(ObjectContainer<T2>, _allocator, key, var, var_ref_def, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addEnum(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	EnumContainer<T2>* container = GAFF_ALLOCT(EnumContainer<T2>, _allocator, key, var, ref_def, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}


template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<T2, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<T2, Allocator>&), ReflectionDefinition<T2, Allocator>& obj_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayObjectContainer<T2>* container = GAFF_ALLOCT(ArrayObjectContainer<T2>, _allocator, key, obj_ref_def, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<T2, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<T2, Allocator>&))
{
	addArray(key, getter, setter, T2::GetReflectionDefinition());
	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<T2, Allocator> T::* var, ReflectionDefinition<T2, Allocator>& obj_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayObjectContainer<T2>* container = GAFF_ALLOCT(ArrayObjectContainer<T2>, _allocator, key, var, obj_ref_def, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<T2, Allocator> T::* var)
{
	addArray(key, var, T2::GetReflectionDefinition());
	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<T2, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<T2, Allocator>&), const EnumReflectionDefinition<T2, Allocator>& enum_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayEnumContainer<T2>* container = GAFF_ALLOCT(ArrayEnumContainer<T2>, _allocator, key, enum_ref_def, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<T2, Allocator> T::* var, const EnumReflectionDefinition<T2, Allocator>& enum_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayEnumContainer<T2>* container = GAFF_ALLOCT(ArrayEnumContainer<T2>, _allocator, key, var, enum_ref_def, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<double, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<double, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayDoubleContainer* container = GAFF_ALLOCT(ArrayDoubleContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<float, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<float, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFloatContainer* container = GAFF_ALLOCT(ArrayFloatContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<unsigned int, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<unsigned int, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayUIntContainer* container = GAFF_ALLOCT(ArrayUIntContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<int, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<int, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayIntContainer* container = GAFF_ALLOCT(ArrayIntContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<unsigned short, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<unsigned short, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayUShortContainer* container = GAFF_ALLOCT(ArrayUShortContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<short, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<short, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayShortContainer* container = GAFF_ALLOCT(ArrayShortContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<unsigned char, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<unsigned char, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayUCharContainer* container = GAFF_ALLOCT(ArrayUCharContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<char, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<char, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayCharContainer* container = GAFF_ALLOCT(ArrayCharContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const Array<AString<Allocator>, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<AString<Allocator>, Allocator>&))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayStringContainer* container = GAFF_ALLOCT(ArrayCharContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<double, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayDoubleContainer* container = GAFF_ALLOCT(ArrayDoubleContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<float, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFloatContainer* container = GAFF_ALLOCT(ArrayFloatContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<unsigned int, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayUIntContainer* container = GAFF_ALLOCT(ArrayUIntContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<int, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayIntContainer* container = GAFF_ALLOCT(ArrayIntContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<unsigned short, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayUShortContainer* container = GAFF_ALLOCT(ArrayUShortContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<short, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayShortContainer* container = GAFF_ALLOCT(ArrayShortContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<unsigned char, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayUCharContainer* container = GAFF_ALLOCT(ArrayUCharContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<char, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayCharContainer* container = GAFF_ALLOCT(ArrayCharContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, Array<AString<Allocator>, Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayStringContainer* container = GAFF_ALLOCT(ArrayStringContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size, class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const T2* (T::*getter)(void) const, void (T::*setter)(const T2*), const EnumReflectionDefinition<T2, Allocator>& enum_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedEnumContainer<array_size, T2>* container = GAFF_ALLOCT(ArrayFixedEnumContainer<array_size, T2>, _allocator, key, enum_ref_def, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size, class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const T2* (T::*getter)(void) const, void (T::*setter)(const T2*), ReflectionDefinition<T2, Allocator>& obj_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedObjectContainer<array_size, T2>* container = GAFF_ALLOCT(ArrayFixedObjectContainer<array_size, T2>, _allocator, key, obj_ref_def, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size, class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const T2* (T::*getter)(void) const, void (T::*setter)(const T2*))
{
	addArray<array_size>(key, getter, setter, T2::GetReflectionDefinition());
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const double* (T::*getter)(void) const, void (T::*setter)(const double*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedDoubleContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedDoubleContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const float* (T::*getter)(void) const, void (T::*setter)(const float*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedFloatContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedFloatContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const unsigned int* (T::*getter)(void) const, void (T::*setter)(const unsigned int*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedUIntContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedUIntContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const int* (T::*getter)(void) const, void (T::*setter)(const int*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedIntContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedIntContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const unsigned short* (T::*getter)(void) const, void (T::*setter)(const unsigned short*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedUShortContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedUShortContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const short* (T::*getter)(void) const, void (T::*setter)(const short*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedShortContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedShortContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const unsigned char* (T::*getter)(void) const, void (T::*setter)(const unsigned char*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedUCharContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedUCharContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const char* (T::*getter)(void) const, void (T::*setter)(const char*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedCharContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedCharContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, const AString<Allocator>* (T::*getter)(void) const, void (T::*setter)(const AString<Allocator>*))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedStringContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedStringContainer<array_size>, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size, class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, T2 (T::*var)[array_size], const EnumReflectionDefinition<T2, Allocator>& enum_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedEnumContainer<array_size, T2>* container = GAFF_ALLOCT(ArrayFixedEnumContainer<array_size, T2>, _allocator, key, var, enum_ref_def, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size, class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, T2 (T::*var)[array_size], ReflectionDefinition<T2, Allocator>& obj_ref_def)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedObjectContainer<array_size, T2>* container = GAFF_ALLOCT(ArrayFixedObjectContainer<array_size, T2>, _allocator, key, var, obj_ref_def, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size, class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, T2 (T::*var)[array_size])
{
	addArray(key, var, T2::GetReflectionDefinition());
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, double (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedDoubleContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedDoubleContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, float (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedFloatContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedFloatContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, unsigned int (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedUIntContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedUIntContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, int (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedIntContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedIntContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, unsigned short (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedUShortContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedUShortContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, short (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedShortContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedShortContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, unsigned char (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedUCharContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedUCharContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, char (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedCharContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedCharContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <unsigned int array_size>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addArray(const char* key, AString<Allocator> (T::*var)[array_size])
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ArrayFixedStringContainer<array_size>* container = GAFF_ALLOCT(ArrayFixedStringContainer<array_size>, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addDouble(const char* key, double T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	DoubleContainer* container = GAFF_ALLOCT(DoubleContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addFloat(const char* key, float T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	FloatContainer* container = GAFF_ALLOCT(FloatContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUInt(const char* key, unsigned int T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UIntContainer* container = GAFF_ALLOCT(UIntContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addInt(const char* key, int T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	IntContainer* container = GAFF_ALLOCT(IntContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUShort(const char* key, unsigned short T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UShortContainer* container = GAFF_ALLOCT(UShortContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addShort(const char* key, short T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ShortContainer* container = GAFF_ALLOCT(ShortContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUChar(const char* key, unsigned char T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UCharContainer* container = GAFF_ALLOCT(UCharContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addChar(const char* key, char T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	CharContainer* container = GAFF_ALLOCT(CharContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBool(const char* key, bool T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	BoolContainer* container = GAFF_ALLOCT(BoolContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addString(const char* key, AString<Allocator> T::* var)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	StringContainer* container = GAFF_ALLOCT(StringContainer, _allocator, key, var, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addEnum(const char* key, const EnumReflectionDefinition<T2, Allocator>& ref_def, T2 (T::*getter)(void) const, void (T::*setter)(T2 value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	EnumContainer<T2>* container = GAFF_ALLOCT(EnumContainer<T2>, _allocator, key, ref_def, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addDouble(const char* key, double (T::*getter)(void) const, void (T::*setter)(double value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	DoubleContainer* container = GAFF_ALLOCT(DoubleContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addFloat(const char* key, float (T::*getter)(void) const, void (T::*setter)(float value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	FloatContainer* container = GAFF_ALLOCT(FloatContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUInt(const char* key, unsigned int (T::*getter)(void) const, void (T::*setter)(unsigned int value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UIntContainer* container = GAFF_ALLOCT(UIntContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addInt(const char* key, int (T::*getter)(void) const, void (T::*setter)(int value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	IntContainer* container = GAFF_ALLOCT(IntContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this)
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUShort(const char* key, unsigned short (T::*getter)(void) const, void (T::*setter)(unsigned short value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UShortContainer* container = GAFF_ALLOCT(UShortContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addShort(const char* key, short (T::*getter)(void) const, void (T::*setter)(short value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	ShortContainer* container = GAFF_ALLOCT(ShortContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addUChar(const char* key, unsigned char (T::*getter)(void) const, void (T::*setter)(unsigned char value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	UCharContainer* container = GAFF_ALLOCT(UCharContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addChar(const char* key, char (T::*getter)(void) const, void (T::*setter)(char value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	CharContainer* container = GAFF_ALLOCT(CharContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addBool(const char* key, bool (T::*getter)(void) const, void (T::*setter)(bool value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	BoolContainer* container = GAFF_ALLOCT(BoolContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addString(const char* key, const AString<Allocator>& (T::*getter)(void) const, void (T::*setter)(const AString<Allocator>& value))
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	StringContainer* container = GAFF_ALLOCT(StringContainer, _allocator, key, getter, setter, _allocator);
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::addCustom(const char* key, ValueContainerBase* container)
{
	GAFF_ASSERT(key && strlen(key) && !_value_containers.hasElementWithKey(key));
	_value_containers.insert(key, ValueContainerPtr(container));
	return std::move(*this);
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
	BaseCallbackHelper<Allocator>::GetInstance(_allocator).triggerBaseClassCallback(this);
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_value_containers.setAllocator(allocator);
	_base_ids.setAllocator(allocator);
	_allocator = allocator;
	return std::move(*this);
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::clear(void)
{
	_value_containers.clear();
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>&& ReflectionDefinition<T, Allocator>::macroFix(void)
{
	return std::move(*this);
}


// On Complete Functor
template <class T, class Allocator>
template <class T2>
ReflectionDefinition<T, Allocator>::OnCompleteFunctor<T2>::OnCompleteFunctor(ReflectionDefinition<T, Allocator>* my_ref_def, bool interface_only):
	_my_ref_def(my_ref_def), _interface_only(interface_only)
{
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::OnCompleteFunctor<T2>::operator()(void) const
{
	if (_interface_only) {
		_my_ref_def->addBaseClass<T2>(T2::GetReflectionHash());
	} else {
		_my_ref_def->addBaseClass<T2>(T2::GetReflectionDefinition(), T2::GetReflectionHash());
	}

	--_my_ref_def->_base_classes_remaining;

	if (!_my_ref_def->_base_classes_remaining) {
		_my_ref_def->markDefined();
	}
}

template <class T, class Allocator>
template <class T2>
void ReflectionDefinition<T, Allocator>::OnCompleteFunctor<T2>::setRefDef(IReflectionDefinition* ref_def)
{
	_my_ref_def = reinterpret_cast<ReflectionDefinition<T, Allocator>*>(ref_def);
}
