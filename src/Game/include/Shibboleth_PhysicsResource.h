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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Gaff_IVirtualDestructor.h>

class btCollisionShape;

NS_SHIBBOLETH

enum PhysicsShapeType
{
	PST_CAPSULE = 0,
	PST_BOX,
	PST_BOX_2D,
	PST_CONE,
	PST_SPHERE,
	PST_CYLINDER,
	PST_STATIC_PLANE,
	//PST_CONVEX_HULL,
	//PST_MULTI_SPHERE,
	//PST_COMPOUND,
	//PST_CONVEX_2D
	PST_TRIANGLE,
	//PST_HEIGHTFIELD,
	PST_COUNT
};

SHIB_ENUM_REF_DEF(PhysicsShapeType);

#ifdef USE_PHYSX
#else

class BulletPhysicsResource : public Gaff::IVirtualDestructor
{
public:
	BulletPhysicsResource(void);
	~BulletPhysicsResource(void);

	btCollisionShape* collision_shape = nullptr;
};
#endif

NS_END
