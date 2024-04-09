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

#pragma once

//#include "Shibboleth_GraphicsResourceComponents.h"
#include "Resources/Shibboleth_ProgramBuffersResource.h"
#include "Resources/Shibboleth_BufferResource.h"
#include "Shibboleth_MaterialComponent.h"
#include <Shibboleth_ISystem.h>
#include <Shibboleth_JobPool.h>
#include <Gleam_Matrix4x4.h>

NS_GLEAM
	class RenderTarget;
	class CommandList;
NS_END

NS_SHIBBOLETH

class ResourceManager;
class RenderManager;

class RenderCommandSubmissionSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	struct SubmissionData final
	{
		RenderCommandSubmissionSystem* rcss;
		Gleam::RenderDevice* device;
	};

	Vector<SubmissionData> _submission_job_data_cache{ GRAPHICS_ALLOCATOR };
	Vector<Gaff::JobData> _job_data_cache{ GRAPHICS_ALLOCATOR };
	Gaff::Counter _job_counter = 0;

	RenderManager* _render_mgr = nullptr;
	int32_t _cache_index = 0;

	static void SubmitCommands(uintptr_t id_int, void* data);

	SHIB_REFLECTION_CLASS_DECLARE(RenderCommandSubmissionSystem);
};

class RenderCommandSystem final : public ISystem
{
public:
	static constexpr const char8_t* StructuredBufferFormat = u8"RenderCommandSystem:StructuredBuffer:%s:%llu:%i";
	static constexpr const char8_t* ProgramBuffersFormat = u8"RenderCommandSystem:ProgramBuffers:%llu";
	static constexpr const char8_t* ConstBufferFormat = u8"RenderCommandSystem:ConstBuffer:%s:%llu";

	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	struct InstanceData final
	{
		struct InstanceBufferPage final
		{
			VectorMap< const Gleam::RenderDevice*, UniquePtr<Gleam::ShaderResourceView> > srv_map{ GRAPHICS_ALLOCATOR };
			ResourcePtr<BufferResource> buffer;
		};

		struct InstanceBufferData final
		{
			Vector<InstanceBufferPage> pages{ GRAPHICS_ALLOCATOR };
			U8String buffer_string{ GRAPHICS_ALLOCATOR };
			int32_t srv_index = -1;
		};

		using BufferVarMap = VectorMap<HashString32<>, InstanceBufferData>;
		using VarMap = VectorMap< HashString32<>, UniquePtr<Gleam::ShaderResourceView> >;

		struct PipelineData final
		{
			BufferVarMap buffer_vars{ GRAPHICS_ALLOCATOR };
			VectorMap<const Gleam::RenderDevice*, VarMap> srv_vars{ GRAPHICS_ALLOCATOR };
		};

		PipelineData pipeline_data[static_cast<size_t>(Gleam::IShader::Type::PipelineCount)];
		ResourcePtr<ProgramBuffersResource> program_buffers;

		InstanceBufferData* instance_data = nullptr;

		int32_t buffer_instance_count = 1;
		int32_t model_to_proj_offset = -1;
	};

	struct RenderJobData final
	{
		RenderCommandSystem* rcs;
		int32_t index;

		Gleam::RenderDevice* device;
		Gleam::CommandList* cmd_list;
		Gleam::RenderTarget* target;

		Gleam::Mat4x4 view_projection;
	};

	struct DeviceJobData final
	{
		DeviceJobData(void) = default;
		DeviceJobData(DeviceJobData&& data)
		{
			rcs = data.rcs;

			render_job_data_cache = std::move(data.render_job_data_cache);
			job_data_cache = std::move(data.job_data_cache);
			device = data.device;
			job_counter = static_cast<int32_t>(data.job_counter);
		}

		RenderCommandSystem* rcs;

