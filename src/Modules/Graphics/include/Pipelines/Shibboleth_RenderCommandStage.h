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

#include "Shibboleth_IModelStageRegistration.h"
#include "Shibboleth_RenderManager.h"
#include "Shibboleth_IRenderStage.h"
#include "Resources/Shibboleth_ProgramBuffersResource.h"
#include "Resources/Shibboleth_BufferResource.h"
#include <Ptrs/Shibboleth_ManagerRef.h>
#include <Shibboleth_JobPool.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_IShader.h>
//#include <Gaff_IncludeEASTLAtomic.h>
#include <eathread/eathread_rwmutex.h>


NS_GLEAM
	class RenderTarget;
NS_END

NS_SHIBBOLETH

class ResourceManager;

// $TODO: This might need to be split up into more discrete stages, such as a culling stage.
class RenderCommandStage final : public IRenderStage, public IModelStageRegistration
{
public:
	bool init(RenderManager& render_mgr) override;
	//void destroy(RenderManager& /*render_mgr*/) override;

	void update(uintptr_t thread_id_int) override;

	const RenderCommandData& getRenderCommands(void) const override;

	// $TODO: Should these APIs use a queue model instead?
	// $TODO: Register something that can retrive the final transform of the model.
	ModelInstanceHandle registerModel(const ModelData& data, const ITransformProvider& transform_provider) override;
	void unregisterModel(ModelInstanceHandle handle) override;

	SHIB_REFLECTION_CLASS_DECLARE(RenderCommandStage);

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

		struct MeshInstanceData final
		{
			PipelineData pipeline_data[static_cast<size_t>(Gleam::IShader::Type::PipelineCount)];
			ResourcePtr<ProgramBuffersResource> program_buffers;

			InstanceBufferData* model_to_proj_data = nullptr;

			int32_t buffer_instance_count = 1;
			int32_t model_to_proj_offset = -1;
		};

		Vector<const ITransformProvider*> transform_providers{ GRAPHICS_ALLOCATOR };
		Vector<MeshInstanceData> mesh_instances{ GRAPHICS_ALLOCATOR };

		// $TODO: When texture arrays are a thing, use this lock when copying new textures.
		EA::Thread::RWMutex lock;
	};

	struct RenderJobData final
	{
		RenderCommandStage* rcs;
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

		RenderCommandStage* rcs;

		Vector<RenderJobData> render_job_data_cache{ GRAPHICS_ALLOCATOR };
		Vector<Gaff::JobData> job_data_cache{ GRAPHICS_ALLOCATOR };
		Gleam::RenderDevice* device;
		Gaff::Counter job_counter = 0;
	};

	RenderCommandData _render_commands;

	VectorMap<Gaff::Hash64, InstanceData> _instance_data{ GRAPHICS_ALLOCATOR };
	EA::Thread::RWMutex _instance_data_lock;

	Vector<DeviceJobData> _device_job_data_cache{ GRAPHICS_ALLOCATOR };
	Vector<Gaff::JobData> _job_data_cache{ GRAPHICS_ALLOCATOR };
	Gaff::Counter _job_counter = 0;

	ManagerRef<ResourceManager> _resource_mgr;
	ManagerRef<RenderManager> _render_mgr;
	JobPool* _job_pool = nullptr;

	void addModelInstance(const ModelData& data, InstanceData& instance_data, Gaff::Hash64 instance_hash, const ITransformProvider& transform_provider);
	bool createInstanceBucket(const ModelData& data, Gaff::Hash64 bucket_hash);

	void addStructuredBuffersSRVs(
		InstanceData& instance_data,
		Gaff::Hash64 instance_hash,
		const Vector<Gleam::RenderDevice*>& devices,
		const Gleam::IShader::Type shader_type,
		const Gleam::ShaderReflection& refl,
		const MaterialResource& material
	);

	void addTextureSRVs(
		const Gleam::IShader::Type shader_type,
		const MaterialData& material_data,
		const Gleam::ShaderReflection& refl,
		InstanceData::VarMap& var_map,
		Gleam::RenderDevice& rd
	);

	void addConstantBuffers(
		Gaff::Hash64 instance_hash,
		const Gleam::IShader::Type shader_type,
		const Gleam::ShaderReflection& refl,
		Gleam::ProgramBuffers& pb,
		Gleam::RenderDevice& rd
	);

	void addSamplers(
		const Gleam::IShader::Type shader_type,
		const MaterialData& material_data,
		const Gleam::ShaderReflection& refl,
		Gleam::ProgramBuffers& pb,
		Gleam::RenderDevice& rd
	);


	static void GenerateCommandListJob(uintptr_t id_int, void* data);
	static void DeviceJob(uintptr_t id_int, void* data);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderCommandStage)
