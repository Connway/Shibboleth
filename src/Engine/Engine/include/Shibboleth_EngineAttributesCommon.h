/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_Broadcaster.h"
#include "Shibboleth_Reflection.h"
#include <Gaff_Flags.h>

NS_SHIBBOLETH

class ReadOnlyAttribute final : public Gaff::IAttribute
{
public:
	IAttribute* clone(void) const override;

	template <class T, class Var>
	void apply(Gaff::IReflectionVar& ref_var, Var T::*) { ref_var.setReadOnly(true);  }

	template <class T, class Var, class Ret>
	void apply(Gaff::IReflectionVar& ref_var, Ret (T::*)(void) const, void (T::*)(Var)) { ref_var.setReadOnly(true); }

	SHIB_REFLECTION_CLASS_DECLARE(ReadOnlyAttribute);
};


class RangeAttribute final : public Gaff::IAttribute
{
public:
	RangeAttribute(double min = std::numeric_limits<double>::min(), double max = std::numeric_limits<double>::max(), double step = 1.0);

	double getStep(void) const;
	double getMin(void) const;
	double getMax(void) const;

	IAttribute* clone(void) const override;

private:
	double _step;
	double _min;
	double _max;

	SHIB_REFLECTION_CLASS_DECLARE(RangeAttribute);
};


// Gaff::Hash32 and Gaff::Hash64 are just typedefs for uint32_t and uint64_t respectively.
// Use this attribute on vars using those types to denote that it is a string hash and not a normal integer. 
class HashStringAttribute final : public Gaff::IAttribute
{
public:
	IAttribute* clone(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(HashStringAttribute);
};


class OptionalAttribute final : public Gaff::IAttribute
{
public:
	IAttribute* clone(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(OptionalAttribute);
};


class ScriptFlagsAttribute final : public Gaff::IAttribute
{
public:
	enum class Flag
	{
		ReferenceOnly,
		NoRegister,
		NoInherit,

		Count
	};

	template <class... Flags>
	ScriptFlagsAttribute(Flags... flags)
	{
		if constexpr (sizeof...(Flags) > 0) {
			_flags.set(true, flags...);
		}
	}

	ScriptFlagsAttribute(Gaff::Flags<Flag> flags):
		_flags(flags)
	{
	}

	IAttribute* clone(void) const override;

	bool canInherit(void) const override { return !_flags.testAll(Flag::NoInherit); }
	Gaff::Flags<Flag> getFlags(void) const { return _flags; }

private:
	Gaff::Flags<Flag> _flags;

	SHIB_REFLECTION_CLASS_DECLARE(ScriptFlagsAttribute);
};


// Template Attributes
template <class T, class Msg>
class GlobalMessageAttribute final : public Gaff::IAttribute
{
public:
	IAttribute* clone(void) const override
	{
		IAllocator& allocator = GetAllocator();
		return SHIB_ALLOCT_POOL(GlobalMessageAttribute, allocator.getPoolIndex("Reflection"), allocator);
	}

	void instantiated(const Gaff::IReflectionDefinition& ref_def, void* object) override
	{
		Broadcaster& broadcaster = GetApp().getBroadcaster();
		T* const instance = ref_def.getInterface<T>(object);

		const BroadcastID id = 
			(_const_func) ?
			broadcaster.listen(MemberFunc(instance, _const_func)) :
			broadcaster.listen(MemberFunc(instance, _func));


		s_removers.emplace(object, BroadcastRemover(id, broadcaster));
		GetAllocator().addOnFreeCallback(OnFree, object);
	}

	void apply(void (T::*func)(const Msg&) const)
	{
		_const_func = func;
	}

	void apply(void (T::*func)(const Msg&))
	{
		_func = func;
	}

private:
	void (T::*_const_func)(const Msg&) const = nullptr;
	void (T::*_func)(const Msg&) = nullptr;

	static VectorMap<void*, BroadcastRemover> s_removers;

	static void OnFree(void* object)
	{
		s_removers.erase(object);
	}

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(GlobalMessageAttribute, T, Msg);
};

SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE(GlobalMessageAttribute, T, Msg)

template <class T, class Msg>
VectorMap<void*, BroadcastRemover> GlobalMessageAttribute<T, Msg>::s_removers = VectorMap<void*, BroadcastRemover>();

NS_END


SHIB_REFLECTION_DECLARE(ReadOnlyAttribute)
SHIB_REFLECTION_DECLARE(RangeAttribute)
SHIB_REFLECTION_DECLARE(HashStringAttribute)
SHIB_REFLECTION_DECLARE(OptionalAttribute)
SHIB_REFLECTION_DECLARE(ScriptFlagsAttribute)

SHIB_TEMPLATE_REFLECTION_DECLARE(GlobalMessageAttribute, T, Msg)

SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(GlobalMessageAttribute, T, Msg)
	.BASE(Gaff::IAttribute)
SHIB_TEMPLATE_REFLECTION_DEFINE_END(GlobalMessageAttribute, T, Msg)
