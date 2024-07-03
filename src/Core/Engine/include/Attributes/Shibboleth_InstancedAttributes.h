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

#pragma once

#include "Reflection/Shibboleth_Reflection.h"
#include "Reflection/Shibboleth_IAttribute.h"
#include "Shibboleth_String.h"

NS_SHIBBOLETH

class InstancedOptionalAttribute final : public Refl::IAttribute
{
public:
	enum class Flag
	{
		LeaveEmptyElements,
		StripPrefix,
		StripSuffix,

		Count
	};

	InstancedOptionalAttribute(const char8_t* prefix, const char8_t* suffix, Gaff::Flags<Flag> flags = Gaff::Flags<Flag>{ Flag::LeaveEmptyElements, Flag::StripPrefix, Flag::StripSuffix });
	explicit InstancedOptionalAttribute(const char8_t* prefix, Gaff::Flags<Flag> flags = Gaff::Flags<Flag>{ Flag::LeaveEmptyElements, Flag::StripPrefix, Flag::StripSuffix });
	InstancedOptionalAttribute(const InstancedOptionalAttribute& attr) = default;

	const char8_t* getPrefix(void) const;
	const char8_t* getSuffix(void) const;
	bool shouldLeaveEmptyElement(void) const;

	eastl::u8string_view stripPrefixAndSuffix(const char8_t* name) const;
	bool matches(const char8_t* name) const;

private:
	const char8_t* const _prefix = nullptr;
	const char8_t* const _suffix = nullptr;
	Gaff::Flags<Flag> _flags{ Flag::LeaveEmptyElements, Flag::StripPrefix, Flag::StripSuffix };

	SHIB_REFLECTION_ATTRIBUTE_DECLARE(InstancedOptionalAttribute);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::InstancedOptionalAttribute)
