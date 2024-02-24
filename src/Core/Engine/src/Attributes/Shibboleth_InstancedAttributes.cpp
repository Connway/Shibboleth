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

#include "Attributes/Shibboleth_InstancedAttributes.h"

SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(Shibboleth::InstancedOptionalAttribute, Refl::IAttribute)

NS_SHIBBOLETH

SHIB_REFLECTION_ATTRIBUTE_DEFINE(InstancedOptionalAttribute)

InstancedOptionalAttribute::InstancedOptionalAttribute(const char8_t* prefix, const char8_t* suffix, bool leave_empty_element):
	_prefix(prefix), _suffix(suffix), _leave_empty_element(leave_empty_element)
{
}

const char8_t* InstancedOptionalAttribute::getPrefix(void) const
{
	return _prefix;
}

const char8_t* InstancedOptionalAttribute::getSuffix(void) const
{
	return _suffix;
}

bool InstancedOptionalAttribute::shouldLeaveEmptyElement(void) const
{
	return _leave_empty_element;
}

bool InstancedOptionalAttribute::matches(const char8_t* name) const
{
	return (!_prefix || Gaff::StartsWith(name, _prefix)) && (!_suffix || Gaff::EndsWith(name, _suffix));
}

NS_END
