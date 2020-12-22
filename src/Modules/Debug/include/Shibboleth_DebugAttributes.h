/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include <Gaff_Flags.h>

NS_SHIBBOLETH

class DebugMenuClassAttribute final : public Gaff::IAttribute
{
	IAttribute* clone(void) const override;

	void instantiated(void* object, const Gaff::IReflectionDefinition& ref_def) override;

	SHIB_REFLECTION_CLASS_DECLARE(DebugMenuClassAttribute);
};

class DebugMenuItemAttribute final : public Gaff::IAttribute
{
public:
	DebugMenuItemAttribute(const char* path);

	const U8String& getPath(void) const;

	IAttribute* clone(void) const override;

	void finish(Gaff::IReflectionDefinition& ref_def) override;

private:
	U8String _path;

	SHIB_REFLECTION_CLASS_DECLARE(DebugMenuItemAttribute);
};

NS_END

SHIB_REFLECTION_DECLARE(DebugMenuClassAttribute)
SHIB_REFLECTION_DECLARE(DebugMenuItemAttribute)
