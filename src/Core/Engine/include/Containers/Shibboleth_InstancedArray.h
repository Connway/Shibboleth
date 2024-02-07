/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Reflection/Shibboleth_IReflectionObject.h"
#include "Reflection/Shibboleth_Reflection.h"

NS_SHIBBOLETH

template <class T>
class InstancedArray final
{
	static_assert(Refl::Reflection<T>::HasReflection, "Cannot serialize if type does not have reflection.");

public:
	explicit InstancedArray(const ProxyAllocator& allocator):
		_metadata(allocator), _instances(allocator)
	{
	}

	InstancedArray(InstancedArray<T>&& instanced_array) = default;
	InstancedArray(void) = default;

	InstancedArray& operator=(InstancedArray<T>&& instanced_array) = default;

	const T* operator[](size_t index) const
	{
		return at(index);
	}

	T* operator[](size_t index)
	{
		return at(index);
	}

	const T* operator[](int32_t index) const
	{
		return at(static_cast<size_t>(index));
	}

	T* operator[](int32_t index)
	{
		return at(static_cast<size_t>(index));
	}

	const T* at(size_t index) const
	{
		return const_cast<InstancedArray<T>*>(this)->at(index);
	}

	T* at(size_t index)
	{
		const Metadata& metadata = _metadata[index];
		return (metadata.ref_def) ? reinterpret_cast<T*>(_instances.data() + metadata.start) : nullptr;
	}

	const T* at(int32_t index) const
	{
		return at(static_cast<size_t>(index));
	}

	T* at(int32_t index)
	{
		return at(static_cast<size_t>(index));
	}

	const Refl::IReflectionDefinition* getReflectionDefinition(size_t index) const
	{
		GAFF_ASSERT(index < _metadata.size());
		return _metadata[index].ref_def;
	}

	const Refl::IReflectionDefinition* getReflectionDefinition(int32_t index) const
	{
		return getReflectionDefinition(static_cast<size_t>(index));
	}

	int32_t size(void) const
	{
		return static_cast<int32_t>(_metadata.size());
	}

	bool empty(void) const
	{
		return size() <= 0;
	}

	template <class U, class... Args>
	U& pushT(Args&&... args)
	{
		static_assert(Refl::Reflection<U>::HasReflection, "U does not have reflection.");
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated types.");
		static_assert(Gaff::OffsetOfClass<T, U>() == 0, "Offset of T in class U must be zero.");

		const Refl::ReflectionDefinition<U>& ref_def = Refl::Reflection<U>::GetReflectionDefinition();
		Metadata metadata = { &ref_def, 0, ref_def.size() };

		if (!_metadata.empty()) {
			const Metadata& last_metadata = _metadata.back();
			metadata.start = last_metadata.start + last_metadata.size;
		}

		_metadata.emplace_back(metadata);
		_instances.resize(_instances.size() + metadata.size);

		U* const instance = reinterpret_cast<U*>(_instances.data() + metadata.start);
		Gaff::Construct(instance, std::forward<Args>(args)...);

		return *instance;
	}

	template <class... Args>
	T& push(const Refl::IReflectionDefinition& ref_def, Args&&... args)
	{
		GAFF_ASSERT(ref_def.getBasePointerOffset<T>() == 0);
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated types.");
		static_assert(Gaff::OffsetOfClass<T, U>() == 0, "Offset of T in class U must be zero.");

		Metadata metadata = { &ref_def, 0, ref_def.size() };

		if (!_metadata.empty()) {
			const Metadata& last_metadata = _metadata.back();
			metadata.start = last_metadata.start + last_metadata.size;
		}

		_metadata.emplace_back(metadata);
		_instances.resize(_instances.size() + metadata.size);

		const auto constructor = ref_def.getConstructor<Args...>();
		GAFF_ASSERT(constructor);

		void* const instance = _instances.data() + metadata.start;
		constructor(instance, std::forward<Args>(args)...);

		return *reinterpret_cast<T*>(instance);
	}

	void pushEmpty(void)
	{
		Metadata metadata = { nullptr, 0, 0 };

		if (!_metadata.empty()) {
			const Metadata& last_metadata = _metadata.back();
			metadata.start = last_metadata.start + last_metadata.size;
		}

		_metadata.emplace_back(metadata);
	}

private:
	struct Metadata final
	{
		const Refl::IReflectionDefinition* ref_def = nullptr;
		int32_t start = -1;
		int32_t size = 0;
	};

	Vector<Metadata> _metadata;
	Vector<uint8_t> _instances; // Byte buffer.
};

NS_END

#include "Shibboleth_InstancedArrayReflection.h"
