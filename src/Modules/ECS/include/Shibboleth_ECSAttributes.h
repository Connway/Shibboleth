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
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>

NS_SHIBBOLETH

class ECSClassAttribute final : public Gaff::IAttribute
{
public:
	ECSClassAttribute(const char* name = nullptr, const char* category = nullptr);

	const char* getCategory(void) const;
	const char* getName(void) const;
	int32_t size(void) const;

	Gaff::IAttribute* clone(void) const override;

	void finish(const Gaff::IReflectionDefinition& ref_def) override;

private:
	const char* _category = nullptr;
	const char* _name = nullptr;
	int32_t _size = 0;

	SHIB_REFLECTION_CLASS_DECLARE(ECSClassAttribute);
};

class IECSVarAttribute : public Gaff::IAttribute
{
public:
	IECSVarAttribute(void) {}
	virtual ~IECSVarAttribute(void) {}

	virtual const Gaff::IReflectionDefinition& getType(void) const = 0;
};

template <class T>
class ECSVarAttribute final : public IECSVarAttribute
{
	static_assert(std::is_pod<T>::value, "ECSVarAttribute type T is not a POD type!");

public:
	Gaff::IAttribute* clone(void) const override
	{
		IAllocator& allocator = GetAllocator();
		return SHIB_ALLOCT_POOL(ECSVarAttribute<T>, allocator.getPoolIndex("Reflection"), allocator);
	}

	const Gaff::IReflectionDefinition& getType(void) const override
	{
		return Reflection<T>::GetReflectionDefinition();
	}

	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(ECSVarAttribute, T);
};


SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(ECSVarAttribute, T)
	.BASE(IECSVarAttribute)
	.BASE(Gaff::IAttribute)
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END(ECSVarAttribute, T)

NS_END

SHIB_REFLECTION_DECLARE(ECSClassAttribute)

SHIB_TEMPLATE_REFLECTION_DECLARE(ECSVarAttribute, T)
SHIB_TEMPLATE_REFLECTION_DEFINE(ECSVarAttribute, T)
