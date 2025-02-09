/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "Gaff_Hash.h"
#include "Reflection/Shibboleth_IReflectionObject.h"
#include "Reflection/Shibboleth_Reflection.h"

NS_SHIBBOLETH

template <class T>
class InstancedArray;

class InstancedArrayAny
{
public:
	class Iterator
	{
	public:
		explicit Iterator(InstancedArrayAny& owner, int32_t index = 0);

		Iterator(const Iterator& it) = default;
		Iterator(void) = default;

		Iterator& operator=(const Iterator& rhs) = default;

		Iterator operator+(int32_t offset) const;
		Iterator& operator+=(int32_t offset);

		Iterator& operator++(void);
		Iterator operator++(int);

		Iterator operator-(int32_t offset) const;
		Iterator& operator-=(int32_t offset);

		Iterator& operator--(void);
		Iterator operator--(int);

		/*T& operator*(void) const
		{
			T* const entry = get();
			GAFF_ASSERT(entry);

			return *entry;
		}

		T* operator->(void) const
		{
			return get();
		}*/

		int32_t getIndex(void) const;
		void* get(void) const;

		bool operator==(const Iterator& rhs) const;

		auto operator<=>(const Iterator& rhs) const
		{
			return _index <=> rhs._index;
		}

	private:
		InstancedArrayAny* _owner = nullptr;
		int32_t _index = -1;
	};

	class ConstIterator
	{
	public:
		explicit ConstIterator(const InstancedArrayAny& owner, int32_t index = 0);
		ConstIterator(const Iterator& it);

		ConstIterator(const ConstIterator& it) = default;
		ConstIterator(void) = default;

		ConstIterator& operator=(const ConstIterator& rhs) = default;
		ConstIterator& operator=(const Iterator& rhs);

		ConstIterator operator+(int32_t offset) const;
		ConstIterator& operator+=(int32_t offset);

		ConstIterator& operator++(void);
		ConstIterator operator++(int);

		ConstIterator operator-(int32_t offset) const;
		ConstIterator& operator-=(int32_t offset);

		ConstIterator& operator--(void);
		ConstIterator operator--(int);

		/*const T& operator*(void) const
		{
			const T* const entry = get();
			GAFF_ASSERT(entry);

			return *entry;
		}

		const T* operator->(void) const
		{
			return get();
		}*/

		int32_t getIndex(void) const;
		const void* get(void) const;

		bool operator==(const ConstIterator& rhs) const;
		bool operator==(const Iterator& rhs) const;

		auto operator<=>(const Iterator& rhs) const
		{
			return _iterator <=> rhs;
		}

		auto operator<=>(const ConstIterator& rhs) const
		{
			return *this <=> rhs._iterator;
		}

	private:
		Iterator _iterator;
	};

	explicit InstancedArrayAny(const ProxyAllocator& allocator);

	InstancedArrayAny(InstancedArrayAny&& instanced_array) = default;
	InstancedArrayAny(void) = default;

	InstancedArrayAny& operator=(InstancedArrayAny&& instanced_array) = default;

	const void* operator[](int32_t index) const;
	void* operator[](int32_t index);

	const void* at(int32_t index) const;
	void* at(int32_t index);

	const Refl::IReflectionDefinition* getReflectionDefinition(int32_t index) const;

	int32_t size(void) const;
	bool empty(void) const;

	ConstIterator begin(void) const;
	ConstIterator end(void) const;
	Iterator begin(void);
	Iterator end(void);

	/*const T& back(void) const
	{
		return const_cast<InstancedArray<T>*>(this)->back();
	}

	T& back(void)
	{
		GAFF_ASSERT(!empty());
		return *at(size() - 1);
	}*/

	template <class U, class... Args>
	U& pushT(Args&&... args)
	{
		static_assert(Refl::Reflection<U>::k_has_reflection, "U does not have reflection.");

		const Refl::ReflectionDefinition<U>& ref_def = Refl::Reflection<U>::GetReflectionDefinition();
		GAFF_ASSERT(!_base_ref_def || ref_def.hasInterface(*_base_ref_def))

		U* const instance = reinterpret_cast<U*>(pushInternal(ref_def));
		Gaff::Construct(instance, std::forward<Args>(args)...);

		return *instance;
	}

