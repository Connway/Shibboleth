/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::PhysicsShapeResource)
	.classAttrs(
		Shibboleth::ResExtAttribute(u8".physics_shape.bin"),
		Shibboleth::ResExtAttribute(u8".physics_shape"),
		Shibboleth::MakeLoadFileCallbackAttribute(&Shibboleth::PhysicsShapeResource::loadShape)
	)

	.base<Shibboleth::IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::PhysicsShapeResource)

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

void PhysicsShapeResource::loadShape(IFile* file, uintptr_t /*thread_id_int*/)
{
	SerializeReaderWrapper readerWrapper;

	if (!OpenJSONOrMPackFile(readerWrapper, getFilePath().getBuffer(), file)) {
		LogErrorResource("Failed to load physics material '%s' with error: '%s'", getFilePath().getBuffer(), readerWrapper.getErrorText());
		failed();
		return;
	}

	const ISerializeReader& reader = *readerWrapper.getReader();

	if (!reader.isObject()) {
		// $TODO: Log error.
		failed();
		return;
	}

	{
		const auto guard = reader.enterElementGuard(u8"material");
		
		if (!Refl::Reflection<PhysicsMaterialResourcePtr>::GetInstance().load(reader, _material)) {
			// $TODO: Log error.
			failed();
			return;
		}
	}

	PhysicsManager& phys_mgr = GetApp().getManagerTFast<PhysicsManager>();

	using LoadPtr = PhysicsShapeResource::LoadResult (PhysicsShapeResource::*)(const ISerializeReader&, PhysicsManager& phys_mgr);

	constexpr LoadPtr k_load_funcs[] = {
		&PhysicsShapeResource::loadCapsule,
		&PhysicsShapeResource::loadSphere,
		&PhysicsShapeResource::loadPlane,
		&PhysicsShapeResource::loadBox,
	};

	for (LoadPtr load_func : k_load_funcs) {
		const LoadResult result = (this->*load_func)(reader, phys_mgr);

		if (result != LoadResult::Skip) {
			break;
		}
	}

	if (_shape) {
		_shape->setLocalPose(physx::PxTransform(physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f))));
		succeeded();

	} else {
		// $TODO: Log error.
		failed();
	}
}

PhysicsShapeResource::LoadResult PhysicsShapeResource::loadCapsule(const ISerializeReader& reader, PhysicsManager& phys_mgr)
{
	const auto capsule_guard = reader.enterElementGuard(u8"capsule");

	if (reader.isNull()) {
		return LoadResult::Skip;
	} else if (!reader.isObject()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	GetApp().getManagerTFast<ResourceManager>().waitForResource(*_material);

	if (_material->hasFailed()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	const float half_height = reader.readFloat(u8"half_height", 0.25f);
	const float radius = reader.readFloat(u8"radius", 0.25f);

	// Do we need to lock here?
	_shape = phys_mgr.getPhysics()->createShape(physx::PxCapsuleGeometry(radius, half_height), *_material->getMaterial());

	if (!_shape) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	return LoadResult::Success;
}

PhysicsShapeResource::LoadResult PhysicsShapeResource::loadSphere(const ISerializeReader& reader, PhysicsManager& phys_mgr)
{
	const auto plane_guard = reader.enterElementGuard(u8"sphere");

	if (reader.isNull()) {
		return LoadResult::Skip;
	} else if (!reader.isObject()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	GetApp().getManagerTFast<ResourceManager>().waitForResource(*_material);

	if (_material->hasFailed()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	const float radius = reader.readFloat(u8"radius", 0.5f);

	// Do we need to lock here?
	_shape = phys_mgr.getPhysics()->createShape(physx::PxSphereGeometry(radius), *_material->getMaterial(), false);

	if (!_shape) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	return LoadResult::Success;
}

PhysicsShapeResource::LoadResult PhysicsShapeResource::loadPlane(const ISerializeReader& reader, PhysicsManager& phys_mgr)
{
	const auto plane_guard = reader.enterElementGuard(u8"plane");

	if (reader.isNull()) {
		return LoadResult::Skip;
	} else if (!reader.isObject()) {
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

PhysicsShapeResource::LoadResult PhysicsShapeResource::loadBox(const ISerializeReader& reader, PhysicsManager& phys_mgr)
{
	const auto plane_guard = reader.enterElementGuard(u8"box");

	if (reader.isNull()) {
		return LoadResult::Skip;
	} else if (!reader.isObject()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	GetApp().getManagerTFast<ResourceManager>().waitForResource(*_material);

	if (_material->hasFailed()) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	float hx = 0.5f;
	float hy = 0.5f;
	float hz = 0.5f;

	{
		const auto size_guard = reader.enterElementGuard(u8"half_extents");

		if (reader.isObject()) {
			hx = reader.readFloat(u8"x", 0.5f);
			hy = reader.readFloat(u8"y", 0.5f);
			hz = reader.readFloat(u8"z", 0.5f);
		}
	}

	// Do we need to lock here?
	_shape = phys_mgr.getPhysics()->createShape(physx::PxBoxGeometry(hx, hy, hz), *_material->getMaterial(), false);

	if (!_shape) {
		// $TODO: Log error.
		return LoadResult::Error;
	}

	return LoadResult::Success;
}

NS_END
