/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include "Shibboleth_RigidBodyComponent.h"
#include <Attributes/Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_GameTime.h>
#include <Shibboleth_JobPool.h>
#include <Gaff_IncludeTracy.h>
#include <Gaff_Math.h>
//#include <PxPhysicsAPI.h>

#ifdef _DEBUG
	#include <Shibboleth_DebugAttributes.h>

	SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::PhysicsManager::DebugFlag)
		.entry("Draw Rigid Bodies", Shibboleth::PhysicsManager::DebugFlag::DrawRigidBodies)
	SHIB_REFLECTION_DEFINE_END(Shibboleth::PhysicsManager::DebugFlag)
#endif

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::PhysicsManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()

#ifdef _DEBUG
	.var(
		"Debug Flags",
		&Shibboleth::PhysicsManager::_debug_flags,
		Shibboleth::DebugMenuItemAttribute{ u8"Physics" },
		Shibboleth::NoSerializeAttribute{}
	)
#endif
SHIB_REFLECTION_DEFINE_END(Shibboleth::PhysicsManager)


namespace
{
	/*class PhysicsAllocator final : public physx::PxAllocatorCallback
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
	};*/

	/*class PhysicsErrorHandler final : public physx::PxErrorCallback
	{
	public:
		void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line) override
		{
			GAFF_REF(code, message, file, line);
		}
	};*/

	/*class PhysicsTaskDispatcher final : public physx::PxCpuDispatcher
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

		static void RunTask(uintptr_t thread_id_int, void* data)
		{
			physx::PxBaseTask& task = *reinterpret_cast<physx::PxBaseTask*>(data);
			task.run();
			task.release();
		}
	};*/

	//static PhysicsTaskDispatcher g_physics_task_dispatcher;
	//static PhysicsErrorHandler g_physics_error_handler;
	//static PhysicsAllocator g_physics_allocator;

#ifdef _DEBUG
	/*static constexpr Shibboleth::IDebugManager::DebugRenderType k_render_type_map[] =
	{
		Shibboleth::IDebugManager::DebugRenderType::Sphere,
		Shibboleth::IDebugManager::DebugRenderType::Plane,
		Shibboleth::IDebugManager::DebugRenderType::Capsule,
		Shibboleth::IDebugManager::DebugRenderType::Box,
		Shibboleth::IDebugManager::DebugRenderType::Count, // Convex Mesh
		Shibboleth::IDebugManager::DebugRenderType::Count, // Triangle Mesh
		Shibboleth::IDebugManager::DebugRenderType::Count  // Height field
	};*/
#endif
}


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(PhysicsManager);

PhysicsManager::~PhysicsManager(void)
{
//	for (auto& pair : _scenes) {
//		GAFF_SAFE_RELEASE(pair.second);
//	}

//	GAFF_SAFE_RELEASE(_physics);

//#ifdef _DEBUG
//	physx::PxPvdTransport* transport = _pvd->getTransport();
//	GAFF_SAFE_RELEASE(transport);
//	GAFF_SAFE_RELEASE(_pvd);
//#endif

//	GAFF_SAFE_RELEASE(_foundation);
}

bool PhysicsManager::initAllModulesLoaded(void)
{
	_game_time = &GetManagerTFast<GameTimeManager>().getGameTime();
	return true;
}

bool PhysicsManager::init(void)
{
	_job_pool = &GetApp().getJobPool();

	//g_physics_task_dispatcher.init();

	//_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, g_physics_allocator, g_physics_error_handler);
	//physx::PxPvd* pvd = nullptr;
	//bool track_allocs = false;

#ifdef _DEBUG
	_debug_mgr = &GETMANAGERT(Shibboleth::IDebugManager, Shibboleth::DebugManager);

	//_pvd = PxCreatePvd(*_foundation);
	//physx::PxPvdTransport* const transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	//_pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	//track_allocs = true;
	//pvd = _pvd;
#endif

	//_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *_foundation, physx::PxTolerancesScale(), track_allocs, pvd);

	//physx::PxSceneDesc scene_desc(_physics->getTolerancesScale());
	//scene_desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	//scene_desc.cpuDispatcher = &g_physics_task_dispatcher;
	//scene_desc.filterShader = physx::PxDefaultSimulationFilterShader;
	//physx::PxScene* main_scene = _physics->createScene(scene_desc);

	//_scenes[Gaff::FNV1aHash32Const(u8"main")] = main_scene;

	//if (physx::PxPvdSceneClient* pvd_client = main_scene->getScenePvdClient()) {
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
	//	pvd_client->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	//}

	//gMaterial = _physics->createMaterial(0.5f, 0.5f, 0.6f);

	//PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	//gScene->addActor(*groundPlane);

	//for (PxU32 i = 0; i < 5; i++)
	//	createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);

	//if (!interactive)
	//	createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(10), PxVec3(0, -50, -100));

	//ECSQuery rb_query;
	//rb_query.addShared(_scene_comps);
	//rb_query.add<RigidBody>(_rigid_bodies);
	//rb_query.add<Position>(_positions);
	//rb_query.add<Rotation>(_rotations);
	//rb_query.add<Scale>(_scales);

	//_ecs_mgr = &GetManagerTFast<ECSManager>();
	//_ecs_mgr->registerQuery(std::move(rb_query));

	return true;
}