		Vector<RenderJobData> render_job_data_cache{ GRAPHICS_ALLOCATOR };
		Vector<Gaff::JobData> job_data_cache{ GRAPHICS_ALLOCATOR };
		Gleam::RenderDevice* device;
		Gaff::Counter job_counter = 0;
	};

	RenderManager* _render_mgr = nullptr;
	ResourceManager* _res_mgr = nullptr;
	//ECSManager* _ecs_mgr = nullptr;
	JobPool* _job_pool = nullptr;

	Vector<InstanceData> _instance_data{ GRAPHICS_ALLOCATOR };

	// Entities
	//Vector<const InstanceBufferCount*> _buffer_count{ GRAPHICS_ALLOCATOR };
	//Vector<const RasterState*> _raster_states{ GRAPHICS_ALLOCATOR };
	//Vector<const Material*> _materials{ GRAPHICS_ALLOCATOR };
	//Vector<const Model*> _models{ GRAPHICS_ALLOCATOR };

	//Vector<ECSQueryResult> _camera_position{ GRAPHICS_ALLOCATOR };
	//Vector<ECSQueryResult> _camera_rotation{ GRAPHICS_ALLOCATOR };
	//Vector<ECSQueryResult> _camera{ GRAPHICS_ALLOCATOR };

	//Vector<ECSQueryResult> _position{ GRAPHICS_ALLOCATOR };
	//Vector<ECSQueryResult> _rotation{ GRAPHICS_ALLOCATOR };
	//Vector<ECSQueryResult> _scale{ GRAPHICS_ALLOCATOR };

	Vector<DeviceJobData> _device_job_data_cache{ GRAPHICS_ALLOCATOR };
	Vector<Gaff::JobData> _job_data_cache{ GRAPHICS_ALLOCATOR };
	Gaff::Counter _job_counter = 0;

	Vector< UniquePtr<Gleam::CommandList> > _cmd_lists[2] = {
		Vector< UniquePtr<Gleam::CommandList> >{ GRAPHICS_ALLOCATOR },
		Vector< UniquePtr<Gleam::CommandList> >{ GRAPHICS_ALLOCATOR }
	};

	int32_t _cmd_list_end[2] = { 0, 0 };

	int32_t _cache_index = 0;

	//void newObjectArchetype(const ECSArchetype& archetype);
	//void removedObjectArchetype(int32_t index);

//	void processNewArchetypeMaterial(
//		InstanceData& instance_data,
//		const Material& material,
//		const ECSArchetype& archetype
//	);
//
//	void addStructuredBuffersSRVs(
//		InstanceData& instance_data,
//		const Material& material,
//		const ECSArchetype& archetype,
//		const Vector<Gleam::IRenderDevice*>& devices,
//		Gleam::IShader::Type shader_type
//	);

//	void addTextureSRVs(
//		const Material& material,
//		const Gleam::ShaderReflection& refl,
//		InstanceData::VarMap& var_map,
//		Gleam::IRenderDevice& rd,
//		Gleam::IShader::Type shader_type
//	);
//
//	void addConstantBuffers(
//		const Gleam::ShaderReflection& refl,
//		Gleam::ProgramBuffers& pb,
//		Gleam::RenderDevice& rd,
//		const ECSArchetype& archetype,
//		const Vector<Gleam::RenderDevice*>& devices,
//		Gleam::IShader::Type shader_type
//	);
//
//	void addSamplers(
//		const Material& material,
//		const Gleam::ShaderReflection& refl,
//		Gleam::ProgramBuffers& pb,
//		Gleam::RenderDevice& rd,
//		Gleam::IShader::Type shader_type
//	);

	static void GenerateCommandListJob(uintptr_t id_int, void* data);
	static void DeviceJob(uintptr_t id_int, void* data);

	SHIB_REFLECTION_CLASS_DECLARE(RenderCommandSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderCommandSubmissionSystem)
SHIB_REFLECTION_DECLARE(Shibboleth::RenderCommandSystem)