	template <class... Args>
	void* push(const Refl::IReflectionDefinition& ref_def, Args&&... args)
	{
		void* const instance = pushInternal(ref_def);

		const auto constructor = ref_def.getConstructor<Args...>();
		GAFF_ASSERT(constructor);

		constructor(instance, std::forward<Args>(args)...);

		return instance;
	}

	void pushEmpty(void);
	void pop(void);

	void erase(const ConstIterator& iterator);
	void erase(int32_t index);

	void resize(int32_t new_size);
	void swap(int32_t index_a, int32_t index_b);

	void clone(InstancedArrayAny& out) const;

private:
	struct Metadata final
	{
		const Refl::IReflectionDefinition* ref_def = nullptr;
		int32_t offset = 0;
		int32_t start = -1;
		int32_t size = 0;
	};

	Vector<Metadata> _metadata;
	Vector<uint8_t> _instances; // Byte buffer.

	const Refl::IReflectionDefinition* _base_ref_def = nullptr;

	// Only usable by template version of InstancedArray.
	InstancedArrayAny(const Refl::IReflectionDefinition& ref_def, const ProxyAllocator& allocator);

	void* pushInternal(const Refl::IReflectionDefinition& ref_def);
	void eraseInternal(int32_t index, bool destroy_instance);
	void insertInternal(int32_t index, const Metadata& metadata, const Vector<uint8_t>& value);

	template <class U>
	friend class VarInstancedArrayAny;

	template <class U, class VarType>
	friend class VarInstancedArray;

	template <class T>
	friend class InstancedArray;
};

template <class T>
class InstancedArray final : public InstancedArrayAny
{
	static_assert(Refl::Reflection<T>::k_has_reflection, "Cannot serialize if type does not have reflection.");

public:
	class Iterator final : public InstancedArrayAny::Iterator
	{
	public:
		explicit Iterator(InstancedArray<T>& owner, int32_t index = 0):
			InstancedArrayAny::Iterator(owner, index)
		{
		}

		Iterator(const Iterator& it) = default;
		Iterator(void) = default;

		Iterator& operator=(const Iterator& rhs) = default;

		Iterator operator+(int32_t offset) const
		{
			return Iterator{ InstancedArrayAny::Iterator::operator+(offset) };
		}

		Iterator& operator+=(int32_t offset)
		{
			InstancedArrayAny::Iterator::operator+=(offset);
			return *this;
		}

		Iterator& operator++(void)
		{
			InstancedArrayAny::Iterator* const it = this;
			++(*it);

			return *this;
		}

		Iterator operator++(int)
		{
			InstancedArrayAny::Iterator* const it = this;
			return Iterator{ (*it)++ };
		}

		Iterator operator-(int32_t offset) const
		{
			return Iterator{ InstancedArrayAny::Iterator::operator-(offset) };
		}

		Iterator& operator-=(int32_t offset)
		{
			InstancedArrayAny::Iterator::operator-=(offset);
			return *this;
		}

		Iterator& operator--(void)
		{
			InstancedArrayAny::Iterator* const it = this;
			--(*it);

			return *this;
		}

		Iterator operator--(int)
		{
			InstancedArrayAny::Iterator* const it = this;
			return Iterator{ (*it)-- };
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

		T* get(void) const
		{
			return reinterpret_cast<T*>(InstancedArrayAny::Iterator::get());
		}

		auto operator<=>(const Iterator& rhs) const
		{
			return InstancedArrayAny::Iterator::operator<=>(rhs);
		}

		bool operator==(const Iterator& rhs) const
		{
			return InstancedArrayAny::Iterator::operator==(rhs);
		}

	private:
		Iterator(const InstancedArrayAny::Iterator& it):
			InstancedArrayAny::Iterator(it)
		{
		}
	};

	class ConstIterator final : public InstancedArrayAny::ConstIterator
	{
	public:
		explicit ConstIterator(const InstancedArray<T>& owner, int32_t index = 0):
			InstancedArrayAny::ConstIterator(const_cast<InstancedArray<T>&>(owner), index)
		{
		}

