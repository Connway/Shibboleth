/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_PhysicsShapeResource.h"
#include "Shibboleth_PhysicsManager.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Math.h>
#include <PxPhysicsAPI.h>

SHIB_REFLECTION_DEFINE_BEGIN(PhysicsShapeResource)
	.classAttrs(
		ResExtAttribute(".physics_shape.bin"),
		ResExtAttribute(".physics_shape"),
		MakeLoadFileCallbackAttribute(&PhysicsShapeResource::loadShape)
	)

	.base<IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(PhysicsShapeResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(PhysicsShapeResource)

PhysicsShapeResource::~PhysicsShapeResource(void)
{
	SAFEGAFFRELEASE(_shape);
}

const physx::PxMaterial* PhysicsShapeResource::getMaterial(void) const
{
	return _material->getMaterial();
}

physx::PxMaterial* PhysicsShapeResource::getMaterial(void)
{
	return _material->getMaterial();
}

const physx::PxShape* PhysicsShapeResource::getShape(void) const
{
	return _shape;
}

physx::PxShape* PhysicsShapeResource::getShape(void)
{
	return _shape;
}

void PhysicsShapeResource::loadShape(IFile* file)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load physics material '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();

	if (!reader.isObject()) {
		// $TODO: Log error.
		failed();
		return;
	}

	{
		const auto guard = reader.enterElementGuard("material");
		
		if (!Reflection<PhysicsMaterialResourcePtr>::Load(reader, _material)) {
			// $TODO: Log error.
			failed();
			return;
		}
	}

	PhysicsManager& phys_mgr = GetApp().getManagerTFast<PhysicsManager>();

	LoadResult result = loadPlane(reader, phys_mgr);

	if (result == LoadResult::Skip) {
		// Try the next shape type.
		result = loadCapsule(reader, phys_mgr);
	}

	if (result == LoadResult::Skip) {
		// Try the next shape type.
	}

	if (_shape) {
		_shape->setLocalPose(physx::PxTransform(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f))));
		succeeded();

	} else {
		// $TODO: Log error.
		failed();
	}
}

PhysicsShapeResource::LoadResult PhysicsShapeResource::loadCapsule(const Gaff::ISerializeReader& reader, PhysicsManager& phys_mgr)
{
	const auto plane_guard = reader.enterElementGuard("capsule");

	if (reader.isNull()) {
		return LoadResult::Skip;
	}
	else if (!reader.isObject()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	GetApp().getManagerTFast<ResourceManager>().waitForResource(*_material);

	if (_material->hasFailed()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	const float half_height = reader.readFloat("half_height", 0.25f);
	const float radius = reader.readFloat("radius", 0.25f);

	// Do we need to lock here?
	_shape = phys_mgr.getPhysics()->createShape(physx::PxCapsuleGeometry(radius, half_height), *_material->getMaterial());

	if (!_shape) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	return LoadResult::Success;
}

PhysicsShapeResource::LoadResult PhysicsShapeResource::loadPlane(const Gaff::ISerializeReader& reader, PhysicsManager& phys_mgr)
{
	const auto plane_guard = reader.enterElementGuard("plane");

	if (reader.isNull()) {
		return LoadResult::Skip;
	}
	else if (!reader.isObject()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	GetApp().getManagerTFast<ResourceManager>().waitForResource(*_material);

	if (_material->hasFailed()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	// Do we need to lock here?
	_shape = phys_mgr.getPhysics()->createShape(physx::PxPlaneGeometry(), *_material->getMaterial(), false);

	if (!_shape) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	return LoadResult::Success;
}

NS_END
