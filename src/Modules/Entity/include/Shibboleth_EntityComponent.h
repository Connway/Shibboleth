/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_IEntityUpdateable.h"
#include <Shibboleth_Reflection.h>
#include <Gaff_Flags.h>
//#include <Shibboleth_ECSEntity.h>
//#include <Gaff_IncludeEASTLAtomic.h>
//#include <Gaff_RefPtr.h>
//#include <Gaff_Hash.h>

NS_SHIBBOLETH

class Entity;

using EntityComponentID = int32_t;
using EntityID = int32_t;
constexpr EntityComponentID EntityComponentID_None = -1;

// Negative IDs are not valid, as we use these as indices.
constexpr bool ValidEntityComponentID(EntityComponentID id)
{
	return id > EntityComponentID_None;
}


class EntityComponent : public IEntityUpdateable
{
	GAFF_NO_COPY(EntityComponent);

public:
	EntityComponent(void) = default;

	virtual bool init(void);
	virtual void destroy(void);

	Entity* getOwner(void) const;

	EntityComponentID getID(void) const;
	void setID(EntityComponentID id);

	void setEnableUpdate(bool enabled);
	bool canUpdate(void) const;

private:
	enum class Flag
	{
		UpdateEnabled,

		Count
	};

	Vector<EntityComponentID> _dependent_on_me_components;
	Vector<EntityID> _dependent_on_me_entities;
	Vector<EntityComponentID> _update_after_components;
	Vector<EntityID> _update_after_entities;

	Entity* _owner = nullptr;

	//EntityManager& _entity_mgr;
	EntityComponentID _id = EntityComponentID_None;

	Gaff::Flags<Flag> _flags;

	friend class EntityManager;
	friend class Entity;

	SHIB_REFLECTION_CLASS_DECLARE(EntityComponent);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntityComponent)
