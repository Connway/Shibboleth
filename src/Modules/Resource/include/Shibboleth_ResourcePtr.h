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

#include "Shibboleth_IResource.h"

NS_SHIBBOLETH

template <class T>
class ResourcePtr final
{
public:
	using NoCountChange = Gaff::RefPtr<T>::NoCountChange;

	static_assert(std::is_base_of_v<IResource, T>, "ResourcePtr requires type to derive from IResource.");
	static_assert(Refl::Reflection<T>::HasReflection, "ResourcePtr requires type to have reflection.");

	ResourcePtr(T* resource, const NoCountChange& no_count_change):
		_resource(resource, no_count_change)
	{
	}

	explicit ResourcePtr(T* resource):
		_resource(resource)
	{
		if (_resource) {
			_resource->requestLoad();
		}
	}

	template <class U>
	explicit ResourcePtr(const ResourcePtr<U>& res_ptr):
		ResourcePtr(res_ptr.get())
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");
	}

	template <class U>
	explicit ResourcePtr(ResourcePtr<U>&& res_ptr):
		_resource(std::move(res_ptr._resource))
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");
	}

	ResourcePtr(const ResourcePtr<T>& res_ptr):
		ResourcePtr(res_ptr.get())
	{
	}

	ResourcePtr(ResourcePtr<T>&& res_ptr) = default;
	ResourcePtr(void) = default;

	~ResourcePtr(void)
	{
		if (_resource) {
			_resource->requestUnload();
		}
	}

	std::strong_ordering operator<=>(const T* rhs) const
	{
		return get() <=> rhs;
	}

	std::strong_ordering operator<=>(const ResourcePtr<T>& rhs) const
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
	std::strong_ordering operator<=>(const ResourcePtr<U>& rhs) const
	{
		static_assert(std::is_base_of_v<T, U>, "Comparing unrelated pointer types.");

		const T* const rhs_ptr = rhs.get();
		return (*this) <=> rhs_ptr;
	}

	ResourcePtr<T>& operator=(const ResourcePtr<T>& rhs)
	{
		*this = const_cast<T*>(rhs.get());
		return *this;
	}

	ResourcePtr<T>& operator=(ResourcePtr<T>&& rhs) = default;

	ResourcePtr<T>& operator=(T* rhs)
	{
		if (_resource) {
			_resource->requestUnload();
		}

		_resource = rhs;

		if (_resource) {
			_resource->requestLoad();
		}

		return *this;
	}

	const T* operator->(void) const
	{
		return get();
	}

	T* operator->(void)
	{
		return get();
	}

	const T& operator*(void) const
	{
		return *_resource;
	}

	T& operator*(void)
	{
		return *_resource;
	}

	operator bool(void) const
	{
		return _resource.get() != nullptr;
	}

	const T* get(void) const
	{
		return _resource.get();
	}

	T* get(void)
	{
		return _resource.get();
	}

	T* release(void)
	{
		// No unload request. Assuming the caller will make an unload request.
		return _resource.release();
	}

private:
	Gaff::RefPtr<T> _resource;
};

NS_END

NS_REFLECTION

template <class Derived, class Base>
Shibboleth::ResourcePtr<Derived> ReflectionCast(Shibboleth::ResourcePtr<Base>&& resource)
{
	if constexpr (std::is_same_v<Base, Derived>) {
		return Shibboleth::ResourcePtr<Derived>(std::move(resource));

	} else {
		Derived* const derived = ReflectionCast<Derived>(resource.get());

		if (derived) {
			resource.release();
		}

		return Shibboleth::ResourcePtr<Derived>(derived, typename Shibboleth::ResourcePtr<Derived>::NoCountChange());
	}
}

template <class Derived, class Base>
Shibboleth::ResourcePtr<Derived> ReflectionCast(const Shibboleth::ResourcePtr<Base>& resource)
{
	return Shibboleth::ResourcePtr<Derived>(ReflectionCast<Derived>(resource.get()));
}

NS_END

#include "Shibboleth_ResourcePtrReflection.h"
