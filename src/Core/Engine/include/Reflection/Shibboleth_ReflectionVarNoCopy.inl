/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

NS_REFLECTION

// VarNoCopy
template <class T, class VarType, class ReflectionType>
VarNoCopy<T VarType, ReflectionType>::VarNoCopy(VarType T::*ptr):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class VarType, class ReflectionType>
const IReflection& VarNoCopy<T VarType, ReflectionType>::getReflection(void) const
{
	return Reflection<ReflectionType>::GetInstance();
}

template <class T, class VarType, class ReflectionType>
const void* VarNoCopy<T VarType, ReflectionType>::getData(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, class ReflectionType>
void* VarNoCopy<T VarType, ReflectionType>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, class ReflectionType>
void VarNoCopy<T VarType, ReflectionType>::setData(void* /*object*/, const void* /*data*/)
{
	GAFF_ASSERT_MSG(
		false,
		"VarNoCopy<T VarType, ReflectionType>::setData() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
	);
}

template <class T, class VarType, class ReflectionType>
void VarNoCopy<T VarType, ReflectionType>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = std::move(*reinterpret_cast<VarType*>(data));
}

template <class T, class VarType, class ReflectionType>
bool VarNoCopy<T VarType, ReflectionType>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	ReflectionType* const var = &(object.*_ptr);
	return Reflection<ReflectionType>::GetInstance().load(reader, *var);
}

template <class T, class VarType, class ReflectionType>
void VarNoCopy<T VarType, ReflectionType>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const ReflectionType* const var = &(object.*_ptr);
	Reflection<ReflectionType>::GetInstance().save(writer, *var);
}

template <class T, class VarType, class ReflectionType>
void VarNoCopy<T, VarType, ReflectionType>::setOffset(ptrdiff_t offset)
{
	_offset = offset;
}



// VectorVarNoCopy
template <class T, class VarType, class ReflectionType, class Vec_Allocator>
VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::VectorVarNoCopy(VectorType T::*ptr):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
const IReflection& VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::getReflection(void) const
{
	return Reflection<ReflectionType>::GetInstance();
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
const void* VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::getData(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void* VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::setData(void* /*object*/, const void* /*data*/)
{
	GAFF_ASSERT_MSG(
		false,
		"VectorVarNoCopy<T VarType, ReflectionType, Vec_Allocator>::setData() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
	);
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr) = std::move(*reinterpret_cast<VectorType*>(data));
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
int32_t VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::size(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return static_cast<int32_t>((obj->*_ptr).size());
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
const void* VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void* VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT_MSG(
		false,
		"VectorVarNoCopy<T VarType, ReflectionType, Vec_Allocator>::setElement() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
	);
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = std::move(*reinterpret_cast<const VarType*>(data));
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a >= 0 && index_a < size(object));
	GAFF_ASSERT(index_b >= 0 && index_b < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr)[index_a], (obj->*_ptr)[index_b]);
	eastl::swap(_elements[index_a], _elements[index_b]);
	_cached_element_vars[index_a].second = &_elements[index_a];
	_cached_element_vars[index_b].second = &_elements[index_b];
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::resize(void* object, size_t new_size)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).resize(new_size);

	const size_t old_size = _elements.size();

	_cached_element_vars.resize(new_size);
	_elements.resize(new_size);

	if (new_size > old_size) {
		regenerateSubVars(old_size + 1, new_size);
	}
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::remove(void* object, int32_t index)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr).erase((obj->*_ptr).begin() + index);

	_cached_element_vars.erase(_cached_element_vars.begin() + index);
	_elements.erase(_elements.begin() + index);

	regenerateSubVars(index, static_cast<int32_t>(_elements.size()));
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
bool VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	const int32_t size = reader.size();
	(object.*_ptr).resize(static_cast<size_t>(size));

	bool success = true;

	for (int32_t i = 0; i < size; ++i) {
		Shibboleth::ScopeGuard scope = reader.enterElementGuard(i);

		if (!Reflection<ReflectionType>::GetInstance().load(reader, (object.*_ptr)[i])) {
			// $TODO: Log error.
			success = false;
		}
	}

	_cached_element_vars.resize(static_cast<size_t>(size));
	_elements.resize(static_cast<size_t>(size));
	regenerateSubVars(0, size);

	return success;
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const int32_t size = static_cast<int32_t>((object.*_ptr).size());
	writer.startArray(static_cast<uint32_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		Reflection<ReflectionType>::GetInstance().save(writer, (object.*_ptr)[i]);
	}

	writer.endArray();
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
const Shibboleth::Vector<IReflectionVar::SubVarData>& VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::getSubVars(void)
{
	return _cached_element_vars;
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::setSubVarBaseName(eastl::u8string_view base_name)
{
	_base_name = base_name;
	regenerateSubVars(0, static_cast<int32_t>(_elements.size()));
}

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
void VectorVarNoCopy<T, VarType, ReflectionType, Vec_Allocator>::regenerateSubVars(int32_t range_begin, int32_t range_end)
{
	for (int32_t i = range_begin; i < range_end; ++i) {
		Shibboleth::U8String element_path(Shibboleth::ProxyAllocator("Reflection"));
		element_path.append_sprintf(u8"%s[%i]", _base_name.data(), i);

		_elements[i].setOffset(static_cast<ptrdiff_t>(i * sizeof(VarType)));
		_elements[i].setNoSerialize(true);
		_cached_element_vars[i].first = Shibboleth::HashString32<>(element_path);
		_cached_element_vars[i].second = &_elements[i];
	}
}



// ArrayVarNoCopy
template <class T, class VarType, class ReflectionType, size_t array_size>
ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::ArrayVarNoCopy(VarType (T::*ptr)[array_size]):
	_ptr(ptr)
{
	GAFF_ASSERT(ptr);

	_cached_element_vars.resize(array_size);

	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		_elements[i].setOffset(static_cast<ptrdiff_t>(i * sizeof(VarType)));
		_elements[i].setNoSerialize(true);
		_cached_element_vars[i].second = &_elements[i];
	}
}

