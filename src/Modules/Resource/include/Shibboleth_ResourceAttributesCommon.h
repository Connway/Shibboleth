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

#pragma once

#include <Reflection/Shibboleth_Reflection.h>
#include <Gaff_HashString.h>

NS_SHIBBOLETH

class CreatableAttribute final : public Refl::IAttribute
{
public:
	Refl::IAttribute* clone(void) const override;

	SHIB_REFLECTION_CLASS_DECLARE(CreatableAttribute);
};

class ResourceExtensionAttribute final : public Refl::IAttribute
{
public:
	ResourceExtensionAttribute(const char8_t* extension);

	const HashStringView32<>& getExtension(void) const;

	Refl::IAttribute* clone(void) const override;

private:
	HashStringView32<> _extension;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceExtensionAttribute);
};

class ResourceLoadPoolAttribute final : public Refl::IAttribute
{
public:
	ResourceLoadPoolAttribute(Gaff::Hash32 pool);

	Gaff::Hash32 getPool(void) const;

	Refl::IAttribute* clone(void) const override;

private:
	Gaff::Hash32 _pool;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceLoadPoolAttribute);
};

class ResourceSchemaAttribute final : public Refl::IAttribute
{
public:
	ResourceSchemaAttribute(const char8_t* schema);

	const char8_t* getSchema(void) const;

	Refl::IAttribute* clone(void) const override;

private:
	const char8_t* const _schema = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(ResourceSchemaAttribute);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::CreatableAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::ResourceExtensionAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::ResourceLoadPoolAttribute)
SHIB_REFLECTION_DECLARE(Shibboleth::ResourceSchemaAttribute)
