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

#include "Shibboleth_EngineAttributesCommon.h"
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>

SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ReadOnlyAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::NoSerializeAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::UniqueAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::RangeAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::OptionalAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ScriptFlagsAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::BaseClassAttribute, IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::NoCopyAttribute, IAttribute)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ReadOnlyAttribute)
SHIB_REFLECTION_CLASS_DEFINE(NoSerializeAttribute)
SHIB_REFLECTION_CLASS_DEFINE(UniqueAttribute)
SHIB_REFLECTION_CLASS_DEFINE(RangeAttribute)
SHIB_REFLECTION_CLASS_DEFINE(OptionalAttribute)
SHIB_REFLECTION_CLASS_DEFINE(ScriptFlagsAttribute)
SHIB_REFLECTION_CLASS_DEFINE(BaseClassAttribute)
SHIB_REFLECTION_CLASS_DEFINE(NoCopyAttribute)



Refl::IAttribute* ReadOnlyAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ReadOnlyAttribute, allocator.getPoolIndex("Reflection"), allocator);
}



Refl::IAttribute* NoSerializeAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(NoSerializeAttribute, allocator.getPoolIndex("Reflection"), allocator);
}



Refl::IAttribute* OptionalAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(OptionalAttribute, allocator.getPoolIndex("Reflection"), allocator);
}



Refl::IAttribute* UniqueAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(UniqueAttribute, allocator.getPoolIndex("Reflection"), allocator);
}



RangeAttribute::RangeAttribute(double min, double max, double step):
	_step(step), _min(min), _max(max)
{
}

double RangeAttribute::getStep(void) const
{
	return _step;
}

double RangeAttribute::getMin(void) const
{
	return _min;
}

double RangeAttribute::getMax(void) const
{
	return _max;
}

Refl::IAttribute* RangeAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(RangeAttribute, allocator.getPoolIndex("Reflection"), allocator, _min, _max);
}



Refl::IAttribute* ScriptFlagsAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ScriptFlagsAttribute, allocator.getPoolIndex("Reflection"), allocator, _flags);
}



Refl::IAttribute* BaseClassAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(BaseClassAttribute, allocator.getPoolIndex("Reflection"), allocator);
}



Refl::IAttribute* NoCopyAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(NoCopyAttribute, allocator.getPoolIndex("Reflection"), allocator);
}

NS_END
