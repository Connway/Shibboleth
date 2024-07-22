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

#include "Shibboleth_ModelInstanceData.h"
#include "Pipelines/Shibboleth_IRenderPipelineData.h"
#include <Gleam_ProgramBuffers.h>

NS_SHIBBOLETH

class ModelPipelineData final : public IRenderPipelineData
{
public:
	struct InstanceBuffer final
	{
		UniquePtr<Gleam::ShaderResourceView> srv;
		UniquePtr<Gleam::Buffer> buffer;

		// $TODO: Command list per buffer?
	};

	struct InstanceTexture final
	{
		UniquePtr<Gleam::ShaderResourceView> srv;
		const Gleam::Texture* texture = nullptr; // Resource should keep this texture alive for now. In the future, we won't need to hold onto this reference, as we will create a texture array.
	};

	struct InstanceBufferData final
	{
		Vector<InstanceBuffer> pages{ GRAPHICS_ALLOCATOR };
		int32_t srv_index = -1;
	};

	using ConstBufferVarMap = VectorMap< HashString32<>, UniquePtr<Gleam::Buffer> >;
	using BufferVarMap = VectorMap<HashString32<>, InstanceBufferData>;
	using VarMap = VectorMap<HashString32<>, InstanceTexture>;

	struct PipelineData final
	{
		ConstBufferVarMap const_buffer_vars{ GRAPHICS_ALLOCATOR };
		BufferVarMap buffer_vars{ GRAPHICS_ALLOCATOR };
		VarMap srv_vars{ GRAPHICS_ALLOCATOR };
	};

	struct MeshInstanceDeviceData final
	{
		PipelineData pipeline_data[static_cast<size_t>(Gleam::IShader::Type::PipelineCount)];
		UniquePtr<Gleam::ProgramBuffers> program_buffers;

		Gleam::RasterState* raster_state = nullptr;
		Gleam::Program* program = nullptr;
		Gleam::Layout* layout = nullptr;
		Gleam::Mesh* mesh = nullptr;

		InstanceBufferData* instance_data = nullptr;

		// $TODO: Command list for each mesh to render.
	};

	struct MeshInstance final
	{
		VectorMap<const Gleam::RenderDevice*, MeshInstanceDeviceData> device_data;

		int32_t buffer_instance_count = 1;
		int32_t model_to_proj_offset = -1;
	};

	struct ModelBucket final
	{
		Vector<const ITransformProvider*> transform_providers{ GRAPHICS_ALLOCATOR };
		Vector<MeshInstance> mesh_instances{ GRAPHICS_ALLOCATOR };

		ResourcePtr<ModelResource> model;
	};

	struct NewModelInstance final
	{
		ModelInstanceData data;
		Vector<ModelInstanceHandle> handles{ GRAPHICS_ALLOCATOR };
	};



	bool init(RenderManager& render_mgr) override;

	// Should never be called with model_data resources not loaded.
	ModelInstanceHandle registerModel(ModelInstanceData& model_data, const ITransformProvider& tform_provider);
	void unregisterModel(ModelInstanceHandle handle);

	void markDirty(ModelInstanceHandle handle);

	void processChanges(uintptr_t thread_id_int);

	// $TODO: Register model instance as dirty.

	const VectorMap<Gaff::Hash64, ModelBucket>& getRegisteredModels(void) const;

	int32_t getModelCount(void) const;
	int32_t getMeshCount(void) const;

private:
	VectorMap< Gaff::Hash64, Vector<ModelInstanceHandle> > _pending_removes{ GRAPHICS_ALLOCATOR };
	VectorMap<Gaff::Hash64, NewModelInstance> _new_models{ GRAPHICS_ALLOCATOR };
	VectorMap<Gaff::Hash64, ModelBucket> _model_buckets{ GRAPHICS_ALLOCATOR };

	VectorMap< Gaff::Hash64, Vector<ModelInstanceHandle> > _pending_removes_cache{ GRAPHICS_ALLOCATOR };
	VectorMap<Gaff::Hash64, NewModelInstance> _new_models_cache{ GRAPHICS_ALLOCATOR };

	EA::Thread::Mutex _pending_removes_lock;
	EA::Thread::Mutex _new_models_lock;

	int32_t _mesh_count = 0;

	RenderManager* _render_mgr = nullptr;


	void addInstance(ModelInstanceData& model_data, ModelInstanceHandle handle);
	void removeInstance(ModelInstanceHandle handle);

	ModelBucket& createBucket(ModelInstanceData& model_data, ModelInstanceHandle handle);

	void addStructuredBuffersSRVs(
		MeshInstance& mesh_instance,
		Gleam::RenderDevice& device,
		MeshInstanceDeviceData& device_data,
		const Gleam::IShader::Type shader_type,
		const Gleam::ShaderReflection& refl
	);

	void addTextureSRVs(
		Gleam::RenderDevice& device,
		MeshInstanceDeviceData& device_data,
		const Gleam::IShader::Type shader_type,
		const Gleam::ShaderReflection& refl,
		const MaterialInstanceData& material_data
	);

	void addConstantBuffers(
		Gleam::RenderDevice& device,
		MeshInstanceDeviceData& device_data,
		const Gleam::IShader::Type shader_type,
		const Gleam::ShaderReflection& refl,
		Gleam::ProgramBuffers& pb
	);

	void addSamplers(
		Gleam::RenderDevice& device,
		const Gleam::IShader::Type shader_type,
		const Gleam::ShaderReflection& refl,
		const MaterialInstanceData& material_data,
		Gleam::ProgramBuffers& pb
	);

	SHIB_REFLECTION_CLASS_DECLARE(ModelPipelineData);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ModelPipelineData)
