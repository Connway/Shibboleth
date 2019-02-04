/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include <Shibboleth_Reflection.h>

NS_SHIBBOLETH

class IFile;

class ILoadFileCallbackAttribute : public Gaff::IAttribute
{
public:
	ILoadFileCallbackAttribute(int32_t pool): _pool(pool) {}
	
	virtual void callCallback(void* object, IFile* file) const = 0;

	int32_t getPool(void) const
	{
		return _pool;
	}

private:
	int32_t _pool;
};

template <class T>
class LoadFileCallbackAttribute final : public ILoadFileCallbackAttribute
{
public:
	LoadFileCallbackAttribute(void (T::*callback)(IFile*), int32_t pool = JPI_ANY):
		ILoadFileCallbackAttribute(pool), _callback(callback)
	{
	}

	void callCallback(void* object, IFile* file) const override
	{
		T* const obj = reinterpret_cast<T*>(object);
		(obj->*_callback)(file);
	}

	Gaff::IAttribute* clone(void) const override
	{
		IAllocator& allocator = GetAllocator();
		return SHIB_ALLOCT_POOL(LoadFileCallbackAttribute, allocator.getPoolIndex("Reflection"), allocator, _callback);
	}

private:
	void (T::*_callback)(IFile*);

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(LoadFileCallbackAttribute, T);
};

template <class T>
LoadFileCallbackAttribute<T> MakeLoadFileCallbackAttribute(void (T::*callback)(IFile*), int32_t pool = JPI_ANY)
{
	return LoadFileCallbackAttribute<T>(callback, pool);
}

SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(LoadFileCallbackAttribute, T)
	.BASE(ILoadFileCallbackAttribute)
	.BASE(Gaff::IAttribute)
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END(LoadFileCallbackAttribute, T)

NS_END

SHIB_TEMPLATE_REFLECTION_DECLARE(LoadFileCallbackAttribute, T)
SHIB_TEMPLATE_REFLECTION_DEFINE(LoadFileCallbackAttribute, T)
