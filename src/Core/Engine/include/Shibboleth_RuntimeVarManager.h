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

#ifdef DEBUG
	#define RuntimeVariable(VarType) static Shibboleth::RuntimeVar<VarType>
	#define SHIB_RUNTIME_VAR_ENABLED
#else
	#define RuntimeVariable(VarType) static const VarType
#endif

#ifdef SHIB_RUNTIME_VAR_ENABLED

#include "Shibboleth_Reflection.h"
#include "Shibboleth_VectorMap.h"

NS_SHIBBOLETH

void RegisterRuntimeVars(void);


class IRuntimeVar
{
public:
	IRuntimeVar(Gaff::Hash64 name): _name(name) {}
	virtual ~IRuntimeVar(void) = default;

	virtual const Refl::IReflection& getReflection(void) const = 0;
	virtual void* getValue(void) = 0;

	const void* getValue(void) const
	{
		return const_cast<IRuntimeVar*>(this)->getValue();
	}

	Gaff::Hash64 getName(void) const
	{
		return _name;
	}

protected:
	static IRuntimeVar* _root;

	IRuntimeVar* _next = nullptr;
	Gaff::Hash64 _name;

	friend void RegisterRuntimeVars(void);
};

template <class T>
class RuntimeVar final : public IRuntimeVar
{
public:
	RuntimeVar(Gaff::Hash64 name, const T& initial_value):
		RuntimeVar(name)
	{
		_value = initial_value;
	}

	RuntimeVar(Gaff::Hash64 name, T&& initial_value):
		RuntimeVar(name)
	{
		_value = std::move(initial_value);
	}

	RuntimeVar(Gaff::Hash64 name):
		IRuntimeVar(name)
	{
		if (!_root) {
			_root = this;

		} else {
			_next = _root;
			_root = this;
		}
	}

	template <size_t name_size>
	RuntimeVar(const char (&name)[name_size], const T& initial_value):
		RuntimeVar(Gaff::FNV1aHash64Const(name), initial_value)
	{
	}

	template <size_t name_size>
	RuntimeVar(const char (&name)[name_size], T&& initial_value):
		RuntimeVar(Gaff::FNV1aHash64Const(name), std::move(initial_value))
	{
	}

	template <size_t name_size>
	RuntimeVar(const char (&name)[name_size]):
		RuntimeVar(Gaff::FNV1aHash64Const(name))
	{
	}

	const Refl::IReflection& getReflection(void) const override
	{
		return Refl::Reflection<T>::GetInstance();
	}

	void* getValue(void) override
	{
		return &_value;
	}

	// Potential thread safety issues. If problematic, can add some support for atomics and locks,
	// but don't want to overcomplicate it for now.
	operator const T& (void) const
	{
		return _value;
	}

	operator T&(void)
	{
		return _value;
	}

private:
	static_assert(Refl::Reflection<T>::HasReflection, "Cannot make a RuntimeVar from a class without Reflection.");

	T _value;
};


class RuntimeVarManager final
{
public:
	template <class Callback>
	bool forEach(Callback&& callback) const
	{
		for (IRuntimeVar* entry : _runtime_vars) {
			if (callback(*entry)) {
				return true;
			}
		}

		return false;
	}

	template <class Callback>
	bool forEach(Callback&& callback)
	{
		for (const IRuntimeVar* entry : _runtime_vars) {
			if (callback(*entry)) {
				return true;
			}
		}

		return false;
	}

	template <class T>
	RuntimeVar<T>* getRuntimeVar(Gaff::Hash64 name) const
	{
		IRuntimeVar* const runtime_var = getRuntimeVar(name);

		GAFF_ASSERT(!runtime_var || &Refl::Reflection<T>::GetInstance() == &runtime_var->getReflection());
		return static_cast<RuntimeVar<T>*>(runtime_var);
	}

	template <class T>
	RuntimeVar<T>* getRuntimeVar(const char* name) const
	{
		return getRuntimeVar<T>(Gaff::FNV1aHash64String(name));
	}

	void addRuntimeVar(IRuntimeVar& runtime_var);

	IRuntimeVar* getRuntimeVar(Gaff::Hash64 name) const;
	IRuntimeVar* getRuntimeVar(const char* name) const;

private:
	Vector<IRuntimeVar*> _runtime_vars{ ProxyAllocator("RuntimeVars") };
};

NS_END

#endif
