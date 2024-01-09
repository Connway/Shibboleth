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

#include "Reflection/Shibboleth_Reflection.h"

NS_SHIBBOLETH

template <class T>
class InstancedPtr final
{
	static_assert(Refl::Reflection<T>::HasReflection, "Cannot serialize if type does not have reflection.");

public:
	explicit InstancedPtr(const ProxyAllocator& allocator = ProxyAllocator()):
		_allocator(allocator)
	{
	}

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

	template <class U>
	InstancedPtr& operator=(InstancedPtr<U>&& rhs)
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");

		_allocator = rhs._allocator;
		_ref_def = rhs._ref_def;
		_ptr = std::move(rhs._ptr);
		return *this;
	}

	InstancedPtr& operator=(InstancedPtr<T>&& rhs)
	{
		_allocator = rhs._allocator;
		_ref_def = rhs._ref_def;
		_ptr = std::move(rhs._ptr);
		return *this;
	}

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

private:
	ProxyAllocator _allocator;
	const Refl::IReflectionDefinition* _ref_def = &Refl::Reflection<T>::GetReflectionDefinition();
	UniquePtr<T> _ptr;
};

NS_END



NS_REFLECTION

template <class T, class VarType>
class VarInstancedPtr;



template <class T, class VarType>
struct VarTypeHelper< T, Shibboleth::InstancedPtr<VarType> > final
{
	using ReflectionType = VarTypeHelper<T, VarType>::ReflectionType;
	using VariableType = VarType;
	using Type = VarInstancedPtr<T, VarType>;
	static constexpr bool k_can_copy = VarTypeHelper<T, VarType>::k_can_copy;
};



template <class T, class VarType>
class VarInstancedPtr final : public IVar<T>
{
public:
	using ReflectionType = VarTypeHelper<T, VarType>::ReflectionType;

	VarInstancedPtr(Shibboleth::InstancedPtr<VarType> T::* ptr);
	VarInstancedPtr(void) = default;

	static const Reflection<ReflectionType>& GetReflection(void);
	const IReflection& getReflection(void) const override;

	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool load(const Shibboleth::ISerializeReader& reader, void* object) override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object) override;

	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;
};

NS_END

#include "Shibboleth_InstancedPtr.inl"
