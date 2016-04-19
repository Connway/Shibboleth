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

#include "Shibboleth_PhysicsLoader.h"
#include "Shibboleth_PhysicsResource.h"
#include <Shibboleth_SchemaManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_JSON.h>

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

static ProxyAllocator g_physics_allocator("Physics");

using ShapeCreator = btCollisionShape* (*)(const Gaff::JSON&);

static btCollisionShape* CreateCollisionShapeCapsule(const Gaff::JSON& json)
{
	Gaff::JSON radius = json["Radius"];
	Gaff::JSON height = json["Height"];

	return SHIB_ALLOCT(
		btCapsuleShape, g_physics_allocator,
		static_cast<float>(radius.getNumber()),
		static_cast<float>(height.getNumber())
	);
}

static btCollisionShape* CreateCollisionShapeBox(const Gaff::JSON& json/*float extent_x, float extent_y, float extent_z*/)
{
	Gaff::JSON width = json["Width"];
	Gaff::JSON height = json["Height"];
	Gaff::JSON depth = json["Depth"];

	return SHIB_ALLOCT(
		btBoxShape, g_physics_allocator,
		btVector3(
			static_cast<float>(width.getNumber()) / 2.0f,
			static_cast<float>(height.getNumber()) / 2.0f,
			static_cast<float>(depth.getNumber()) / 2.0f
		)
	);
}

static btCollisionShape* CreateCollisionShapeBox2D(const Gaff::JSON& json)
{
	Gaff::JSON width = json["Width"];
	Gaff::JSON height = json["Height"];

	return SHIB_ALLOCT(
		btBox2dShape, g_physics_allocator,
		btVector3(
			static_cast<float>(width.getNumber()) / 2.0f,
			static_cast<float>(height.getNumber()) / 2.0f,
			0.0f
		)
	);
}

static btCollisionShape* CreateCollisionShapeCone(const Gaff::JSON& json)
{
	Gaff::JSON radius = json["Radius"];
	Gaff::JSON height = json["Height"];

	return SHIB_ALLOCT(
		btConeShape, g_physics_allocator,
		static_cast<float>(radius.getNumber()),
		static_cast<float>(height.getNumber())
	);
}

static btCollisionShape* CreateCollisionShapeSphere(const Gaff::JSON& json)
{
	Gaff::JSON radius = json["Radius"];
	return SHIB_ALLOCT(btSphereShape, g_physics_allocator, static_cast<float>(radius.getNumber()));
}

static btCollisionShape* CreateCollisionShapeCylinder(const Gaff::JSON& json/*float extent_x, float extent_y, float extent_z*/)
{
	Gaff::JSON width = json["Width"];
	Gaff::JSON height = json["Height"];
	Gaff::JSON depth = json["Depth"];

	return SHIB_ALLOCT(
		btCylinderShape, g_physics_allocator,
		btVector3(
			static_cast<float>(width.getNumber()) / 2.0f,
			static_cast<float>(height.getNumber()) / 2.0f,
			static_cast<float>(depth.getNumber()) / 2.0f
		)
	);
}

static btCollisionShape* CreateCollisionShapeStaticPlane(const Gaff::JSON& json/*float nx, float ny, float nz, float distance*/)
{	
	Gaff::JSON nx = json["Normal X"];
	Gaff::JSON ny = json["Normal Y"];
	Gaff::JSON nz = json["Normal Z"];
	Gaff::JSON dist = json["Distance"];

	return SHIB_ALLOCT(
		btStaticPlaneShape, g_physics_allocator,
		btVector3(
			static_cast<float>(nx.getNumber()),
			static_cast<float>(ny.getNumber()),
			static_cast<float>(nz.getNumber())
		),
		static_cast<float>(dist.getNumber())
	);
}

//static btCollisionShape* CreateCollisionShapeConvexHull(const Gaff::JSON& json/*float* points, size_t num_points, size_t stride*/)
//{
//
//}
//
////btCollisionShape* CreateCollisionShapeMultiSphere(const Gaff::JSON& json)
////{
////}
//
//static btCollisionShape* CreateCollisionShapeCompound(const Gaff::JSON& json)
//{
//
//}
//
//static btCollisionShape* CreateCollisionShapeConvex2D(const Gaff::JSON& json/*btCollisionShape* shape*/)
//{
//
//}

