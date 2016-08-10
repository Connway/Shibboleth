/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_IBulletPhysicsComponent.h"
#include <Shibboleth_ResourceWrapper.h>
#include <Shibboleth_Component.h>
#include <LinearMath/btVector3.h>

class btRigidBody;

NS_SHIBBOLETH

class BulletPhysicsResource;

static const char* g_physics_schema_names[] = {
	"PhysicsCapsule.schema",
	"PhysicsBox.schema"
};

class BulletPhysicsComponent : public Component, public IBulletPhysicsComponent
{
public:
	BulletPhysicsComponent(void);
	~BulletPhysicsComponent(void);

	const Gaff::JSON& getSchema(void) const;

	bool load(const Gaff::JSON&) override;
	bool save(Gaff::JSON&) override;

	void addToWorld(void) override;
	void removeFromWorld(void) override;

	void setActive(bool active) override;

	const btRigidBody* getRigidBody(void) const override;
	btRigidBody* getRigidBody(void) override;

private:
	btRigidBody* _rigid_body;
	btVector3 _inertia;
	float _mass;

	ResourceWrapper<BulletPhysicsResource> _phys_res;

	void collisionShapeLoaded(ResourceContainer*);

	SHIB_REF_DEF(BulletPhysicsComponent);
	REF_DEF_REQ;
};

NS_END
