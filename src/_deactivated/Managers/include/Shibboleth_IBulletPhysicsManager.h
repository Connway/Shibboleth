/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include <Shibboleth_ReflectionDefinitions.h>

class btCollisionShape;
class btMotionState;
class btRigidBody;

NS_SHIBBOLETH

class Object;

class IBulletPhysicsManager
{
public:
	IBulletPhysicsManager(void) {}
	virtual ~IBulletPhysicsManager(void) {}

	virtual void clearMainWorld(void) = 0;
	//virtual void clearExtraWorld(size_t world) = 0;

	virtual btRigidBody* createRigidBody(Object* object, btCollisionShape* shape, float mass, btMotionState* motion_state = nullptr) = 0;

	// collision_group is what group we belong to, collision_mask is what groups we can collide with
	virtual void addToMainWorld(btRigidBody* body, short collision_group, short collision_mask) = 0;
	virtual void addToMainWorld(btRigidBody* body) = 0;
	virtual void removeFromMainWorld(btRigidBody* body) = 0;

	SHIB_INTERFACE_REFLECTION(IBulletPhysicsManager)
	SHIB_INTERFACE_NAME("Physics Manager")
};

NS_END
