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

#include <Shibboleth_Defines.h>
#include <Gaff_RefPtr.h>

NS_SHIBBOLETH

class IResource;

/*class IResourcePtr
{
public:
	static bool Load(const ISerializeReader& reader, IResourcePtr& out);
	static void Save(ISerializeWriter& writer, const IResourcePtr& value);

protected:
	Gaff::RefPtr<IResource> _resource;
};*/

template <class T>
class ResourcePtr final
{
public:
	static_assert(std::is_base_of_v<IResource, T>, "ResourcePtr requires type to derive from IResource.");

	explicit ResourcePtr(T* resource):
		_resource(resource)
	{
	}

	ResourcePtr(void):
		_resource(nullptr)
	{
	}

	ResourcePtr(const ResourcePtr<T>& res_ptr):
		_resource(res_ptr._resource)
	{
	}

	ResourcePtr(ResourcePtr<T>&& res_ptr):
		_resource(std::move(res_ptr._resource))
	{
	}

	bool operator==(const ResourcePtr<T>& rhs) const
	{
		return _resource == rhs._resource;
	}

	bool operator==(const T* rhs) const
	{
		return _resource == rhs;
	}

	bool operator!=(const ResourcePtr<T>& rhs) const
	{
		return _resource != rhs._resource;
	}

	bool operator!=(const T* rhs) const
	{
		return _resource != rhs;
	}

	bool operator<(const T* rhs) const
	{
		return _resource < rhs;
	}

	bool operator>(const T* rhs) const
	{
		return _resource > rhs;
	}

	ResourcePtr<T>& operator=(const ResourcePtr<T>& rhs)
	{
		_resource = rhs._resource;
		return *this;
	}

	ResourcePtr<T>& operator=(ResourcePtr<T>&& rhs)
	{
		_resource = std::move(rhs._resource);
		return *this;
	}

	ResourcePtr<T>& operator=(T* rhs)
	{
		_resource = rhs;
		return *this;
	}

	//const T* operator->(void) const
	//{
	//	return _resource;
	//}

	T* operator->(void) const
	{
		return _resource.get();
	}

	//const T& operator*(void) const
	//{
	//	return *_resource;
	//}

	T& operator*(void) const
	{
		return *_resource;
	}

	operator bool(void) const
	{
		return _resource.get() != nullptr;
	}

	//const T* get(void) const
	//{
	//	return _resource;
	//}

	T* get(void) const
	{
		return _resource.get();
	}

private:
	Gaff::RefPtr<T> _resource;
};

NS_END
