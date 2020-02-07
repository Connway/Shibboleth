/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

template <class T, class Allocator>
Vector<eastl::function<void(void)>, Allocator> ReflectionBase<T, Allocator>::g_on_defined_callbacks;

template <class T, class Allocator>
typename RefDefType<T, Allocator>::Type* ReflectionBase<T, Allocator>::g_ref_def = nullptr;

template <class T, class Allocator>
ReflectionVersion<T> ReflectionBase<T, Allocator>::g_version;

template <class T, class Allocator>
bool ReflectionBase<T, Allocator>::g_defined = false;

template <class T, class Allocator>
constexpr Hash64 ReflectionBase<T, Allocator>::GetHash(void)
{
	return GAFF_HASHABLE_NAMESPACE::GetHash<T>();
}

template <class T, class Allocator>
constexpr const char* ReflectionBase<T, Allocator>::GetName(void)
{
	return GAFF_HASHABLE_NAMESPACE::GetName<T>();
}

template <class T, class Allocator>
constexpr int32_t ReflectionBase<T, Allocator>::Size(void)
{
	return sizeof(T);
}

template <class T, class Allocator>
constexpr static bool ReflectionBase<T, Allocator>::IsEnum(void)
{
	return std::is_enum<T>::value;
}

template <class T, class Allocator>
Hash64 ReflectionBase<T, Allocator>::GetInstanceHash(const T& object, Hash64 init)
{
	return GetReflectionDefinition().getInstanceHash(&object, init);
}

template <class T, class Allocator>
Hash64 ReflectionBase<T, Allocator>::GetVersion(void)
{
	return g_version.getHash();
}

template <class T, class Allocator>
const typename RefDefType<T, Allocator>& ReflectionBase<T, Allocator>::GetReflectionDefinition(void)
{
	GAFF_ASSERT(g_ref_def);
	return *g_ref_def;
}

template <class T, class Allocator>
bool ReflectionBase<T, Allocator>::IsDefined(void)
{
	return g_defined;
}

template <class T, class Allocator>
void ReflectionBase<T, Allocator>::RegisterOnDefinedCallback(const eastl::function<void (void)>& callback)
{
	g_on_defined_callbacks.emplace_back(callback);
}

template <class T, class Allocator>
void ReflectionBase<T, Allocator>::RegisterOnDefinedCallback(eastl::function<void (void)>&& callback)
{
	g_on_defined_callbacks.emplace_back(std::move(callback));
}

template <class T, class Allocator>
bool ReflectionBase<T, Allocator>::Load(const ISerializeReader& reader, type& object, bool refl_load)
{
	//if constexpr (IsEnum()) {
	//	return getEnumReflectionDefinition().load(reader, object, refl_load);
	//} else {
	//	return getReflectionDefinition().load(reader, object, refl_load);
	//}
}

template <class T, class Allocator>
void ReflectionBase<T, Allocator>::Save(ISerializeWriter& writer, const type& object, bool refl_save)
{
	//if constexpr (IsEnum()) {
	//	return getEnumReflectionDefinition().save(writer, object, refl_save);
	//} else {
	//	return getReflectionDefinition().save(writer, object, refl_save);
	//}
}

template <class T, class Allocator>
void ReflectionBase<T, Allocator>::SetAllocator(const Allocator& a)
{
	GAFF_ASSERT(g_ref_def);
	g_on_defined_callbacks.set_allocator(a);
	g_ref_def->setAllocator(a);
}

template <class T, class Allocator>
ReflectionBase<T, Allocator>::ReflectionBase(void)
{
	if constexpr (std::is_enum<T>::value) {
		AddToEnumReflectionChain(this);

	} else {
		if constexpr (std::is_base_of<IAttribute, T>::value) {
			AddToAttributeReflectionChain(this);
		} else {
			AddToReflectionChain(this);
		}
	}
}

template <class T, class Allocator>
bool ReflectionBase<T, Allocator>::isEnum(void) const
{
	return IsEnum();
}

template <class T, class Allocator>
Hash64 ReflectionBase<T, Allocator>::getInstanceHash(const void* object, Hash64 init) const
{
	return GetInstanceHash(*reinterpret_cast<const T*>(object), init);
}

template <class T, class Allocator>
Hash64 ReflectionBase<T, Allocator>::getInstanceHash(const T& object, Hash64 init) const
{
	return GetInstanceHash(object, init);
}

template <class T, class Allocator>
const char* ReflectionBase<T, Allocator>::getName(void) const
{
	return GetName();
}

template <class T, class Allocator>
Hash64 ReflectionBase<T, Allocator>::getHash(void) const
{
	return GetHash();
}

template <class T, class Allocator>
Hash64 ReflectionBase<T, Allocator>::getVersion(void) const
{
	return GetVersion();
}

template <class T, class Allocator>
int32_t ReflectionBase<T, Allocator>::size(void) const
{
	return sizeof(T);
}

template <class T, class Allocator>
const IEnumReflectionDefinition& ReflectionBase<T, Allocator>::getEnumReflectionDefinition(void) const
{
	if constexpr (std::is_enum<T>::value) {
		return reinterpret_cast<const IEnumReflectionDefinition&>(GetReflectionDefinition()); /* To calm the compiler, even though this should be compiled out ... */
	} else {
		return IReflection::getEnumReflectionDefinition();
	}
}

template <class T, class Allocator>
const IReflectionDefinition& ReflectionBase<T, Allocator>::getReflectionDefinition(void) const
{
	if constexpr (std::is_enum<T>::value) {
		return IReflection::getReflectionDefinition();
	} else {
		return reinterpret_cast<const IReflectionDefinition&>(GetReflectionDefinition()); /* To calm the compiler, even though this should be compiled out ... */
	}
}

template <class T, class Allocator>
bool ReflectionBase<T, Allocator>::load(const ISerializeReader& reader, void* object, bool refl_load) const
{
	GAFF_ASSERT(object);
	return Load(reader, *reinterpret_cast<T*>(object), refl_load);
}

template <class T, class Allocator>
void ReflectionBase<T, Allocator>::save(ISerializeWriter& writer, const void* object, bool refl_save) const
{
	GAFF_ASSERT(object);
	return Save(writer, *reinterpret_cast<const T*>(object), refl_save);
}