template <class T, class VarType, class ReflectionType, size_t array_size>
const IReflection& ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::getReflection(void) const
{
	return Reflection<ReflectionType>::GetInstance();
}

template <class T, class VarType, class ReflectionType, size_t array_size>
const void* ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::getData(const void* object) const
{
	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void* ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::getData(void* object)
{
	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr);
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::setData(void* /*object*/, const void* /*data*/)
{
	GAFF_ASSERT_MSG(
		false,
		"ArrayVarNoCopy<T VarType, ReflectionType, array_size>::setData() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
	);
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::setDataMove(void* object, void* data)
{
	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	const VarType* const vars = reinterpret_cast<const VarType*>(data);
	T* const obj = reinterpret_cast<T*>(object);

	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		(obj->*_ptr)[i] = std::move(vars[i]);
	}
}

template <class T, class VarType, class ReflectionType, size_t array_size>
const void* ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::getElement(const void* object, int32_t index) const
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	const T* const obj = reinterpret_cast<const T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void* ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::getElement(void* object, int32_t index)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	T* const obj = reinterpret_cast<T*>(object);
	return &(obj->*_ptr)[index];
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::setElement(void* object, int32_t index, const void* data)
{
	GAFF_ASSERT_MSG(
		false,
		"ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::setElement() was called with ReflectionType of '%s'.",
		reinterpret_cast<const char*>(Reflection<ReflectionType>::GetName())
	);
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::setElementMove(void* object, int32_t index, void* data)
{
	GAFF_ASSERT(index >= 0 && index < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	(obj->*_ptr)[index] = std::move(*reinterpret_cast<const VarType*>(data));
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::swap(void* object, int32_t index_a, int32_t index_b)
{
	GAFF_ASSERT(index_a >= 0 && index_a < size(object));
	GAFF_ASSERT(index_b >= 0 && index_b < size(object));

	if (IReflectionVar::isReadOnly()) {
		// $TODO: Log error.
		return;
	}

	T* const obj = reinterpret_cast<T*>(object);
	eastl::swap((obj->*_ptr)[index_a], (obj->*_ptr)[index_b]);
	eastl::swap(_elements[index_a], _elements[index_b]);
	_cached_element_vars[index_a].second = &_elements[index_a];
	_cached_element_vars[index_b].second = &_elements[index_b];
}

template <class T, class VarType, class ReflectionType, size_t array_size>
bool ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::load(const Shibboleth::ISerializeReader& reader, T& object)
{
	GAFF_ASSERT(reader.size() == static_cast<int32_t>(array_size));

	bool success = true;

	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		Shibboleth::ScopeGuard scope = reader.enterElementGuard(i);

		if (!Reflection<ReflectionType>::GetInstance().load(reader, (object.*_ptr)[i])) {
			// $TODO: Log error.
			success = false;
		}
	}

	return success;
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::save(Shibboleth::ISerializeWriter& writer, const T& object)
{
	const int32_t size = static_cast<int32_t>((object.*_ptr).size());
	writer.startArray(static_cast<uint32_t>(size));

	for (int32_t i = 0; i < size; ++i) {
		Reflection<ReflectionType>::GetInstance().save(writer, (object.*_ptr)[i]);
	}

	writer.endArray();
}

template <class T, class VarType, class ReflectionType, size_t array_size>
const Shibboleth::Vector<IReflectionVar::SubVarData>& ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::getSubVars(void)
{
	return _cached_element_vars;
}

template <class T, class VarType, class ReflectionType, size_t array_size>
void ArrayVarNoCopy<T, VarType, ReflectionType, array_size>::setSubVarBaseName(eastl::u8string_view base_name)
{
	for (int32_t i = 0; i < static_cast<int32_t>(array_size); ++i) {
		Shibboleth::U8String element_path(Shibboleth::ProxyAllocator("Reflection"));
		element_path.append_sprintf(u8"%s[%i]", base_name.data(), i);

		_cached_element_vars[i].first = Shibboleth::HashString32<>(element_path);
	}
}

NS_END
