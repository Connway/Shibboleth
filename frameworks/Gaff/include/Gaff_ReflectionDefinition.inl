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

NS_GAFF

//template <class T, class Allocator>
//ReflectionDefinition<T, Allocator>::ReflectionDefinition(const char* name, const Allocator& allocator)
//{
//}

//template <class T, class Allocator>
//ReflectionDefinition<T, Allocator>::ReflectionDefinition(const Allocator& allocator)
//{
//}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::load(ISerializeReader& reader, void* object) const
{
	load(reader, *reinterpret_cast<T*>(object));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::save(ISerializeWriter& writer, const void* object) const
{
	save(writer, *reinterpret_cast<const T*>(object));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::load(ISerializeReader& /*reader*/, T& /*object*/) const
{
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::save(ISerializeWriter& /*writer*/, const T& /*object*/) const
{
}

template <class T, class Allocator>
const void* ReflectionDefinition<T, Allocator>::getInterface(ReflectionHash class_hash, const void* object) const
{
	if (class_hash == T::GetReflectionHash()) {
		return object;
	}

	auto it = Gaff::Find(_base_class_offsets, class_hash);

	if (it == _base_class_offsets.end()) {
		return nullptr;
	}

	return reinterpret_cast<const int8_t*>(object) + it->second;
}

template <class T, class Allocator>
void* ReflectionDefinition<T, Allocator>::getInterface(ReflectionHash class_hash, void* object) const
{
	return const_cast<void*>(getInterface(class_hash, const_cast<const void*>(object)));
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::setAllocator(const Allocator& allocator)
{
	_base_class_offsets.set_allocator(allocator);
	_allocator = allocator;
}

template <class T, class Allocator>
void ReflectionDefinition<T, Allocator>::setReflectionInstance(const ISerializeInfo* reflection_instance)
{
	_reflection_instance = reflection_instance;
}

template <class T, class Allocator>
const ISerializeInfo& ReflectionDefinition<T, Allocator>::getReflectionInstance(void) const
{
	GAFF_ASSERT(_reflection_instance);
	return *_reflection_instance;
}

template <class T, class Allocator>
Hash64 ReflectionDefinition<T, Allocator>::getVersionHash(void) const
{
	return _version.getHash();
}

template <class T, class Allocator>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::baseClass(const char* name, ReflectionHash hash, ptrdiff_t offset)
{
	auto pair = std::move(eastl::make_pair(HashString32<Allocator>(name, hash, nullptr, _allocator), offset));
	_base_class_offsets.insert(std::move(pair));

	_version.baseClass(name, hash, offset);
	return *this;
}

template <class T, class Allocator>
template <class Base>
ReflectionDefinition<T, Allocator>& ReflectionDefinition<T, Allocator>::baseClass(void)
{
	return baseClass(Base::GetReflectionName(), Base::GetReflectionHash(), Gaff::OffsetOfClass<T, Base>());
}

NS_END
