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

#include "Shibboleth_ResourceAttributesCommon.h"

SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::CreatableAttribute, Refl::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::ResExtAttribute, Refl::IAttribute)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(CreatableAttribute)
SHIB_REFLECTION_CLASS_DEFINE(ResExtAttribute)

Refl::IAttribute* CreatableAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(CreatableAttribute, allocator.getPoolIndex("Reflection"), allocator);
}



ResExtAttribute::ResExtAttribute(const char8_t* extension):
	_extension(extension, eastl::CharStrlen(extension))
{
}

const HashStringView32<>& ResExtAttribute::getExtension(void) const
{
	return _extension;
}

Refl::IAttribute* ResExtAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ResExtAttribute, allocator.getPoolIndex("Reflection"), allocator, _extension.getBuffer());
}

NS_END
