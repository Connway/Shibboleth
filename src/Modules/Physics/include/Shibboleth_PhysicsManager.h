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

#pragma once

#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_ECSQuery.h>
#include <Shibboleth_ISystem.h>

namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxScene;
	class PxPvd;
}

NS_SHIBBOLETH

class ECSManager;
struct Time;

class PhysicsManager final : public IManager
{
public:
	~PhysicsManager(void);

	bool init(void) override;
	void update(void);

	physx::PxFoundation* getFoundation(void);
	physx::PxPhysics* getPhysics(void);

private:
	VectorMap<Gaff::Hash32, physx::PxScene*> _scenes{ ProxyAllocator("Physics") };

	ECSQuery::SharedOutput<Scene> _scene_comps{ ProxyAllocator("Physics") };
	ECSQuery::Output _rigid_bodies{ ProxyAllocator("Physics") };
	ECSQuery::Output _positions{ ProxyAllocator("Physics") };
	ECSQuery::Output _rotations{ ProxyAllocator("Physics") };
	ECSQuery::Output _scales{ ProxyAllocator("Physics") };

	physx::PxFoundation* _foundation = nullptr;
	physx::PxPhysics* _physics = nullptr;

	ECSManager* _ecs_mgr = nullptr;

	const Time* _game_time = nullptr;
	float _remaining_time = 0.0f;

	JobPool* _job_pool = nullptr;

#ifdef _DEBUG
	physx::PxPvd* _pvd = nullptr;
#endif

	SHIB_REFLECTION_CLASS_DECLARE(PhysicsManager);
};

class PhysicsSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(void) override;

private:
	PhysicsManager* _physics_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(PhysicsSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(PhysicsManager)
SHIB_REFLECTION_DECLARE(PhysicsSystem)