void PhysicsManager::update(uintptr_t /*thread_id_int*/)
{
	ZoneScoped;

	//const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	// $TODO: Add to a config file.
	constexpr float k_frame_step = 1.0f / 60.0f;

	_remaining_time += _game_time->getDeltaFloat();

	while (_remaining_time > k_frame_step) {
		//for (auto& pair : _scenes) {
		//	pair.second->simulate(k_frame_step);
		//}

		_remaining_time -= k_frame_step;

		// $TODO: While the sim is running, run threads to create new bodies.

		//for (auto& pair : _scenes) {
		//	while (!pair.second->fetchResults()) {
		//		_job_pool->help(thread_id);
		//		EA::Thread::ThreadSleep();
		//	}
		//}
	}

	// Create new bodies and update transforms.
	//const int32_t num_bodies = static_cast<int32_t>(_rigid_bodies.size());

	// $TODO: Thread this loop.
	/*for (int32_t rb_index = 0; rb_index < num_bodies; ++rb_index) {
		_ecs_mgr->iterate<RigidBody, Position, Rotation, Scale>(
			_rigid_bodies[rb_index],
			_positions[rb_index],
			_rotations[rb_index],
			_scales[rb_index],
			[&](ECSEntityID id, RigidBody& rb, Position position, Rotation rotation, Scale scale) -> void
			{
				GAFF_REF(scale);

				// Update transform data to reflect physics state.
				if (rb.body.body_dynamic) {
					const physx::PxTransform transform = (rb.is_static) ? rb.body.body_static->getGlobalPose() : rb.body.body_dynamic->getGlobalPose();

					position.value = Gleam::Vec3(transform.p.x, transform.p.y, transform.p.z);

					const Gleam::Quat rot(transform.q.w, transform.q.x, transform.q.y, transform.q.z);
					rotation.value = glm::eulerAngles(rot) * Gaff::RadToTurns;

					// $TODO: Scale

					Position::Set(*_ecs_mgr, id, position);
					Rotation::Set(*_ecs_mgr, id, rotation);

				// Body needs to be created.
				} else {
					if (!rb.shape->isLoaded()) {
						return;
					}

					const Gleam::Quat rot(rotation.value * Gaff::TurnsToRad);

					const physx::PxTransform transform = physx::PxTransform(
						physx::PxVec3(position.value.x, position.value.y, position.value.z),
						physx::PxQuat(rot.x, rot.y, rot.z, rot.w)
					);

					physx::PxActor* actor = nullptr;

					if (rb.is_static) {
						rb.body.body_static = physx::PxCreateStatic(*_physics, transform, *rb.shape->getShape());
						actor = rb.body.body_static;
					} else {
						rb.body.body_dynamic = physx::PxCreateDynamic(*_physics, transform, *rb.shape->getShape(), rb.density);
						actor = rb.body.body_dynamic;
					}

					const auto it = _scenes.find(_scene_comps[rb_index]->value);

					if (it == _scenes.end()) {
						// $TODO: Log error.
						return;
					}

					it->second->addActor(*actor);
				}
			}
		);
	}*/
}

//physx::PxFoundation* PhysicsManager::getFoundation(void)
//{
//	return _foundation;
//}

//physx::PxPhysics* PhysicsManager::getPhysics(void)
//{
//	return _physics;
//}

