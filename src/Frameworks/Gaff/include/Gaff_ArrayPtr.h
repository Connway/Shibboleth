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

#include "Gaff_DefaultAllocator.h"
#include "Gaff_Assert.h"

NS_GAFF

template < class T, class Allocator, class Deleter = AllocatorDeleter<T, Allocator> >
class ArrayPtr final
{
public:
	ArrayPtr(ArrayPtr&& array_ptr):
		_data(array_ptr._data)
	{
		array_ptr._data.ptr = nullptr;
		array_ptr._data.size = 0;
	}

	ArrayPtr(T* data, size_t size): _data(data, size) {}
	ArrayPtr(void) = default;

	ArrayPtr(const ArrayPtr&) = delete;
	ArrayPtr(T*) = delete;

	~ArrayPtr(void)
	{
		destroy();
	}

	ArrayPtr& operator=(const ArrayPtr&) = delete;
	ArrayPtr& operator=(T*) = delete;

	ArrayPtr& operator=(ArrayPtr&& rhs)
	{
		destroy();

		_data = rhs._data;

		rhs._data.ptr = nullptr;
		rhs._data.size = 0;

		return *this;
	}

	const T& operator[](size_t index) const
	{
		GAFF_ASSERT(index < _data.size);
		return _data.ptr[index];
	}

	T& operator[](size_t index)
	{
		GAFF_ASSERT(index < _data.size);
		return _data.ptr[index];
	}

	const T& operator[](int32_t index) const
	{
		GAFF_ASSERT(index >= 0 && static_cast<size_t>(index) < _data.size);
		return _data.ptr[index];
	}

	T& operator[](int32_t index)
	{
		GAFF_ASSERT(index >= 0 && static_cast<size_t>(index) < _data.size);
		return _data.ptr[index];
	}

	std::strong_ordering operator<=>(const ArrayPtr& rhs) const
	{
		return *this <=> rhs._data.ptr;
	}

	std::strong_ordering operator<=>(const T* rhs) const
	{
		return _data.ptr <=> rhs;
	}

	bool valid(void) const { return _data.ptr; }
	size_t size(void) const { return _data.size; }
	const T* get(void) const { return _data.ptr; }
	T* get(void) { return _data.ptr; }

	operator bool(void) const { return valid(); }

	const T* begin(void) const { return _data.ptr; }
	T* begin(void) { return _data.ptr; }
	const T* end(void) const { return _data.ptr + _data.size; }
	T* end(void) { return _data.ptr + _data.size; }


	template <class... Args>
	void constructElement(size_t index, Args&&... args)
	{
		GAFF_ASSERT(index < _data.size);
		Gaff::Construct(_data.ptr + index, std::forward<Args>(args)...);
	}

	template <class... Args>
	void construct(Args&&... args)
	{
		for (size_t i = 0; i < _data.size; ++i) {
			Gaff::Construct(_data.ptr + i, std::forward<Args>(args)...);
		}
	}

	void destroy(void)
	{
		for (size_t i = 0; i < _data.size; ++i) {
			Gaff::Deconstruct(_data.ptr + i);
		}

		_data.destroy();

		_data.ptr = nullptr;
		_data.size = 0;
	}

	const Deleter& getDeleter(void) const { return _data.getDeleter(); }
	Deleter& getDeleter(void) { return _data.getDeleter(); }
	void setDeleter(const Deleter& deleter) { _data.setDeleter(deleter); }

private:
	template <bool deleter_requires_storage>
	struct ArrayPtrData;

	template <>
	struct ArrayPtrData<true> final
	{
		Deleter deleter;
		T* ptr = nullptr;
		size_t size = 0;

		ArrayPtrData(T* in_ptr = nullptr, size_t in_size = 0): ptr(in_ptr), size(in_size) {}

		void destroy(void) { deleter(ptr); }
		const Deleter& getDeleter(void) const { return deleter; }
		Deleter& getDeleter(void) { return deleter; }
		void setDeleter(const Deleter& in_deleter) { deleter = in_deleter; }
	};

	template <>
	struct ArrayPtrData<false> final
	{
		T* ptr = nullptr;
		size_t size = 0;

		ArrayPtrData(T* in_ptr = nullptr, size_t in_size = 0): ptr(in_ptr), size(in_size) {}

		void destroy(void)
		{
			Deleter deleter;
			deleter(ptr);
		}

		const Deleter& getDeleter(void) const
		{
			GAFF_ASSERT_MSG(false, "ArrayPtr::ArrayPtrData: Attempting to get a deleter for a deleter type that doesn't require data.");
			Deleter* deleter = nullptr;
			return *deleter;
		}

		Deleter& getDeleter(void)
		{
			GAFF_ASSERT_MSG(false, "ArrayPtr::ArrayPtrData: Attempting to get a deleter for a deleter type that doesn't require data.");
			Deleter* deleter = nullptr;
			return *deleter;
		}

		void setDeleter(const Deleter&)
		{
			GAFF_ASSERT_MSG(false, "ArrayPtr::ArrayPtrData: Attempting to set a deleter for a deleter type that doesn't require data.");
		}
	};

	template <bool has_requires_storage>
	struct RequiresStorage;

	template <>
	struct RequiresStorage<true> final
	{
		static constexpr bool value = Deleter::RequiresStorage;
	};

	template <>
	struct RequiresStorage<false> final
	{
		static constexpr bool value = false;
	};

	static constexpr bool k_has_requires_storage = requires() { Deleter::RequiresStorage; };

	ArrayPtrData< RequiresStorage<k_has_requires_storage>::value > _data;
};

NS_END
