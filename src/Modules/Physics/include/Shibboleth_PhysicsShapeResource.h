/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_PhysicsMaterialResource.h"

namespace physx
{
	class PxShape;
}

NS_SHIBBOLETH

class PhysicsManager;

class PhysicsShapeResource final : public IResource
{
public:
	~PhysicsShapeResource(void);

	const physx::PxMaterial* getMaterial(void) const;
	physx::PxMaterial* getMaterial(void);

	const physx::PxShape* getShape(void) const;
	physx::PxShape* getShape(void);

	void load(const ISerializeReader& reader, uintptr_t thread_id_int) override;

private:
	enum class LoadResult
	{
		Skip,
		Error,
		Success
	};

	PhysicsMaterialResourcePtr _material;
	physx::PxShape* _shape = nullptr;

	LoadResult loadCapsule(const ISerializeReader& reader, PhysicsManager& phys_mgr);
	LoadResult loadSphere(const ISerializeReader& reader, PhysicsManager& phys_mgr);
	LoadResult loadPlane(const ISerializeReader& reader, PhysicsManager& phys_mgr);
	LoadResult loadBox(const ISerializeReader& reader, PhysicsManager& phys_mgr);

	SHIB_REFLECTION_CLASS_DECLARE(PhysicsShapeResource);
};

using PhysicsShapeResourcePtr = Gaff::RefPtr<PhysicsShapeResource>;

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::PhysicsShapeResource)
