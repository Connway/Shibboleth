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

#ifndef USE_PHYSX

#include "Shibboleth_BulletPhysicsManager.h"
#include <Shibboleth_Object.h>
#include <Gaff_Utils.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable: 4127 4456)
#endif

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>
#include <BulletCollision/CollisionShapes/btTriangleShape.h>
#include <BulletCollision/CollisionShapes/btConvex2dShape.h>
#include <BulletCollision/CollisionShapes/btBox2dShape.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif


NS_SHIBBOLETH

class MotionState : public btMotionState
{
public:
	MotionState(Object* object):
		_object(object)
	{
	}

	void getWorldTransform(btTransform& worldTrans) const override
	{
		const auto& tform = _object->getWorldTransform();
		const auto& t = tform.getTranslation();
		const auto& r = tform.getRotation();

		worldTrans.setOrigin(btVector3(t[0], t[1], t[2]));
		worldTrans.setRotation(btQuaternion(r[0], r[1], r[2], r[3]));
	}

	//Bullet only calls the update of worldtransform for active objects
	void setWorldTransform(const btTransform& worldTrans) override
	{
		Gleam::TransformCPU conv_tform;

		const auto& rot = worldTrans.getRotation();
		const auto& pos = worldTrans.getOrigin();

		conv_tform.setTranslation(Gleam::Vector4CPU(pos.x(), pos.y(), pos.z(), 1.0f));
		conv_tform.setRotation(Gleam::QuaternionCPU(rot.x(), rot.y(), rot.z(), rot.w()));

		_object->setWorldTransform(conv_tform);
	}

private:
	Object* _object;
};

REF_IMPL_REQ(BulletPhysicsManager);
SHIB_REF_IMPL(BulletPhysicsManager)
.addBaseClassInterfaceOnly<BulletPhysicsManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

static size_t g_memory_pool = SIZE_T_FAIL;

static void* PhysicsAllocate(size_t size)
{
	return ShibbolethAllocate(size, g_memory_pool);
}

const char* BulletPhysicsManager::GetFriendlyName(void)
{
	return "Physics Manager";
}

void BulletPhysicsManager::SetMemoryFunctions(void)
{
	if (g_memory_pool == SIZE_T_FAIL) {
		g_memory_pool = GetPoolIndex("Physics");
	}

	btAlignedAllocSetCustom(&PhysicsAllocate, &ShibbolethFree);
}

BulletPhysicsManager::BulletPhysicsManager(void):
	_config(nullptr), _broadphase(nullptr),
	_solver(nullptr), _dispatcher(nullptr),
	_main_world(nullptr), _physics_allocator("Physics")
{
}

BulletPhysicsManager::~BulletPhysicsManager(void)
{
	// clearExtraWorlds();
	clearMainWorld();

	IAllocator* allocator = GetAllocator();

	// free extra worlds

	SHIB_FREET(_main_world, *allocator);
	SHIB_FREET(_solver, *allocator);
	SHIB_FREET(_broadphase, *allocator);
	SHIB_FREET(_dispatcher, *allocator);
	SHIB_FREET(_config, *allocator);
}

const char* BulletPhysicsManager::getName() const
{
	return GetFriendlyName();
}

void BulletPhysicsManager::allManagersCreated(void)
{
	_config = SHIB_ALLOCT(btDefaultCollisionConfiguration, _physics_allocator);
	_dispatcher = SHIB_ALLOCT(btCollisionDispatcher, _physics_allocator, _config);
	_broadphase = SHIB_ALLOCT(btDbvtBroadphase, _physics_allocator);
	_solver = SHIB_ALLOCT(btSequentialImpulseConstraintSolver, _physics_allocator);

	_main_world = SHIB_ALLOCT(btDiscreteDynamicsWorld, _physics_allocator, _dispatcher, _broadphase, _solver, _config);
	_main_world->setGravity(btVector3(0.0f, -9.81f, 0.0f));
}

void BulletPhysicsManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(AString("Physics Manager: Update"), Gaff::Bind(this, &BulletPhysicsManager::update));
}

void BulletPhysicsManager::update(double dt, void*)
{
	_main_world->stepSimulation(static_cast<btScalar>(dt));
}

void BulletPhysicsManager::clearMainWorld(void)
{
	clearWorld(_main_world);
}

//void clearExtraWorld(size_t world);

btRigidBody* BulletPhysicsManager::createRigidBody(Object* object, btCollisionShape* shape, float mass, btMotionState* motion_state)
{
	// RigidBody is dynamic if and only if mass is non zero, otherwise static
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	shape->calculateLocalInertia(mass, localInertia);

	if (object && !motion_state) {
		motion_state = SHIB_ALLOCT(MotionState, _physics_allocator, object);
	}

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, shape, localInertia);
	btRigidBody* body = SHIB_ALLOCT(btRigidBody, _physics_allocator, rbInfo);

	return body;
}

void BulletPhysicsManager::addToMainWorld(btRigidBody* body, short collision_group, short collision_mask)
{
	_main_world->addRigidBody(body, collision_group, collision_mask);
}

void BulletPhysicsManager::addToMainWorld(btRigidBody* body)
{
	_main_world->addRigidBody(body);
}

void BulletPhysicsManager::removeFromMainWorld(btRigidBody* body)
{
	_main_world->removeRigidBody(body);
}

void BulletPhysicsManager::clearWorld(btDynamicsWorld* world)
{
	auto& coll_objects = world->getCollisionObjectArray();
	IAllocator* allocator = GetAllocator();

	for (int i = world->getNumConstraints() - 1; i >= 0; --i) {
		btTypedConstraint* constraint = world->getConstraint(i);
		world->removeConstraint(constraint);
		SHIB_FREET(constraint, *allocator);
	}

	for (int i = world->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject* object = coll_objects[i];
		btRigidBody* rb = btRigidBody::upcast(object);

		if (rb) {
			btMotionState* motion_state = rb->getMotionState();

			if (motion_state) {
				SHIB_FREET(motion_state, *allocator);
			}

			world->removeRigidBody(rb);

		} else {
			world->removeCollisionObject(object);
		}

		SHIB_FREET(object, *allocator);
	}
}

NS_END

STATIC_FILE_FUNC
{
	Shibboleth::BulletPhysicsManager::SetMemoryFunctions();
}

#endif
