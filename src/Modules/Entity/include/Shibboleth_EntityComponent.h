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

#include "Shibboleth_EntityUpdater.h"
#include <Reflection/Shibboleth_Reflection.h>
#include <Gaff_Flags.h>

NS_SHIBBOLETH

class Entity;

enum class EntityComponentFlag
{
	Count
};


class EntityComponent : public Refl::IReflectionObject
{
public:
	EntityComponent(void) = default;

	virtual bool init(void);
	virtual void destroy(void);

	virtual bool clone(EntityComponent& new_component, const ISerializeReader* overrides = nullptr) const;
	bool clone(EntityComponent*& new_component, const ISerializeReader* overrides = nullptr) const;

	Entity* getOwner(void) const;

	const HashString64<>& getName(void) const;
	void setName(const HashString64<>& name);
	void setName(HashString64<>&& name);
	void setName(const HashStringView64<>& name);
	void setName(const U8String& name);

private:
	EntityUpdater _updater;

	Entity* _owner = nullptr;

	HashString64<> _name;

	Gaff::Flags<EntityComponentFlag> _flags;

	friend class EntityManager;
	friend class Entity;

	SHIB_REFLECTION_CLASS_DECLARE(EntityComponent);
	GAFF_NO_COPY(EntityComponent);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntityComponentFlag)
SHIB_REFLECTION_DECLARE(Shibboleth::EntityComponent)

#ifdef SHIB_REFL_IMPL
	SHIB_REFLECTION_BUILD_BEGIN(Shibboleth::EntityComponent)
		.template ctor<>()

		.var("flags", &Type::_flags)
		.var("name", &Type::_name)
	SHIB_REFLECTION_BUILD_END(Shibboleth::EntityComponent)
#endif
