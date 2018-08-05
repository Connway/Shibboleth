/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include <Shibboleth_Broadcaster.h>
#include <Shibboleth_Reflection.h>

NS_SHIBBOLETH

class ReadOnlyAttribute final : public Gaff::IAttribute
{
public:
	IAttribute* clone(void) const override;

	template <class T, class Var>
	void apply(Gaff::IReflectionVar* ref_var, Var T::*) { ref_var->setReadOnly(true);  }
	template <class T, class Var, class Ret>
	void apply(Gaff::IReflectionVar* ref_var, Ret (T::*)(void) const, void (T::*)(Var)) { ref_var->setReadOnly(true); }

	SHIB_REFLECTION_CLASS_DECLARE(ReadOnlyAttribute);
};

class EditorAttribute final : public Gaff::IAttribute
{
public:
	IAttribute* clone(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(EditorAttribute);
};

template <class T, class Msg>
class GlobalMessageAttribute final : public Gaff::IAttribute
{
public:
	GlobalMessageAttribute(BroadcastRemover T::* remover):
		_remover(remover)
	{
	}

	IAttribute* clone(void) const override
	{
		IAllocator& allocator = GetAllocator();
		return SHIB_ALLOCT_POOL(GlobalMessageAttribute, allocator.getPoolIndex("Reflection"), allocator, _remover);
	}

	void instantiated(const Gaff::IReflectionDefinition*, void* object) override
	{
		Broadcaster& broadcaster = GetApp().getBroadcaster();
		T* const instance = reinterpret_cast<T*>(object);
		GAFF_REF(broadcaster); GAFF_REF(instance);

		if (_const_func) {
			const BroadcastID id = broadcaster.listen(MemberFunc(instance, _const_func));
			(instance->*_remover) = BroadcastRemover(id, broadcaster);
		} else if (_func) {
			const BroadcastID id = broadcaster.listen(MemberFunc(instance, _func));
			(instance->*_remover) = BroadcastRemover(id, broadcaster);
		}
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
	BroadcastRemover T::* _remover = nullptr;

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(GlobalMessageAttribute, T, Msg);
};

SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(GlobalMessageAttribute, T, Msg)
	.BASE(Gaff::IAttribute)
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END(GlobalMessageAttribute, T, Msg)

NS_END

SHIB_REFLECTION_DECLARE(ReadOnlyAttribute)
SHIB_REFLECTION_DECLARE(EditorAttribute)
SHIB_TEMPLATE_REFLECTION_DECLARE(GlobalMessageAttribute, T, Msg)
SHIB_TEMPLATE_REFLECTION_DEFINE(GlobalMessageAttribute, T, Msg)
