/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include <Shibboleth_IResource.h>

namespace physx
{
	class PxMaterial;
}

NS_SHIBBOLETH

class PhysicsMaterialResource final : public IResource
{
public:
	~PhysicsMaterialResource(void);

	const physx::PxMaterial* getMaterial(void) const;
	physx::PxMaterial* getMaterial(void);

private:
	void loadMaterial(IFile* file, uintptr_t thread_id_int);

	physx::PxMaterial* _material = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(PhysicsMaterialResource);
};

using PhysicsMaterialResourcePtr = Gaff::RefPtr<PhysicsMaterialResource>;

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::PhysicsMaterialResource)