/*#ifdef _DEBUG
void PhysicsManager::updateDebug(uintptr_t thread_id_int)
{
	ZoneScoped;

	// Update all rigid body debug draws.
	if (_debug_flags.testAll(DebugFlag::DrawRigidBodies)) {
		int32_t handle_indices[static_cast<size_t>(IDebugManager::DebugRenderType::Count)] = { 0 };
		const int32_t num_bodies = static_cast<int32_t>(_rigid_bodies.size());

		for (int32_t rb_index = 0; rb_index < num_bodies; ++rb_index) {
			_ecs_mgr->iterate<RigidBody>(
				_rigid_bodies[rb_index],
				[&](ECSEntityID, RigidBody& rb) -> void
			{
				if (!rb.body.body_dynamic) {
					return;
				}

				const IDebugManager::DebugRenderType render_type = k_render_type_map[static_cast<int32_t>(rb.shape->getShape()->getGeometryType())];

				if (render_type == IDebugManager::DebugRenderType::Count) {
					return;
				}

				auto& render_handles = _debug_render_handles[static_cast<size_t>(render_type)];
				int32_t& handle_index = handle_indices[static_cast<size_t>(render_type)];
				IDebugManager::DebugRenderHandle* render_handle;

				if (handle_index >= static_cast<int32_t>(render_handles.size())) {
					switch (render_type) {
						case IDebugManager::DebugRenderType::Plane: {
							const auto handle = _debug_mgr->renderDebugPlane(glm::zero<Gleam::Vec3>(), Gleam::Vec3(10000.0f), Gleam::Color::Yellow, true);
							render_handles.emplace_back(handle);
						} break;

						case IDebugManager::DebugRenderType::Sphere: {
							const auto handle = _debug_mgr->renderDebugSphere(glm::zero<Gleam::Vec3>(), 1.0f, Gleam::Color::Yellow, true);
							render_handles.emplace_back(handle);
						} break;

						case IDebugManager::DebugRenderType::Capsule: {
							const auto handle = _debug_mgr->renderDebugCapsule(glm::zero<Gleam::Vec3>(), 1.0f, 1.0f, Gleam::Color::Yellow, true);
							render_handles.emplace_back(handle);
						} break;

						case IDebugManager::DebugRenderType::Box: {
							const auto handle = _debug_mgr->renderDebugBox(glm::zero<Gleam::Vec3>(), glm::one<Gleam::Vec3>(), Gleam::Color::Yellow, true);
							render_handles.emplace_back(handle);
						} break;

						default:
							// Should never happen.
							GAFF_ASSERT(false);
							break;
					}

					render_handle = &render_handles.back();

				} else {
					render_handle = &render_handles[handle_index];
				}

				auto& debug_instance = render_handle->getInstance();

				const physx::PxTransform transform = (rb.is_static) ? rb.body.body_static->getGlobalPose() : rb.body.body_dynamic->getGlobalPose();

				Gleam::Vec3 position(transform.p.x, transform.p.y, transform.p.z);
				Gleam::Quat rotation(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

				switch (render_type) {
					case IDebugManager::DebugRenderType::Plane:
						position -= debug_instance.transform.getScale() * Gleam::Vec3(0.5f, 0.0f, 0.5f);
						break;

					case IDebugManager::DebugRenderType::Capsule: {
						physx::PxCapsuleGeometry capsule;

						if (rb.shape->getShape()->getCapsuleGeometry(capsule)) {
							debug_instance.transform.setScale(Gleam::Vec3(capsule.radius, capsule.halfHeight, capsule.radius) * 2.0f);
						}
					} break;

					case IDebugManager::DebugRenderType::Sphere: {
						physx::PxSphereGeometry sphere;

						if (rb.shape->getShape()->getSphereGeometry(sphere)) {
							debug_instance.transform.setScale(sphere.radius * 2.0f);
						}
					} break;

					case IDebugManager::DebugRenderType::Box: {
						physx::PxBoxGeometry box;

						if (rb.shape->getShape()->getBoxGeometry(box)) {
							const physx::PxVec3 box_scale = box.halfExtents * 2.0f;
							debug_instance.transform.setScale(Gleam::Vec3(box_scale.x, box_scale.y, box_scale.z));
						}
					} break;

					default:
						// Should never happen.
						GAFF_ASSERT(false);
						break;
				}

				debug_instance.transform.setTranslation(position);
				debug_instance.transform.setRotation(rotation);

				++handle_index;
			});
		}

		for (int32_t i = 0; i < std::size(handle_indices); ++i) {
			_debug_render_handles[i].resize(handle_indices[i]);
		}

	} else {
		for (auto& handles : _debug_render_handles) {
			handles.resize(0);
		}
	}
}
#endif*/

NS_END
