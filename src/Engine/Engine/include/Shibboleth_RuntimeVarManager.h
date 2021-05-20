/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include "Shibboleth_VectorMap.h"
#include "Shibboleth_SmartPtrs.h"

NS_SHIBBOLETH

class IRuntimeVar
{
public:
	virtual ~IRuntimeVar(void) = default;

	virtual const Gaff::IReflection& getReflection(void) const = 0;
};

template <class T>
class RuntimeVar final : public IRuntimeVar
{
public:
	const Gaff::IReflection& getReflection(void) const override
	{
		return Reflection<T>::GetInstance();
	}

	operator const T& (void) const
	{
		return _value;
	}

	operator T&(void)
	{
		return _value;
	}

private:
	T _value;

	static_assert(Reflection<T>::HasReflection, "Cannot make a RuntimeVar from a class without Reflection.");
};


class RuntimeVarManager final
{
public:
	template <class T>
	void addRuntimeVar(Gaff::Hash64 name, const T& initial_value = T())
	{
		RuntimeVar<T>* const runtime_var = SHIB_ALLOCT(RuntimeVar<T>, _allocator, initial_value);
		addRuntimeVar(name, runtime_var);
	}

	template <class T>
	void addRuntimeVar(const char* name, const T& initial_value = T())
	{
		addRuntimeVar(Gaff::FNV1aHash64String(name), initial_value);
	}

	template <class T>
	RuntimeVar<T>* getRuntimeVar(Gaff::Hash64 name) const
	{
		IRuntimeVar* const runtime_var = getRuntimeVar(Gaff::FNV1aHash64String(name));

		GAFF_ASSERT(!runtime_var || &Reflection<T>::GetInstance() == &runtime_var->getReflection());
		return static_cast<RuntimeVar<T>*>(runtime_var);
	}

	template <class T>
	RuntimeVar<T>* getRuntimeVar(const char* name) const
	{
		return getRuntimeVar<T>(Gaff::FNV1aHash64String(name));
	}

	void addRuntimeVar(Gaff::Hash64 name, IRuntimeVar& runtime_var);
	void addRuntimeVar(const char* name, IRuntimeVar& runtime_var);

	IRuntimeVar* getRuntimeVar(Gaff::Hash64 name) const;
	IRuntimeVar* getRuntimeVar(const char* name) const;

private:
	VectorMap< Gaff::Hash64, UniquePtr<IRuntimeVar> > _runtime_vars;
	ProxyAllocator _allocator{ "RuntimeVars" };
};

NS_END