static btCollisionShape* CreateCollisionShapeTriangle(const Gaff::JSON& json)
{
	Gaff::JSON p1x = json["Point 1 X"];
	Gaff::JSON p1y = json["Point 1 Y"];
	Gaff::JSON p1z = json["Point 1 Z"];

	Gaff::JSON p2x = json["Point 2 X"];
	Gaff::JSON p2y = json["Point 2 Y"];
	Gaff::JSON p2z = json["Point 2 Z"];

	Gaff::JSON p3x = json["Point 3 X"];
	Gaff::JSON p3y = json["Point 3 Y"];
	Gaff::JSON p3z = json["Point 3 Z"];

	return SHIB_ALLOCT(
		btTriangleShape, g_physics_allocator,
		btVector3(
			static_cast<float>(p1x.getNumber()),
			static_cast<float>(p1y.getNumber()),
			static_cast<float>(p1z.getNumber())
		),
		btVector3(
			static_cast<float>(p2x.getNumber()),
			static_cast<float>(p2y.getNumber()),
			static_cast<float>(p2z.getNumber())
		),
		btVector3(
			static_cast<float>(p3x.getNumber()),
			static_cast<float>(p3y.getNumber()),
			static_cast<float>(p3z.getNumber())
		)
	);
}

//static btCollisionShape* CreateCollisionShapeHeightfield(const Gaff::JSON& json)
//{
//	
//}

// btBvhTriangleMeshShape?
// btScaledBvhTriangleMeshShape?
// btUniformScalingShape?

static ShapeCreator g_shape_creators[PST_COUNT] = {
	CreateCollisionShapeCapsule,
	CreateCollisionShapeBox,
	CreateCollisionShapeBox2D,
	CreateCollisionShapeCone,
	CreateCollisionShapeSphere,
	CreateCollisionShapeCylinder,
	CreateCollisionShapeStaticPlane,
	//CreateCollisionShapeConvexHull,
	//CreateCollisionShapeMultiSphere,
	//CreateCollisionShapeCompound,
	//CreateCollisionShapeConvex2D,
	CreateCollisionShapeTriangle//,
	//CreateCollisionShapeHeightfield
};

static const char* g_physics_schema_names[PST_COUNT] = {
	"PhysicsCapsule.schema",
	"PhysicsBox.schema",
	"PhysicsBox2D.schema",
	"PhysicsCone.schema",
	"PhysicsSphere.schema",
	"PhysicsCylinder.schema",
	"PhysicsStaticPlane.schema"//,
	//"PhysicsConvexHull.schema",
	//"PhysicsMultiSphere.schema",
	//"PhysicsCompound.schema",
	//"PhysicsConvex2D.schema"
};

static PhysicsShapeType ValidateSchema(const Gaff::JSON& shape_info, const char* file_name)
{
	SchemaManager& schema_mgr = GetApp().getManagerT<SchemaManager>();
	PhysicsShapeType shape = PST_COUNT;
		
	const char* shape_name = shape_info["Shape"].getString();

	if (!GetEnumRefDef<PhysicsShapeType>().getValue(shape_name, shape)) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - '%s' is not a value in the PhysicsShapeType enum.\n", shape_name);
		return PST_COUNT;
	}

	const Gaff::JSON& schema = schema_mgr.getSchema(g_physics_schema_names[shape]);

	if (shape_info.validate(schema)) {
		return shape;
	}

	GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Could not validate schema for '%s'.\n", file_name);
	return PST_COUNT;
}


PhysicsLoader::PhysicsLoader(void)
{
}

PhysicsLoader::~PhysicsLoader(void)
{
}

Gaff::IVirtualDestructor* PhysicsLoader::load(const char* file_name, uint64_t, HashMap<AString, IFile*>& file_map)
{
	GAFF_ASSERT(file_map.hasElementWithKey(AString(file_name)));
	IFile*& file = file_map[AString(file_name)];

	Gaff::JSON shape_info;
	bool success = shape_info.parse(file->getBuffer());

	GetApp().getFileSystem()->closeFile(file);
	file = nullptr;

	if (!success) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to parse file '%s'.\n", file_name);
		return nullptr;
	}

	PhysicsShapeType shape_type = ValidateSchema(shape_info, file_name);

	if (shape_type == PST_COUNT) {
		return nullptr;
	}

	BulletPhysicsResource* phys_resource = SHIB_ALLOCT(BulletPhysicsResource, g_physics_allocator);
	phys_resource->collision_shape = g_shape_creators[shape_type](shape_info);

	return phys_resource;
}

NS_END

#endif