		ConstIterator(const Iterator& it):
			InstancedArrayAny::Iterator(it)
		{
		}

		ConstIterator(const ConstIterator& it) = default;
		ConstIterator(void) = default;

		ConstIterator& operator=(const ConstIterator& rhs) = default;
		ConstIterator& operator=(const Iterator& rhs)
		{
			InstancedArrayAny::ConstIterator::operator=(rhs);
			return *this;
		}

		ConstIterator operator+(int32_t offset) const
		{
			return ConstIterator{ InstancedArrayAny::ConstIterator::operator+(offset) };
		}

		ConstIterator& operator+=(int32_t offset)
		{
			InstancedArrayAny::ConstIterator::operator+=(offset);
			return *this;
		}

		ConstIterator& operator++(void)
		{
			InstancedArrayAny::ConstIterator* const it = this;
			++(*it);

			return *this;
		}

		ConstIterator operator++(int)
		{
			InstancedArrayAny::ConstIterator* const it = this;
			return ~ConstIterator{ (*it)++ };
		}

		ConstIterator operator-(int32_t offset) const
		{
			return ConstIterator{ InstancedArrayAny::ConstIterator::operator-(offset) };
		}

		ConstIterator& operator-=(int32_t offset)
		{
			InstancedArrayAny::ConstIterator::operator-=(offset);
			return *this;
		}

		ConstIterator& operator--(void)
		{
			InstancedArrayAny::ConstIterator* const it = this;
			--(*it);

			return *this;
		}

		ConstIterator operator--(int)
		{
			InstancedArrayAny::ConstIterator* const it = this;
			return ConstIterator{ (*it)-- };
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

		const T* get(void) const
		{
			return reinterpret_cast<const T*>(InstancedArrayAny::ConstIterator::get());
		}

		auto operator<=>(const Iterator& rhs) const
		{
			return InstancedArrayAny::ConstIterator::operator<=>(rhs);
		}

		auto operator<=>(const ConstIterator& rhs) const
		{
			return InstancedArrayAny::ConstIterator::operator<=>(rhs);
		}

		bool operator==(const Iterator& rhs) const
		{
			return InstancedArrayAny::ConstIterator::operator==(rhs);
		}

		bool operator==(const ConstIterator& rhs) const
		{
			return InstancedArrayAny::ConstIterator::operator==(rhs);
		}

	private:
		ConstIterator(const InstancedArrayAny::ConstIterator& it):
			InstancedArrayAny::ConstIterator(it)
		{
		}
	};



	explicit InstancedArray(const ProxyAllocator& allocator):
		InstancedArrayAny(Refl::Reflection<T>::GetReflectionDefinition(), allocator)
	{
	}

	InstancedArray(void):
		InstancedArray(ProxyAllocator{})
	{
	}

	InstancedArray(InstancedArray<T>&& instanced_array) = default;

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
		return reinterpret_cast<T*>(InstancedArrayAny::at(index));
	}

	ConstIterator begin(void) const
	{
		return ConstIterator{ *this };
	}

	ConstIterator end(void) const
	{
		return ConstIterator{ *this, size() };
	}

	Iterator begin(void)
	{
		return Iterator{ *this };
	}

	Iterator end(void)
	{
		return Iterator{ *this, size() };
	}

	const T& back(void) const
	{
		return const_cast<InstancedArray<T>*>(this)->back();
	}

	T& back(void)
	{
		return *at(size() - 1);
	}

	template <class... Args>
	T& push(const Refl::IReflectionDefinition& ref_def, Args&&... args)
	{
		void* const instance = InstancedArrayAny::push(ref_def, std::forward<Args>(args)...);
		return *reinterpret_cast<T*>(reinterpret_cast<int8_t*>(instance) + _metadata.back().offset);
	}

	template <class U>
	friend class VarInstancedArrayAny;

	template <class U, class VarType>
	friend class VarInstancedArray;
};

NS_END

#include "Shibboleth_InstancedArrayReflection.h"
