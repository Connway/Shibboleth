/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Attributes/Shibboleth_EngineAttributesCommon.h"
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>

SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ReadOnlyAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::NoSerializeAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::UniqueAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::RangeAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::OptionalAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ScriptFlagsAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::NoCopyAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ClassBucketAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::AttributeBucketAttribute, Refl::IAttribute)


NS_SHIBBOLETH

SHIB_REFLECTION_ATTRIBUTE_DEFINE(ReadOnlyAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(NoSerializeAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(UniqueAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(RangeAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(OptionalAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(ScriptFlagsAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(NoCopyAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(ClassBucketAttribute)
SHIB_REFLECTION_ATTRIBUTE_DEFINE(AttributeBucketAttribute)



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



bool ClassBucketAttribute::canInherit(void) const
{
	return false;
}

void ClassBucketAttribute::finish(Refl::IReflectionDefinition& ref_def)
{
	GetApp().getReflectionManager().registerTypeBucket(ref_def);
}



bool AttributeBucketAttribute::canInherit(void) const
{
	return false;
}

void AttributeBucketAttribute::finish(Refl::IReflectionDefinition& ref_def)
{
	GAFF_ASSERT(ref_def.hasInterface<Refl::IAttribute>());
	GetApp().getReflectionManager().registerAttributeBucket(ref_def);
}

NS_END
