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
#include <Shibboleth_ISystem.h>
#include <Shibboleth_ECSQuery.h>

NS_SHIBBOLETH

class RenderManagerBase;
class ResourceManager;
class ECSManager;

class RenderCommandSystem final : public ISystem
{
public:
	bool init(void) override;
	void update() override;

private:
	struct InstanceData final
	{
		struct InstanceBufferData final
		{
			BufferResourcePtr buffer;
			VectorMap< const Gleam::IRenderDevice*, UniquePtr<Gleam::IShaderResourceView> > srv_map{ ProxyAllocator("Graphics") };
			int32_t srv_index = -1;
		};

		using BufferVarMap = VectorMap< HashString32, Vector<InstanceBufferData> >;

		ProgramBuffersResourcePtr program_buffers;

		BufferVarMap buffers[Gleam::IShader::SHADER_PIPELINE_COUNT]{
			BufferVarMap{ ProxyAllocator("Graphics") },
			BufferVarMap{ ProxyAllocator("Graphics") },
			BufferVarMap{ ProxyAllocator("Graphics") },
			BufferVarMap{ ProxyAllocator("Graphics") },
			BufferVarMap{ ProxyAllocator("Graphics") }
		};

		using VarMap = VectorMap< HashString32, UniquePtr<Gleam::IShaderResourceView> >;
		VectorMap<const Gleam::IRenderDevice*, VarMap> shader_srvs[Gleam::IShader::SHADER_PIPELINE_COUNT]{
			VectorMap<const Gleam::IRenderDevice*, VarMap>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, VarMap>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, VarMap>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, VarMap>{ ProxyAllocator("Graphics") },
			VectorMap<const Gleam::IRenderDevice*, VarMap>{ ProxyAllocator("Graphics") }
		};

		Vector<InstanceBufferData>* instance_data = nullptr;
		int32_t buffer_instance_count = 1;
		int32_t model_to_proj_offset = -1;
	};

	RenderManagerBase* _render_mgr = nullptr;
	ResourceManager* _res_mgr = nullptr;
	ECSManager* _ecs_mgr = nullptr;

	Vector<InstanceData> _instance_data{ ProxyAllocator("Graphics") };

	// Entities
	Vector<const InstanceBufferCount*> _buffer_count{ ProxyAllocator("Graphics") };
	Vector<const Material*> _materials{ ProxyAllocator("Graphics") };
	Vector<const Model*> _models{ ProxyAllocator("Graphics") };

	Vector<ECSQueryResult> _camera_position{ ProxyAllocator("Graphics") };
	Vector<ECSQueryResult> _camera_rotation{ ProxyAllocator("Graphics") };
	Vector<ECSQueryResult> _camera{ ProxyAllocator("Graphics") };

	Vector<ECSQueryResult> _position{ ProxyAllocator("Graphics") };
	Vector<ECSQueryResult> _rotation{ ProxyAllocator("Graphics") };
	Vector<ECSQueryResult> _scale{ ProxyAllocator("Graphics") };

	// Cameras

	void newArchetype(const ECSArchetype& archetype);
	void removedArchetype(int32_t index);

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

	SHIB_REFLECTION_CLASS_DECLARE(RenderCommandSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(RenderCommandSystem)
