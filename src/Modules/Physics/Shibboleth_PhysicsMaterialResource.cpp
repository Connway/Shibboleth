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

#include "Shibboleth_PhysicsMaterialResource.h"
#include "Shibboleth_PhysicsManager.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_LoadFileCallbackAttribute.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_LogManager.h>
#include <PxPhysicsAPI.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::PhysicsMaterialResource)
	.classAttrs(
		Shibboleth::ResExtAttribute(u8".physics_material.bin"),
		Shibboleth::ResExtAttribute(u8".physics_material"),
		Shibboleth::MakeLoadFileCallbackAttribute(&Shibboleth::PhysicsMaterialResource::loadMaterial)
	)

	.base<Shibboleth::IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::PhysicsMaterialResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(PhysicsMaterialResource)

PhysicsMaterialResource::~PhysicsMaterialResource(void)
{
	SAFEGAFFRELEASE(_material);
}

const physx::PxMaterial* PhysicsMaterialResource::getMaterial(void) const
{
	return _material;
}

physx::PxMaterial* PhysicsMaterialResource::getMaterial(void)
{
	return _material;
}

void PhysicsMaterialResource::loadMaterial(IFile* file, uintptr_t /*thread_id_int*/)
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

	const float static_friction = reader.readFloat(u8"static_friction", 0.5f);
	const float dynamic_friction = reader.readFloat(u8"dynamic_friction", 0.5f);
	const float restitution = reader.readFloat(u8"restitution", 0.5f);

	// Do we need to lock here?
	PhysicsManager& phys_mgr = GetApp().getManagerTFast<PhysicsManager>();
	_material = phys_mgr.getPhysics()->createMaterial(static_friction, dynamic_friction, restitution);

	if (_material) {
		succeeded();

	} else {
		// $TODO: Log error.
		failed();
	}
}

NS_END
