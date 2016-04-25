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

#ifndef USE_PHYSX

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>
#include <Gaff_SpinLock.h>

class btCollisionConfiguration;
class btBroadphaseInterface;
class btConstraintSolver;
class btCollisionShape;
class btDynamicsWorld;
class btMotionState;
class btDispatcher;
class btRigidBody;
class btVector3;

namespace Gleam
{
	class Vector4CPU;
}

NS_SHIBBOLETH

class Object;

class BulletPhysicsManager : public IManager, public IUpdateQuery
{
public:
	enum HeightfieldUpAxis
	{
		UP_X_AXIS = 0,
		UP_Y_AXIS,
		UP_Z_AXIS,
	};

	static const char* GetFriendlyName(void);
	static void SetMemoryFunctions(void);

	BulletPhysicsManager(void);
	~BulletPhysicsManager(void);

	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	void update(double dt, void*);

	INLINE void clearMainWorld(void);
	//INLINE void clearExtraWorld(size_t world);

	btRigidBody* createRigidBody(Object* object, btCollisionShape* shape, float mass, btMotionState* motion_state = nullptr);

	// collision_group is what group we belong to, collision_mask is what groups we can collide with
	INLINE void addToMainWorld(btRigidBody* body, short collision_group, short collision_mask);
	INLINE void addToMainWorld(btRigidBody* body);
	INLINE void removeFromMainWorld(btRigidBody* body);

private:
	struct AddBodyData
	{
		AddBodyData(btRigidBody* b, short cg, short cm, bool ob):
			body(b), collision_group(cg), collision_mask(cm), only_body(ob)
		{
		}

		btRigidBody* body;
		short collision_group;
		short collision_mask;
		bool only_body;
	};

	btDynamicsWorld* _main_world;

	Array<AddBodyData> _new_bodies;
	Gaff::SpinLock _new_bodies_lock;

	btCollisionConfiguration* _config;
	btBroadphaseInterface* _broadphase;
	btConstraintSolver* _solver;
	btDispatcher* _dispatcher;

	ProxyAllocator _physics_allocator;

	void clearWorld(btDynamicsWorld* world);

	SHIB_REF_DEF(BulletPhysicsManager);
	REF_DEF_REQ;
};

NS_END

#endif
