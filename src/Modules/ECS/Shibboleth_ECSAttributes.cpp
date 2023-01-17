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

#include "Shibboleth_ECSAttributes.h"
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ECSClassAttribute)
	.template BASE(Refl::IAttribute)
SHIB_REFLECTION_DEFINE_END(Shibboleth::ECSClassAttribute)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ECSClassAttribute)

ECSClassAttribute::ECSClassAttribute(const char8_t* name, const char8_t* category):
	_category(category),
	_name(name)
{
}

const char8_t* ECSClassAttribute::getCategory(void) const
{
	return _category;
}

const char8_t* ECSClassAttribute::getName(void) const
{
	return _name ? _name : Refl::Reflection<ECSClassAttribute>::GetName();
}

Refl::IAttribute* ECSClassAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(ECSClassAttribute, allocator.getPoolIndex("Reflection"), allocator, _name, _category);
}

NS_END
