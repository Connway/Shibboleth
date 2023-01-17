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

NS_SHIBBOLETH

class Entity;

class EntityComponent : public IEntityUpdateable
{
	GAFF_NO_COPY(EntityComponent);

public:
	EntityComponent(void) = default;

	virtual bool init(void);
	virtual bool clone(EntityComponent*& new_component, const ISerializeReader* overrides);
	virtual void destroy(void);

	Entity* getOwner(void) const;

	void updateAfter(IEntityUpdateable& after);

	void setEnableUpdate(bool enabled);
	bool canUpdate(void) const;

	const U8String& getName(void) const;
	void setName(const U8String& name);
	void setName(U8String&& name);

private:
	enum class Flag
	{
		UpdateEnabled,

		Count
	};

	Entity* _owner = nullptr;

	U8String _name;

	Gaff::Flags<Flag> _flags;

	friend class EntityManager;
	friend class Entity;

	SHIB_REFLECTION_CLASS_DECLARE(EntityComponent);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntityComponent)
