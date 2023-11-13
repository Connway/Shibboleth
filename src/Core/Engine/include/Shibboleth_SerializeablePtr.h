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

#include "Shibboleth_Reflection.h"

NS_SHIBBOLETH

// This class is not intended to be used directly.
class ISerializeablePtr
{
public:
	static bool Load(const ISerializeReader& reader, ISerializeablePtr& out);
	static void Save(ISerializeWriter& writer, const ISerializeablePtr& value);

protected:
	ISerializeablePtr(Refl::IReflectionObject* ptr, const ProxyAllocator& allocator):
		_allocator(allocator), _ptr(ptr)
	{
	}

	ProxyAllocator _allocator;
	Refl::IReflectionObject* _ptr = nullptr;
};

template <class T>
class SerializeablePtr final : public ISerializeablePtr
{
	static_assert(Refl::Reflection<T>::HasReflection, "Cannot serialize if type does not have reflection.");
	static_assert(std::is_base_of_v<Refl::IReflectionObject, T>, "Must inherit from IReflectionObject.");

public:
	template <class U>
	explicit SerializeablePtr(U* ptr, const ProxyAllocator& allocator = ProxyAllocator()):
		ISerializeablePtr(ptr, allocator)
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");
	}

	explicit SerializeablePtr(T* ptr, const ProxyAllocator& allocator = ProxyAllocator()):
		ISerializeablePtr(ptr, allocator)
	{
	}

	template <class U>
	SerializeablePtr(const SerializeablePtr<U>& serializeable_ptr):
		ISerializeablePtr(const_cast<U*>(serializeable_ptr.get()), serializeable_ptr._allocator)
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");
	}

	SerializeablePtr(const SerializeablePtr<T>& serializeable_ptr):
		ISerializeablePtr(serializeable_ptr._ptr, serializeable_ptr._allocator)
	{
	}

	SerializeablePtr(const ProxyAllocator& allocator = ProxyAllocator()):
		ISerializeablePtr(nullptr, allocator)
	{
	}

	T* get(void) const
	{
		return static_cast<T*>(_ptr);
	}

	operator T*(void) const
	{
		return get();
	}

	template <class U>
	SerializeablePtr& operator=(const SerializeablePtr<U>& rhs)
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");

		_allocator = rhs._allocator;
		_ptr = rhs._ptr;
		return *this;
	}

	SerializeablePtr& operator=(const SerializeablePtr<T>& rhs)
	{
		_allocator = rhs._allocator;
		_ptr = rhs._ptr;
		return *this;
	}

	template <class U>
	SerializeablePtr& operator=(U* rhs)
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");

		_ptr = rhs;
		return *this;
	}

	SerializeablePtr& operator=(T* rhs)
	{
		_ptr = rhs;
		return *this;
	}

	std::strong_ordering operator<=>(const T* rhs) const
	{
		if (get() == rhs) {
			return std::strong_ordering::equal;
		} else if (get() < rhs) {
			return std::strong_ordering::less;
		} else {
			return std::strong_ordering::greater;
		}
	}

	std::strong_ordering operator<=>(const SerializeablePtr<T>& rhs) const
	{
		return (*this) <=> rhs.get();
	}

	template <class U>
	std::strong_ordering operator<=>(const U* rhs) const
	{
		static_assert(std::is_base_of_v<T, U>, "Comparing unrelated pointer types.");

		const T* const rhs_ptr = rhs;
		return (*this) <=> rhs_ptr;
	}

	template <class U>
	std::strong_ordering operator<=>(const SerializeablePtr<U>& rhs) const
	{
		static_assert(std::is_base_of_v<T, U>, "Comparing unrelated pointer types.");

		const T* const rhs_ptr = rhs.get();
		return (*this) <=> rhs_ptr;
	}

	const T& operator*(void) const
	{
		return *get();
	}

	T& operator*(void)
	{
		return *get();
	}

	const T* operator->(void) const
	{
		return get();
	}

	T* operator->(void)
	{
		return get();
	}

	operator bool(void) const
	{
		return _ptr != nullptr;
	}
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ISerializeablePtr)
