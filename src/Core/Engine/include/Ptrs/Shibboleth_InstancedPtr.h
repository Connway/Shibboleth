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

#include "Reflection/Shibboleth_Reflection.h"

NS_SHIBBOLETH

template <class T>
class InstancedPtr final
{
	static_assert(Refl::Reflection<T>::HasReflection, "Cannot serialize if type does not have reflection.");

public:
	template <class U>
	explicit InstancedPtr(U* ptr, const ProxyAllocator& allocator = ProxyAllocator()):
		_ptr(ptr),
		_ref_def(&Refl::Reflection<U>::GetReflectionDefinition()),
		_allocator(allocator)
	{
		static_assert(Refl::Reflection<U>::HasReflection, "Assigning class does not have reflection.");
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");
	}

	explicit InstancedPtr(T* ptr, const ProxyAllocator& allocator = ProxyAllocator()):
		_ptr(ptr),
		_allocator(allocator)
	{
	}

	explicit InstancedPtr(const ProxyAllocator& allocator):
		_allocator(allocator)
	{
	}

	InstancedPtr(InstancedPtr<T>&& instanced_ptr) = default;
	InstancedPtr(void) = default;

	const Refl::IReflectionDefinition* getReflectionDefinition(void) const
	{
		return _ref_def;
	}

	T* get(void) const
	{
		return _ptr.get();
	}

	template <class U>
	void reset(U* ptr)
	{
		static_assert(Refl::Reflection<U>::HasReflection, "Assigning class does not have reflection.");
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");

		_ref_def = &Refl::Reflection<U>::GetReflectionDefinition();
		_ptr.reset(ptr);
	}

	void reset(T* ptr)
	{
		_ref_def = &Refl::Reflection<T>::GetReflectionDefinition();
		_ptr.reset(ptr);
	}

	T* release(void)
	{
		return _ptr.release();
	}

	template <class U>
	InstancedPtr& operator=(InstancedPtr<U>&& rhs)
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");

		_allocator = rhs._allocator;
		_ref_def = rhs._ref_def;
		_ptr = std::move(rhs._ptr);
		return *this;
	}

	InstancedPtr& operator=(InstancedPtr<T>&& rhs) = default;

	std::strong_ordering operator<=>(const T* rhs) const
	{
		return get() <=> rhs;
	}

	std::strong_ordering operator<=>(const InstancedPtr<T>& rhs) const
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
	std::strong_ordering operator<=>(const InstancedPtr<U>& rhs) const
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

	operator const T*(void) const
	{
		return get();
	}

	operator T*(void)
	{
		return get();
	}

	operator bool(void) const
	{
		return _ptr != nullptr;
	}

	ProxyAllocator& getAllocator(void)
	{
		return _allocator;
	}

private:
	UniquePtr<T> _ptr;

	const Refl::IReflectionDefinition* _ref_def = &Refl::Reflection<T>::GetReflectionDefinition();
	ProxyAllocator _allocator;
};

NS_END

#include "Shibboleth_InstancedPtrReflection.h"
