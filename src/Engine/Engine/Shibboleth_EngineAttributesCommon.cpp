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

#include "Shibboleth_EngineAttributesCommon.h"
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>

SHIB_REFLECTION_DEFINE(ReadOnlyAttribute)
SHIB_REFLECTION_DEFINE(RangeAttribute)
SHIB_REFLECTION_DEFINE(HashStringAttribute)
SHIB_REFLECTION_DEFINE(OptionalAttribute)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_WITH_BASE_NO_INHERITANCE(ReadOnlyAttribute, Gaff::IAttribute)

Gaff::IAttribute* ReadOnlyAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ReadOnlyAttribute, allocator.getPoolIndex("Reflection"), allocator);
}


SHIB_REFLECTION_CLASS_DEFINE_WITH_BASE_NO_INHERITANCE(RangeAttribute, Gaff::IAttribute)

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

Gaff::IAttribute* RangeAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(RangeAttribute, allocator.getPoolIndex("Reflection"), allocator, _min, _max);
}


SHIB_REFLECTION_CLASS_DEFINE_WITH_BASE_NO_INHERITANCE(HashStringAttribute, Gaff::IAttribute)

Gaff::IAttribute* HashStringAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(HashStringAttribute, allocator.getPoolIndex("Reflection"), allocator);
}


SHIB_REFLECTION_CLASS_DEFINE_WITH_BASE_NO_INHERITANCE(OptionalAttribute, Gaff::IAttribute)

Gaff::IAttribute* OptionalAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(OptionalAttribute, allocator.getPoolIndex("Reflection"), allocator);
}

NS_END
