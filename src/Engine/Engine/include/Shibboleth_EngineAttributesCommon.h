/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

//#include "Shibboleth_Broadcaster.h"
#include "Shibboleth_Reflection.h"
#include <limits>

NS_SHIBBOLETH

class ReadOnlyAttribute final : public Refl::IAttribute
{
public:
	Refl::IAttribute* clone(void) const override;

	template <class T, class Var>
	void apply(Refl::IReflectionVar& ref_var, Var T::*) { ref_var.setReadOnly(true); }

	template <class T, class Var, class Ret>
	void apply(Refl::IReflectionVar& ref_var, Ret (T::*)(void) const, void (T::*)(Var)) { ref_var.setReadOnly(true); }

	template <class T, class Var, class Vec_Allocator, size_t size>
	void apply(Refl::IReflectionVar& ref_var, Gaff::Vector<Var, Vec_Allocator> T::* /*vec*/) { ref_var.setReadOnly(true); }

	template <class T, class Var, size_t size>
	void apply(Refl::IReflectionVar& ref_var, Var (T::* /*arr*/)[size]) { ref_var.setReadOnly(true); }

	SHIB_REFLECTION_CLASS_DECLARE(ReadOnlyAttribute);
};



class NoSerializeAttribute final : public Refl::IAttribute
{
public:
	Refl::IAttribute* clone(void) const override;

	template <class T, class Var>
	void apply(Refl::IReflectionVar& ref_var, Var T::*) { ref_var.setNoSerialize(true); }

	template <class T, class Var, class Ret>
	void apply(Refl::IReflectionVar& ref_var, Ret (T::*)(void) const, void (T::*)(Var)) { ref_var.setNoSerialize(true); }

	template <class T, class Var, class Vec_Allocator, size_t size>
	void apply(Refl::IReflectionVar& ref_var, Gaff::Vector<Var, Vec_Allocator> T::* /*vec*/) { ref_var.setNoSerialize(true); }

	template <class T, class Var, size_t size>
	void apply(Refl::IReflectionVar& ref_var, Var (T::* /*arr*/)[size]) { ref_var.setNoSerialize(true); }

	SHIB_REFLECTION_CLASS_DECLARE(NoSerializeAttribute);
};



class OptionalAttribute final : public Refl::IAttribute
{
public:
	Refl::IAttribute* clone(void) const override;

	template <class T, class Var>
	void apply(Refl::IReflectionVar& ref_var, Var T::*) { ref_var.setOptional(true); }

	template <class T, class Var, class Ret>
	void apply(Refl::IReflectionVar& ref_var, Ret (T::*)(void) const, void (T::*)(Var)) { ref_var.setOptional(true); }

	template <class T, class Var, class Vec_Allocator, size_t size>
	void apply(Refl::IReflectionVar& ref_var, Gaff::Vector<Var, Vec_Allocator> T::* /*vec*/) { ref_var.setOptional(true); }

	template <class T, class Var, size_t size>
	void apply(Refl::IReflectionVar& ref_var, Var (T::* /*arr*/)[size]) { ref_var.setOptional(true); }

	SHIB_REFLECTION_CLASS_DECLARE(OptionalAttribute);
};



// Not sure what I added this for. It's not used anywhere ...
class UniqueAttribute final : public Refl::IAttribute
{
public:
	Refl::IAttribute* clone(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(UniqueAttribute);
};



class RangeAttribute final : public Refl::IAttribute
{
public:
	RangeAttribute(double min = std::numeric_limits<double>::min(), double max = std::numeric_limits<double>::max(), double step = 1.0);

	double getStep(void) const;
	double getMin(void) const;
	double getMax(void) const;

	Refl::IAttribute* clone(void) const override;

private:
	double _step;
	double _min;
	double _max;

	SHIB_REFLECTION_CLASS_DECLARE(RangeAttribute);
};



class ScriptFlagsAttribute final : public Refl::IAttribute
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

	Refl::IAttribute* clone(void) const override;

	bool canInherit(void) const override { return !_flags.testAll(Flag::NoInherit); }
	bool isReferenceOnly(void) const { return _flags.testAll(Flag::ReferenceOnly); }
	bool canRegister(void) const { return !_flags.testAll(Flag::NoRegister); }

	Gaff::Flags<Flag> getFlags(void) const { return _flags; }

private:
	Gaff::Flags<Flag> _flags;

	SHIB_REFLECTION_CLASS_DECLARE(ScriptFlagsAttribute);
};

class BaseClassAttribute final : public Refl::IAttribute
{
public:
	Refl::IAttribute* clone(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(BaseClassAttribute);
};

// Template Attributes
////template <class T, class Msg>
////class GlobalMessageAttribute final : public Refl::IAttribute
////{
////public:
////	Refl::IAttribute* clone(void) const override
////	{
////		IAllocator& allocator = GetAllocator();
////		return SHIB_ALLOCT_POOL(GlobalMessageAttribute, allocator.getPoolIndex("Reflection"), allocator);
////	}
////
////	void instantiated(void* object, const Refl::IReflectionDefinition& ref_def) override
////	{
////		// $TODO: Implement
////
////		Broadcaster& broadcaster = GetApp().getBroadcaster();
////		T* const instance = ref_def.getInterface<T>(object);
////
////		const BroadcastID id = 
////			(_const_func) ?
////			broadcaster.listen(MemberFunc(instance, _const_func)) :
////			broadcaster.listen(MemberFunc(instance, _func));
////
////		s_removers.emplace(object, BroadcastRemover(id, broadcaster));
////		GetAllocator().addOnFreeCallback(OnFree, object);
////	}
////
////	void apply(void (T::*func)(const Msg&) const)
////	{
////		_const_func = func;
////	}
////
////	void apply(void (T::*func)(const Msg&))
////	{
////		_func = func;
////	}
////
////private:
////	void (T::*_const_func)(const Msg&) const = nullptr;
////	void (T::*_func)(const Msg&) = nullptr;
////
////	static VectorMap<void*, BroadcastRemover> s_removers;
////
////	static void OnFree(void* object)
////	{
////		s_removers.erase(object);
////	}
////
////	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(GlobalMessageAttribute, T, Msg);
////};

//SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE(GlobalMessageAttribute, T, Msg)
//
//template <class T, class Msg>
//VectorMap<void*, BroadcastRemover> GlobalMessageAttribute<T, Msg>::s_removers = VectorMap<void*, BroadcastRemover>();

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ReadOnlyAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::NoSerializeAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::OptionalAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::UniqueAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::RangeAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::ScriptFlagsAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::BaseClassAttribute)

//SHIB_TEMPLATE_REFLECTION_DECLARE(Shibboleth::GlobalMessageAttribute, T, Msg)
//
//SHIB_TEMPLATE_REFLECTION_DEFINE_BEGIN(Shibboleth::GlobalMessageAttribute, T, Msg)
//	.template BASE(Refl::IAttribute)
//SHIB_TEMPLATE_REFLECTION_DEFINE_END(Shibboleth::GlobalMessageAttribute, T, Msg)
