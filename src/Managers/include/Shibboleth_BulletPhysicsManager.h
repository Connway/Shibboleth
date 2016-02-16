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
#include <Shibboleth_Map.h>

class btCollisionConfiguration;
class btBroadphaseInterface;
class btConstraintSolver;
class btDynamicsWorld;
class btDispatcher;
class btRigidBody;

NS_SHIBBOLETH

class Object;

class BulletPhysicsManager : public IManager, public IUpdateQuery
{
public:
	static const char* GetName(void) { return "Physics Manager"; }
	static void SetMemoryFunctions(void);

	BulletPhysicsManager(void);
	~BulletPhysicsManager(void);

	void* rawRequestInterface(Gaff::ReflectionHash class_id) const override;

	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	void update(double dt, void*);

	void addTestCapsule(Object* object);

private:
	btCollisionConfiguration* _config;
	btBroadphaseInterface* _broadphase;
	btConstraintSolver* _solver;
	btDispatcher* _dispatcher;
	btDynamicsWorld* _main_world;

	Map<Object*, btRigidBody*> _body_map; // temp

	ProxyAllocator _physics_allocator;

	SHIB_REF_DEF(BulletPhysicsManager);
};

NS_END

#endif
