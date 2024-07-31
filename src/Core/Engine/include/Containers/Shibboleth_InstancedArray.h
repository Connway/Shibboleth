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
	class Iterator final
	{
	public:
		explicit Iterator(InstancedArray<T>& owner, int32_t index = 0):
			_owner(&owner), _index(index)
		{
		}

		Iterator(const Iterator& it) = default;
		Iterator(void) = default;

		Iterator& operator=(const Iterator& rhs) = default;

		Iterator operator+(int32_t offset) const
		{
			return (_owner) ? Iterator(*_owner, _index + offset) : Iterator();
		}

		Iterator& operator+=(int32_t offset)
		{
			_index += offset;
			return *this;
		}

		Iterator& operator++(void)
		{
			++_index;
			return *this;
		}

		Iterator operator++(int)
		{
			return (_owner) ? Iterator(_owner, _index++) : Iterator();
		}

		Iterator operator-(int32_t offset) const
		{
			return (_owner) ? Iterator(*_owner, _index - offset) : Iterator();
		}

		Iterator& operator-=(int32_t offset)
		{
			_index -= offset;
			return *this;
		}

		Iterator& operator--(void)
		{
			--_index;
			return *this;
		}

		Iterator operator--(int)
		{
			return (_owner) ? Iterator(_owner, _index--) : Iterator();
		}

		T& operator*(void) const
		{
			T* const entry = get();
			GAFF_ASSERT(entry);

			return *entry;
		}

		T* operator->(void) const
		{
			return get();
		}

		int32_t getIndex(void) const
		{
			return _index;
		}

		T* get(void) const
		{
			return (_owner) ? _owner->at(_index) : nullptr;
		}

		auto operator<=>(const Iterator& rhs) const
		{
			return _index <=> rhs._index;
		}

		bool operator==(const Iterator& rhs) const
		{
			return _owner == rhs._owner && _index == rhs._index;
		}

	private:
		InstancedArray<T>* _owner = nullptr;
		int32_t _index = -1;
	};

	class ConstIterator final
	{
	public:
		explicit ConstIterator(InstancedArray<T>& owner, int32_t index = 0):
		_iterator(owner, index)
		{
		}

		ConstIterator(const Iterator& it):
		_iterator(it)
		{
		}

		ConstIterator(const ConstIterator& it) = default;
		ConstIterator(void) = default;

		ConstIterator& operator=(const ConstIterator& rhs) = default;
		ConstIterator& operator=(const Iterator& rhs)
		{
			_iterator = rhs;
			return *this;
		}

		ConstIterator operator+(int32_t offset) const
		{
			return _iterator + offset;
		}

		ConstIterator& operator+=(int32_t offset)
		{
			_iterator += offset;
			return *this;
		}

		ConstIterator& operator++(void)
		{
			++_iterator;
			return *this;
		}

		ConstIterator operator++(int)
		{
			return ConstIterator(_iterator++);
		}

		ConstIterator operator-(int32_t offset) const
		{
			return _iterator - offset;
		}

		ConstIterator& operator-=(int32_t offset)
		{
			_iterator -= offset;
			return *this;
		}

		ConstIterator& operator--(void)
		{
			--_iterator;
			return *this;
		}

		ConstIterator operator--(int)
		{
			return ConstIterator(_iterator--);
		}

		const T& operator*(void) const
		{
			const T* const entry = get();
			GAFF_ASSERT(entry);

			return *entry;
		}

		const T* operator->(void) const
		{
			return get();
		}

		int32_t getIndex(void) const
		{
			return _iterator.getIndex();
		}

		const T* get(void) const
		{
			return _iterator.get();
		}

		auto operator<=>(const ConstIterator& rhs) const
		{
			return *this <=> rhs._iterator;
		}

		auto operator<=>(const Iterator& rhs) const
		{
			return _iterator <=> rhs;
		}

		bool operator==(const ConstIterator& rhs) const
		{
			return *this == rhs._iterator;
		}

		bool operator==(const Iterator& rhs) const
		{
			return _iterator == rhs;
		}

	private:
		Iterator _iterator;
	};



	explicit InstancedArray(const ProxyAllocator& allocator):
		_metadata(allocator), _instances(allocator)
	{
	}

	InstancedArray(InstancedArray<T>&& instanced_array) = default;
	InstancedArray(void) = default;

	InstancedArray& operator=(InstancedArray<T>&& instanced_array) = default;

	const T* operator[](int32_t index) const
	{
		return at(index);
	}

	T* operator[](int32_t index)
	{
		return at(index);
	}

	const T* at(int32_t index) const
	{
		return const_cast<InstancedArray<T>*>(this)->at(index);
	}

	T* at(int32_t index)
	{
		GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_metadata.size()));
		const Metadata& metadata = _metadata[index];
		return (metadata.ref_def) ? reinterpret_cast<T*>(_instances.data() + metadata.start) : nullptr;
	}

	const Refl::IReflectionDefinition* getReflectionDefinition(int32_t index) const
	{
		GAFF_ASSERT(index >= 0 && index < static_cast<int32_t>(_metadata.size()));
		return _metadata[index].ref_def;
	}

	int32_t size(void) const
	{
		return static_cast<int32_t>(_metadata.size());
	}

	bool empty(void) const
	{
		return size() <= 0;
	}

	ConstIterator begin(void) const
	{
		return const_cast<InstancedArray<T>*>(this)->begin();
	}

	ConstIterator end(void) const
	{
		return const_cast<InstancedArray<T>*>(this)->end();
	}

	Iterator begin(void)
	{
		return Iterator(*this);
	}

	Iterator end(void)
	{
		return Iterator(*this, size());
	}

	const T& back(void) const
	{
		return const_cast<InstancedArray<T>*>(this)->back();
	}

	T& back(void)
	{
		return *at(size() - 1);
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
		GAFF_ASSERT_MSG(ref_def.getBasePointerOffset<T>() == 0, "Offset of T in class U must be zero.");

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

	void pop(void)
	{
		GAFF_ASSERT(!empty());
		erase(size() - 1);
	}

	void erase(int32_t index)
	{
		eraseInternal(index, true);
	}

	void resize(int32_t new_size)
	{
		new_size = Gaff::Max(0, new_size);

		const int32_t old_size = size();

		if (new_size > old_size) {
			for (int32_t i = old_size; i < new_size; ++i) {
				pushEmpty();
			}

		} else if (new_size < old_size) {
			for (int32_t i = old_size; i > new_size; --i) {
				pop();
			}
		}
	}

	void swap(int32_t index_a, int32_t index_b)
	{
		GAFF_ASSERT(index_a >= 0 && index_a < size());
		GAFF_ASSERT(index_b >= 0 && index_b < size());

		if (index_a == index_b) {
			return;
		}

		// Ensure index_a < index_b.
		if (index_a > index_b) {
			eastl::swap(index_a, index_b);
		}

		// Copying as we're going to erase the entries later.
		const Metadata metadata_a = _metadata[index_a];
		const Metadata metadata_b = _metadata[index_b];

		Vector<uint8_t> value_a(metadata_a.size, 0, _metadata.get_allocator());
		Vector<uint8_t> value_b(metadata_b.size, 0, _metadata.get_allocator());

		// Copy bytes into temp value buffers.
		for (int32_t i = 0; i < metadata_a.size; ++i) {
			value_a[i] = _instances[metadata_a.start + i];
		}

		for (int32_t i = 0; i < metadata_b.size; ++i) {
			value_b[i] = _instances[metadata_b.start + i];
		}

		eraseInternal(index_a, false);
		insertInternal(index_a, metadata_b, value_b);

		eraseInternal(index_b, false);
		insertInternal(index_b, metadata_a, value_a);
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

	void eraseInternal(int32_t index, bool destroy_instance)
	{
		GAFF_ASSERT(index >= 0 && index < size());

		const Metadata& metadata = _metadata[index];

		if (metadata.ref_def) {
			T* const instance = at(index);

			if (destroy_instance) {
				metadata.ref_def->destroyInstance(instance);
			}

			const auto start = _instances.begin() + metadata.start;
			_instances.erase(start, start + metadata.size);
		}

		for (int32_t i = index + 1; i < size(); ++i) {
			_metadata[i].start -= metadata.size;
		}

		_metadata.erase(_metadata.begin() + index);
	}

	void insertInternal(int32_t index, const Metadata& metadata, const Vector<uint8_t>& value)
	{
		const int32_t start = _metadata[index].start;

		for (int32_t i = index; i < size(); ++i) {
			_metadata[i].start += metadata.size;
		}

		_metadata.insert(_metadata.begin() + index, metadata);
		_metadata[index].start = start;

		_instances.insert(_instances.begin() + start, value.begin(), value.end());
	}

	template <class U, class VarType>
	friend class VarInstancedArray;
};

NS_END

#include "Shibboleth_InstancedArrayReflection.h"
