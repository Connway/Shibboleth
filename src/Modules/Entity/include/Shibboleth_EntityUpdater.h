/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

#include "Shibboleth_EntityDefines.h"
#include <Containers/Shibboleth_Vector.h>
#include <Shibboleth_HashString.h>
#include <Gaff_EnumIterator.h>
#include <Gaff_Flags.h>

NS_SHIBBOLETH

// Modify on a per game basis as needed.
enum class EntityUpdatePhase
{
	PrePhysics,
	DuringPhysics,
	PostPhysics,

	Count
};


class EntityUpdater final
{
public:
	using UpdateFunction = eastl::function<void (float)>;

	~EntityUpdater(void);

	void updateAfter(EntityUpdater& updater);

	void setEnabled(bool value);
	bool isEnabled(void) const;

	// "register" is a reserved keyword, so using enable/disable.
	void disable(void);
	void enable(void);

	void setUpdateFunction(const UpdateFunction& function);
	void setUpdateFunction(UpdateFunction&& function);

	void setUpdatePhase(EntityUpdatePhase update_phase);
	EntityUpdatePhase getUpdatePhase(void) const;

private:
	UpdateFunction _update_function;

	Vector<EntityUpdater*> _update_after{ ENTITY_ALLOCATOR };
	Vector<EntityUpdater*> _depends_on_me{ ENTITY_ALLOCATOR };

	EntityUpdatePhase _update_phase = EntityUpdatePhase::PrePhysics;

	int32_t _node_id = -1; // For use by EntityManager only.

	void markDirty(void);

	friend class EntityManager;
};

NS_END

GAFF_DEFINE_ENUM_ITERATOR(Shibboleth::EntityUpdatePhase)
