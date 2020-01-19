/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_GraphicsResourceComponents.h"
#include "Shibboleth_ProgramBuffersResource.h"
#include "Shibboleth_MaterialComponent.h"
#include "Shibboleth_BufferResource.h"
#include <Shibboleth_ECSQuery.h>
#include <Shibboleth_ISystem.h>
#include <Shibboleth_JobPool.h>
#include <Gleam_IncludeMatrix.h>
#include <Gleam_ICommandList.h>

NS_GLEAM
	class IRenderTarget;
NS_END

NS_SHIBBOLETH

class RenderManagerBase;
class ResourceManager;
class ECSManager;

class RenderCommandSystem final : public ISystem
{
public:
	static constexpr const char* StructuredBufferFormat = "RenderCommandSystem:StructuredBuffer:%s:%llu:%i";
	static constexpr const char* ProgramBuffersFormat = "RenderCommandSystem:ProgramBuffers:%llu";
	static constexpr const char* ConstBufferFormat = "RenderCommandSystem:ConstBuffer:%s:%llu";

	bool init(void) override;
	void update() override;

private:
	struct InstanceData final
	{
		struct DeferredContextData final
		{
			UniquePtr<Gleam::IRenderDevice> deferred_context;
			UniquePtr<Gleam::ICommandList> command_list;
		};

		struct InstanceBufferPage final
		{
			VectorMap< const Gleam::IRenderDevice*, UniquePtr<Gleam::IShaderResourceView> > srv_map{ ProxyAllocator("Graphics") };
			BufferResourcePtr buffer;
		};

		struct InstanceBufferData final
		{
			Vector<InstanceBufferPage> pages{ ProxyAllocator("Graphics") };
			U8String buffer_string{ ProxyAllocator("Graphics") };
			int32_t srv_index = -1;
		};

		using BufferVarMap = VectorMap<HashString32, InstanceBufferData>;
		using VarMap = VectorMap< HashString32, UniquePtr<Gleam::IShaderResourceView> >;

		struct PipelineData final
		{
			BufferVarMap buffer_vars{ ProxyAllocator("Graphics") };
			VectorMap<const Gleam::IRenderDevice*, VarMap> srv_vars{ ProxyAllocator("Graphics") };
		};

		PipelineData pipeline_data[Gleam::IShader::SHADER_PIPELINE_COUNT];
		ProgramBuffersResourcePtr program_buffers;

		InstanceBufferData* instance_data = nullptr;

		VectorMap<const Gleam::IRenderDevice*, DeferredContextData> deferred_data;
		int32_t buffer_instance_count = 1;
		int32_t model_to_proj_offset = -1;
	};

	struct RenderJobData final
	{
		RenderCommandSystem* rcs;
		int32_t index;

		Gleam::ICommandList* cmd_list;
		Gleam::IRenderDevice* device;
		Gleam::IRenderTarget* target;

		glm::mat4x4 view_projection;
	};

	RenderManagerBase* _render_mgr = nullptr;
	ResourceManager* _res_mgr = nullptr;
	ECSManager* _ecs_mgr = nullptr;
	JobPool* _job_pool = nullptr;

	Vector<InstanceData> _instance_data{ ProxyAllocator("Graphics") };

	// Entities
	Vector<const InstanceBufferCount*> _buffer_count{ ProxyAllocator("Graphics") };
	Vector<const RasterState*> _raster_states{ ProxyAllocator("Graphics") };
	Vector<const Material*> _materials{ ProxyAllocator("Graphics") };
	Vector<const Model*> _models{ ProxyAllocator("Graphics") };

	Vector<ECSQueryResult> _camera_position{ ProxyAllocator("Graphics") };
	Vector<ECSQueryResult> _camera_rotation{ ProxyAllocator("Graphics") };
	Vector<ECSQueryResult> _camera{ ProxyAllocator("Graphics") };

	Vector<ECSQueryResult> _position{ ProxyAllocator("Graphics") };
	Vector<ECSQueryResult> _rotation{ ProxyAllocator("Graphics") };
	Vector<ECSQueryResult> _scale{ ProxyAllocator("Graphics") };

	Vector<RenderJobData> _render_job_data_cache{ ProxyAllocator("Graphics") };
	Vector<Gaff::JobData> _job_data_cache{ ProxyAllocator("Graphics") };
	Gaff::Counter _job_counter = 0;

	// Cameras

	void newObjectArchetype(const ECSArchetype& archetype);
	void removedObjectArchetype(int32_t index);

	void processNewArchetypeMaterial(
		InstanceData& instance_data,
		const Material& material,
		const ECSArchetype& archetype
	);

	void addStructuredBuffersSRVs(
		InstanceData& instance_data,
		const Material& material,
		const ECSArchetype& archetype,
		const Vector<Gleam::IRenderDevice*>& devices,
		Gleam::IShader::ShaderType shader_type
	);

	void addTextureSRVs(
		const Material& material,
		const Gleam::ShaderReflection& refl,
		InstanceData::VarMap& var_map,
		Gleam::IRenderDevice& rd,
		Gleam::IShader::ShaderType shader_type
	);

	void addConstantBuffers(
		const Gleam::ShaderReflection& refl,
		Gleam::IProgramBuffers& pb,
		Gleam::IRenderDevice& rd,
		const ECSArchetype& archetype,
		const Vector<Gleam::IRenderDevice*>& devices,
		Gleam::IShader::ShaderType shader_type
	);

	void addSamplers(
		const Material& material,
		const Gleam::ShaderReflection& refl,
		Gleam::IProgramBuffers& pb,
		Gleam::IRenderDevice& rd,
		Gleam::IShader::ShaderType shader_type
	);

	static void GenerateCommandListJob(void* data);

	SHIB_REFLECTION_CLASS_DECLARE(RenderCommandSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(RenderCommandSystem)
