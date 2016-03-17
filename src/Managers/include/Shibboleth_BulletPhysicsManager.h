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

	static const char* GetName(void) { return "Physics Manager"; }
	static void SetMemoryFunctions(void);

	BulletPhysicsManager(void);
	~BulletPhysicsManager(void);

	void* rawRequestInterface(Gaff::ReflectionHash class_id) const override;

	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	void update(double dt, void*);

	INLINE void clearMainWorld(void);
	//INLINE void clearExtraWorld(size_t world);

	btCollisionShape* createCollisionShapeCapsule(float radius, float height);
	btCollisionShape* createCollisionShapeBox(float extent_x, float extent_y, float extent_z);
	INLINE btCollisionShape* createCollisionShapeBox(const Gleam::Vector4CPU& extents);
	INLINE btCollisionShape* createCollisionShapeBox(float extent);
	btCollisionShape* createCollisionShapeBox2D(float extent_x, float extent_y);
	INLINE btCollisionShape* createCollisionShapeBox2D(float extent);
	btCollisionShape* createCollisionShapeCone(float radius, float height);
	btCollisionShape* createCollisionShapeSphere(float radius);
	btCollisionShape* createCollisionShapeCylinder(float extent_x, float extent_y, float extent_z);
	INLINE btCollisionShape* createCollisionShapeCylinder(const Gleam::Vector4CPU& extents);
	INLINE btCollisionShape* createCollisionShapeCylinder(float extent);
	btCollisionShape* createCollisionShapeStaticPlane(float nx, float ny, float nz, float distance);
	INLINE btCollisionShape* createCollisionShapeStaticPlane(const Gleam::Vector4CPU& norm_dist);
	btCollisionShape* createCollisionShapeConvexHull(float* points, size_t num_points, size_t stride);
	//btCollisionShape* createCollisionShapeMultiSphere(size_t identifier = SIZE_T_FAIL);
	btCollisionShape* createCollisionShapeCompound(size_t identifier = SIZE_T_FAIL);
	btCollisionShape* createCollisionShapeConvex2D(btCollisionShape* shape);

	btCollisionShape* createCollisionShapeTriangle(
		float x1, float y1, float z1,
		float x2, float y2, float z2,
		float x3, float y3, float z3
	);
	INLINE btCollisionShape* createCollisionShapeTriangle(
		const Gleam::Vector4CPU& p1, const Gleam::Vector4CPU& p2,
		const Gleam::Vector4CPU& p3
	);

	btCollisionShape* createCollisionShapeHeightfield(
		size_t stick_width, size_t stick_length, const float* data,
		float height_scale, float min_height, float max_height,
		HeightfieldUpAxis up_axis, bool flip_quad_edges = false
	);

	// btBvhTriangleMeshShape?
	// btScaledBvhTriangleMeshShape?
	// btUniformScalingShape?

	void removeCollisionShape(btCollisionShape* shape);
	void clearCollisionShapes(void);

	btRigidBody* createRigidBody(Object* object, btCollisionShape* shape, float mass, btMotionState* motion_state = nullptr);

	// collision_group is what group we belong to, collision_mask is what groups we can collide with
	INLINE void addToMainWorld(btRigidBody* body, short collision_group, short collision_mask);
	INLINE void addToMainWorld(btRigidBody* body);
	INLINE void removeFromMainWorld(btRigidBody* body);

private:
	btCollisionConfiguration* _config;
	btBroadphaseInterface* _broadphase;
	btConstraintSolver* _solver;
	btDispatcher* _dispatcher;
	btDynamicsWorld* _main_world;

	ProxyAllocator _physics_allocator;

	Map<uint32_t, btCollisionShape*> _shapes;

	void clearWorld(btDynamicsWorld* world);

	SHIB_REF_DEF(BulletPhysicsManager);
};

NS_END

#endif
