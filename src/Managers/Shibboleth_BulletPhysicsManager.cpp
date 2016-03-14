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
	#pragma warning(disable: 4127)
#endif

#include <btBulletDynamicsCommon.h>

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
}

const char* BulletPhysicsManager::getName() const
{
	return GetName();
}

void BulletPhysicsManager::allManagersCreated(void)
{
	// create main physics world
	_config = _physics_allocator.template allocT<btDefaultCollisionConfiguration>();
	_dispatcher = _physics_allocator.template allocT<btCollisionDispatcher>(_config);
	_broadphase = _physics_allocator.template allocT<btDbvtBroadphase>();
	_solver = _physics_allocator.template allocT<btSequentialImpulseConstraintSolver>();

	_main_world = _physics_allocator.template allocT<btDiscreteDynamicsWorld>(_dispatcher, _broadphase, _solver, _config);
	_main_world->setGravity(btVector3(0.0f, -9.81f, 0.0f));

	// Add a test shape for the ground (remove later!)
	{
		btCollisionShape* groundShape = _physics_allocator.template allocT<btBoxShape>(btVector3(btScalar(50.0f), btScalar(1.0f), btScalar(50.0f)));

		//keep track of the shapes, we release memory at exit.
		//make sure to re-use collision shapes among rigid bodies whenever possible!
		//btAlignedObjectArray<btCollisionShape*> collisionShapes;

		//collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0.0f, -15.0f, 0.0f));

		btScalar mass(0.0f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		btVector3 localInertia(btVector3(0.0f, 0.0f, 0.0f));

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = _physics_allocator.template allocT<btDefaultMotionState>(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = _physics_allocator.template allocT<btRigidBody>(rbInfo);

		//add the body to the dynamics world
		_main_world->addRigidBody(body);
	}
}

void BulletPhysicsManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(AString("Physics Manager: Update"), Gaff::Bind(this, &BulletPhysicsManager::update));
}

void BulletPhysicsManager::update(double dt, void*)
{
	_main_world->stepSimulation(static_cast<btScalar>(dt));
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeCapsule(float radius, float height)
{
	uint32_t hash = Gaff::FNV1aHash32V(&radius, &height);

	auto it = _capsule_shapes.findElementWithKey(hash);

	if (it != _capsule_shapes.end()) {
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btCapsuleShape>(radius, height);
	_capsule_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeBox(float extent_x, float extent_y, float extent_z)
{
	uint32_t hash = Gaff::FNV1aHash32V(&extent_x, &extent_y, &extent_z);

	auto it = _box_shapes.findElementWithKey(hash);

	if (it != _box_shapes.end()) {
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btBoxShape>(btVector3(extent_x, extent_y, extent_z));
	_box_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeBox(float extent)
{
	return createCollisionShapeBox(extent, extent, extent);
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeCone(float radius, float height)
{
	uint32_t hash = Gaff::FNV1aHash32V(&radius, &height);

	auto it = _cone_shapes.findElementWithKey(hash);

	if (it != _cone_shapes.end()) {
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btConeShape>(radius, height);
	_cone_shapes.emplace(hash, shape);

	return shape;
}

btRigidBody* BulletPhysicsManager::createRigidBody(Object* object, btCollisionShape* shape, float mass, btMotionState* motion_state)
{
	// RigidBody is dynamic if and only if mass is non zero, otherwise static
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	shape->calculateLocalInertia(mass, localInertia);

	if (!motion_state) {
		motion_state = _physics_allocator.template allocT<MotionState>(object);
	}

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, shape, localInertia);
	btRigidBody* body = _physics_allocator.template allocT<btRigidBody>(rbInfo);

	return body;
}

void BulletPhysicsManager::addToMainWorld(btRigidBody* body)
{
	_main_world->addRigidBody(body);
}

void BulletPhysicsManager::removeFromMainWorld(btRigidBody* body)
{
	_main_world->removeRigidBody(body);
}

NS_END

STATIC_FILE_FUNC
{
	Shibboleth::BulletPhysicsManager::SetMemoryFunctions();
}

#endif
