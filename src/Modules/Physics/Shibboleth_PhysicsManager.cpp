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

#include "Shibboleth_PhysicsManager.h"
#include <Shibboleth_GameTime.h>
#include <Shibboleth_JobPool.h>
#include <PxPhysicsAPI.h>

SHIB_REFLECTION_DEFINE_BEGIN(PhysicsManager)
	.base<IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(PhysicsManager)

SHIB_REFLECTION_DEFINE_WITH_CTOR_AND_BASE(PhysicsSystem, ISystem)


namespace
{
	class PhysicsAllocator final : public physx::PxAllocatorCallback
	{
	public:
		void* allocate(size_t size, const char* type_name, const char* filename, int line) override
		{
			GAFF_REF(type_name);
			return _allocator.alloc(size, 16, filename, line);
		}

		void deallocate(void* ptr) override
		{
			_allocator.free(ptr);
		}

	private:
		Shibboleth::ProxyAllocator _allocator{ "Physics" };
	};

	class PhysicsErrorHandler final : public physx::PxErrorCallback
	{
	public:
		void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			GAFF_REF(code, message, file, line);
		}
	};

	class PhysicsTaskDispatcher final : public physx::PxCpuDispatcher
	{
	public:
		void init(void)
		{
			_job_pool = &Shibboleth::GetApp().getJobPool();
		}

		void submitTask(physx::PxBaseTask& task) override
		{
			Gaff::JobData job_data{ RunTask, &task };
			_job_pool->addJobs(&job_data, 1);
		}

		uint32_t getWorkerCount() const override
		{
			return static_cast<uint32_t>(_job_pool->getNumTotalThreads());
		}

	private:
		Shibboleth::JobPool* _job_pool = nullptr;

		static void RunTask(void* data)
		{
			physx::PxBaseTask& task = *reinterpret_cast<physx::PxBaseTask*>(data);
			task.run();
			task.release();
		}
	};

	static PhysicsTaskDispatcher g_physics_task_dispatcher;
	static PhysicsErrorHandler g_physics_error_handler;
	static PhysicsAllocator g_physics_allocator;
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(PhysicsManager)
SHIB_REFLECTION_CLASS_DEFINE(PhysicsSystem)

PhysicsManager::~PhysicsManager(void)
{
	for (auto& pair : _scenes) {
		SAFEGAFFRELEASE(pair.second);
	}

	SAFEGAFFRELEASE(_physics);

#ifdef _DEBUG
	physx::PxPvdTransport* transport = _pvd->getTransport();
	SAFEGAFFRELEASE(transport);
	SAFEGAFFRELEASE(_pvd);
#endif

	SAFEGAFFRELEASE(_foundation);
}

bool PhysicsManager::init(void)
{
	_game_time = &GetApp().getManagerTFast<GameTimeManager>().getGameTime();
	_job_pool = &GetApp().getJobPool();

	g_physics_task_dispatcher.init();

	_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_physics_allocator, g_physics_error_handler);
	physx::PxPvd* pvd = nullptr;
	bool track_allocs = false;

#ifdef _DEBUG
	_pvd = PxCreatePvd(*_foundation);
	physx::PxPvdTransport* const transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	track_allocs = true;
	pvd = _pvd;
#endif

	_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(), track_allocs, pvd);

	physx::PxSceneDesc scene_desc(_physics->getTolerancesScale());
	scene_desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	scene_desc.cpuDispatcher = &g_physics_task_dispatcher;
	scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
	physx::PxScene* main_scene = _physics->createScene(scene_desc);

	_scenes[Gaff::FNV1aHash32Const("main")] = main_scene;

	if (physx::PxPvdSceneClient* pvd_client = main_scene->getScenePvdClient()) {
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	//gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	//PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	//gScene->addActor(*groundPlane);

	//for (PxU32 i = 0; i < 5; i++)
	//	createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

	//if (!interactive)
	//	createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));

	return true;
}

void PhysicsManager::update(void)
{
	// $TODO: Add to a config file.
	constexpr float k_frame_step = 1.0f / 60.0f;

	_remaining_time += _game_time->getDeltaFloat();

	while (_remaining_time > k_frame_step) {
		for (auto& pair : _scenes) {
			pair.second->simulate(k_frame_step);
		}

		_remaining_time -= k_frame_step;
		
		for (auto& pair : _scenes) {
			while (!pair.second->fetchResults()) {
				_job_pool->doAJob();
				EA::Thread::ThreadSleep();
			}
		}
	}
}



bool PhysicsSystem::init(void)
{
	_physics_mgr = &GetApp().getManagerTFast<PhysicsManager>();
	return true;
}

void PhysicsSystem::update(void)
{
	_physics_mgr->update();
}

NS_END
