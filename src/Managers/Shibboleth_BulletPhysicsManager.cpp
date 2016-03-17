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
	clearCollisionShapes();

	IAllocator* allocator = GetAllocator();

	// free extra worlds

	allocator->freeT(_main_world);
	allocator->freeT(_solver);
	allocator->freeT(_broadphase);
	allocator->freeT(_dispatcher);
	allocator->freeT(_config);
}

const char* BulletPhysicsManager::getName() const
{
	return GetName();
}

void BulletPhysicsManager::allManagersCreated(void)
{
	_config = _physics_allocator.template allocT<btDefaultCollisionConfiguration>();
	_dispatcher = _physics_allocator.template allocT<btCollisionDispatcher>(_config);
	_broadphase = _physics_allocator.template allocT<btDbvtBroadphase>();
	_solver = _physics_allocator.template allocT<btSequentialImpulseConstraintSolver>();

	_main_world = _physics_allocator.template allocT<btDiscreteDynamicsWorld>(_dispatcher, _broadphase, _solver, _config);
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

btCollisionShape* BulletPhysicsManager::createCollisionShapeCapsule(float radius, float height)
{
	uint32_t hash = Gaff::FNV1aHash32V(&radius, &height);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == CAPSULE_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btCapsuleShape>(radius, height);
	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeBox(float extent_x, float extent_y, float extent_z)
{
	uint32_t hash = Gaff::FNV1aHash32V(&extent_x, &extent_y, &extent_z);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == BOX_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btBoxShape>(btVector3(extent_x, extent_y, extent_z));
	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeBox(const Gleam::Vector4CPU& extents)
{
	return createCollisionShapeBox(extents[0], extents[1], extents[2]);
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeBox(float extent)
{
	return createCollisionShapeBox(extent, extent, extent);
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeBox2D(float extent_x, float extent_y)
{
	uint32_t hash = Gaff::FNV1aHash32V(&extent_x, &extent_y);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == BOX_2D_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btBox2dShape>(btVector3(extent_x, extent_y, 0.0f));
	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeBox2D(float extent)
{
	return createCollisionShapeBox2D(extent, extent);
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeCone(float radius, float height)
{
	uint32_t hash = Gaff::FNV1aHash32V(&radius, &height);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == CONE_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btConeShape>(radius, height);
	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeSphere(float radius)
{
	uint32_t hash = Gaff::FNV1aHash32T(&radius);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == SPHERE_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btSphereShape>(radius);
	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeCylinder(float extent_x, float extent_y, float extent_z)
{
	uint32_t hash = Gaff::FNV1aHash32V(&extent_x, &extent_y, &extent_z);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == CYLINDER_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btCylinderShape>(btVector3(extent_x, extent_y, extent_z));
	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeCylinder(const Gleam::Vector4CPU& extents)
{
	return createCollisionShapeCylinder(extents[0], extents[1], extents[2]);
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeCylinder(float extent)
{
	return createCollisionShapeCylinder(extent, extent, extent);
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeStaticPlane(float nx, float ny, float nz, float distance)
{
	uint32_t hash = Gaff::FNV1aHash32V(&nx, &ny, &nz, &distance);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == STATIC_PLANE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btStaticPlaneShape>(btVector3(nx, ny, nz), distance);
	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeStaticPlane(const Gleam::Vector4CPU& norm_dist)
{
	return createCollisionShapeStaticPlane(norm_dist[0], norm_dist[1], norm_dist[2], norm_dist[3]);
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeConvexHull(float* points, size_t num_points, size_t stride)
{
	uint32_t hash = Gaff::FNV1aHash32V(&points, &num_points, &stride);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == CONVEX_HULL_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btConvexHullShape>(
		points, static_cast<int>(num_points), static_cast<int>(stride)
	);

	_shapes.emplace(hash, shape);

	return shape;
}

//btCollisionShape* BulletPhysicsManager::createCollisionShapeMultiSphere(size_t identifier)
//{
//	if (identifier != SIZE_T_FAIL) {
//		return _physics_allocator.template allocT<btMultiSphereShape>();
//	}
//
//	uint32_t hash = Gaff::FNV1aHash32V(&identifier);
//
//	auto it = _shapes.findElementWithKey(hash);
//
//	if (it != _shapes.end()) {
//		GAFF_ASSERT(it->second->getShapeType() == MULTI_SPHERE_SHAPE_PROXYTYPE);
//		return it->second;
//	}
//
//	btCollisionShape* shape = _physics_allocator.template allocT<btCompoundShape>();
//
//	_shapes.emplace(hash, shape);
//
//	return shape;
//}

btCollisionShape* BulletPhysicsManager::createCollisionShapeCompound(size_t identifier)
{
	if (identifier == SIZE_T_FAIL) {
		return _physics_allocator.template allocT<btCompoundShape>();
	}

	uint32_t hash = Gaff::FNV1aHash32V(&identifier);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == COMPOUND_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btCompoundShape>();

	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeTriangle(
	float x1, float y1, float z1,
	float x2, float y2, float z2,
	float x3, float y3, float z3)
{
	uint32_t hash = Gaff::FNV1aHash32V(&x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == TRIANGLE_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btTriangleShape>(
		btVector3(x1, y1, z1), btVector3(x2, y2, z2), btVector3(x3, y3, z3)
	);

	_shapes.emplace(hash, shape);

	return shape;
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeTriangle(
	const Gleam::Vector4CPU& p1, const Gleam::Vector4CPU& p2,
	const Gleam::Vector4CPU& p3)
{
	return createCollisionShapeTriangle(
		p1[0], p1[1], p1[2],
		p2[0], p2[1], p2[2],
		p3[0], p3[1], p3[2]
	);
}

btCollisionShape* BulletPhysicsManager::createCollisionShapeHeightfield(
	size_t stick_width, size_t stick_length, const float* data,
	float height_scale, float min_height, float max_height,
	HeightfieldUpAxis up_axis, bool flip_quad_edges)
{
	uint32_t hash = Gaff::FNV1aHash32V(
		&stick_width, &stick_length, &data,
		&height_scale, &min_height, &max_height,
		&up_axis, &flip_quad_edges
	);

	auto it = _shapes.findElementWithKey(hash);

	if (it != _shapes.end()) {
		GAFF_ASSERT(it->second->getShapeType() == TERRAIN_SHAPE_PROXYTYPE);
		return it->second;
	}

	btCollisionShape* shape = _physics_allocator.template allocT<btHeightfieldTerrainShape>(
		static_cast<int>(stick_width), static_cast<int>(stick_length), data,
		height_scale, min_height, max_height, up_axis, PHY_FLOAT, flip_quad_edges
	);

	_shapes.emplace(hash, shape);

	return shape;
}

void BulletPhysicsManager::removeCollisionShape(btCollisionShape* shape)
{
	auto it = _shapes.findElementWithValue(shape);

	if (it != _shapes.end()) {
		_shapes.erase(it);
	}
}

void BulletPhysicsManager::clearCollisionShapes(void)
{
	IAllocator* allocator = GetAllocator();

	for (auto it = _shapes.begin(); it != _shapes.end(); ++it) {
		allocator->freeT(it->second);
	}

	_shapes.clear();
}

btRigidBody* BulletPhysicsManager::createRigidBody(Object* object, btCollisionShape* shape, float mass, btMotionState* motion_state)
{
	// RigidBody is dynamic if and only if mass is non zero, otherwise static
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	shape->calculateLocalInertia(mass, localInertia);

	if (object && !motion_state) {
		motion_state = _physics_allocator.template allocT<MotionState>(object);
	}

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motion_state, shape, localInertia);
	btRigidBody* body = _physics_allocator.template allocT<btRigidBody>(rbInfo);

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
		allocator->freeT(constraint);
	}

	for (int i = world->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject* object = coll_objects[i];
		btRigidBody* rb = btRigidBody::upcast(object);

		if (rb) {
			btMotionState* motion_state = rb->getMotionState();

			if (motion_state) {
				allocator->freeT(motion_state);
			}

			world->removeRigidBody(rb);

		} else {
			world->removeCollisionObject(object);
		}

		allocator->freeT(object);
	}
}

NS_END

STATIC_FILE_FUNC
{
	Shibboleth::BulletPhysicsManager::SetMemoryFunctions();
}

#endif
