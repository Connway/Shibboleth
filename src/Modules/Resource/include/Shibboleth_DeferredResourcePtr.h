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

#include "Shibboleth_ResourcePtr.h"

NS_SHIBBOLETH

ResourcePtr<IResource> DeferredResourceRequestResourceHelper(const HashString64<>& _file_path, const Refl::IReflectionDefinition& ref_def);

template <class T>
class DeferredResourcePtr final
{
public:
	using NoCountChange = Gaff::RefPtr<T>::NoCountChange;

	static_assert(std::is_base_of_v<IResource, T>, "DeferredResourcePtr requires type to derive from IResource.");
	static_assert(Refl::Reflection<T>::HasReflection, "DeferredResourcePtr requires type to have reflection.");

	DeferredResourcePtr(T* resource, const NoCountChange& no_count_change):
		_ref_def(&Refl::Reflection<T>::GetReflectionDefinition()),
		_file_path((resource) ? resource->getFilePath() : HashString64<>()),
		_resource(resource, no_count_change)
	{
	}

	explicit DeferredResourcePtr(T* resource):
		_ref_def((resource) ? &resource->getReflectionDefinition() : &Refl::Reflection<T>::GetReflectionDefinition()),
		_file_path((resource) ? resource->getFilePath() : HashString64<>()),
		_resource(resource)
	{
	}

	template <class U>
	explicit DeferredResourcePtr(const DeferredResourcePtr<U>& res_ptr):
		_ref_def(res_ptr._ref_def), _file_path(res_ptr._file_path), _resource(res_ptr._resource)
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");
	}

	template <class U>
	explicit DeferredResourcePtr(DeferredResourcePtr<U>&& res_ptr):
		_ref_def(res_ptr._ref_def), _file_path(std::move(res_ptr._file_path)), _resource(std::move(res_ptr._resource))
	{
		static_assert(std::is_base_of_v<T, U>, "Assigning unrelated pointer types.");
	}

	explicit DeferredResourcePtr(const HashString64<>& file_path, const Refl::IReflectionDefinition* ref_def = &Refl::Reflection<T>::GetReflectionDefinition()):
		_ref_def(ref_def), _file_path(file_path), _resource(nullptr)
	{
	}

	explicit DeferredResourcePtr(HashString64<>&& file_path, const Refl::IReflectionDefinition* ref_def = &Refl::Reflection<T>::GetReflectionDefinition()):
		_ref_def(ref_def), _file_path(std::move(file_path)), _resource(nullptr)
	{
	}

	DeferredResourcePtr(const DeferredResourcePtr<T>& res_ptr) = default;
	DeferredResourcePtr(DeferredResourcePtr<T>&& res_ptr) = default;
	DeferredResourcePtr(void) = default;

	std::strong_ordering operator<=>(const T* rhs) const
	{
		return get() <=> rhs;
	}

	std::strong_ordering operator<=>(const DeferredResourcePtr<T>& rhs) const
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
	std::strong_ordering operator<=>(const DeferredResourcePtr<U>& rhs) const
	{
		static_assert(std::is_base_of_v<T, U>, "Comparing unrelated pointer types.");

		const T* const rhs_ptr = rhs.get();
		return (*this) <=> rhs_ptr;
	}

	DeferredResourcePtr<T>& operator=(const DeferredResourcePtr<T>& rhs) = default;
	DeferredResourcePtr<T>& operator=(DeferredResourcePtr<T>&& rhs) = default;

	DeferredResourcePtr<T>& operator=(T* rhs)
	{
		_ref_def = &rhs->getReflectionDefinition();
		_file_path = (rhs) ? rhs->getFilePath() : HashString64<>();
		_resource = rhs;
		return *this;
	}

	DeferredResourcePtr<T>& operator=(const HashString64<>& rhs)
	{
		_ref_def = &Refl::Reflection<T>::GetReflectionDefinition();
		_file_path = rhs;
		_resource = nullptr;
		return *this;
	}

	DeferredResourcePtr<T>& operator=(HashString64<>&& rhs)
	{
		_ref_def = &Refl::Reflection<T>::GetReflectionDefinition();
		_file_path = std::move(rhs);
		_resource = nullptr;
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

	operator ResourcePtr<T>(void) const
	{
		return _resource;
	}

	const T* get(void) const
	{
		return _resource.get();
	}

	T* get(void)
	{
		return _resource.get();
	}

	const Refl::IReflectionDefinition* getReflectionDefinition(void) const
	{
		return _ref_def;
	}

	const HashString64<>& getFilePath(void) const
	{
		return _file_path;
	}

	void requestLoad(void)
	{
		GAFF_ASSERT(!_file_path.empty() && _ref_def);
		_resource = ReflectionCast<T>(DeferredResourceRequestResourceHelper(_file_path, *_ref_def));
	}

private:
	const Refl::IReflectionDefinition* _ref_def = &Refl::Reflection<T>::GetReflectionDefinition();
	HashString64<> _file_path;

	ResourcePtr<T> _resource;
};

NS_END

NS_REFLECTION

template <class Derived, class Base>
Shibboleth::DeferredResourcePtr<Derived> ReflectionCast(Shibboleth::DeferredResourcePtr<Base>&& resource)
{
	if constexpr (std::is_same_v<Base, Derived>) {
		return Shibboleth::DeferredResourcePtr<Derived>(std::move(resource));

	} else {
		if (resource) {
			Derived* const derived = ReflectionCast<Derived>(resource.get());

			if (derived) {
				resource.release();
			}

			return Shibboleth::DeferredResourcePtr<Derived>(derived, typename Shibboleth::DeferredResourcePtr<Derived>::NoCountChange());

		} else {
			if (resource.getReflectionDefinition()->template hasInterface<Derived>()) {
				return Shibboleth::DeferredResourcePtr<Derived>(resource.getFilePath());
			} else {
				return Shibboleth::DeferredResourcePtr<Derived>();
			}
		}
	}
}

template <class Derived, class Base>
Shibboleth::DeferredResourcePtr<Derived> ReflectionCast(const Shibboleth::DeferredResourcePtr<Base>& resource)
{
	if (resource) {
		return Shibboleth::DeferredResourcePtr<Derived>(ReflectionCast<Derived>(resource.get()));

	} else {
		if (resource.getReflectionDefinition()->template hasInterface<Derived>()) {
			return Shibboleth::DeferredResourcePtr<Derived>(resource.getFilePath());
		} else {
			return Shibboleth::DeferredResourcePtr<Derived>();
		}
	}
}

NS_END

#include "Shibboleth_DeferredResourcePtrReflection.h"
