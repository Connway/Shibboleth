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

#include "Shibboleth_IDebugManager.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_ECSQuery.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_ISystem.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_ModelResource.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IDepthStencilState.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_ICommandList.h>
#include <Gleam_IRasterState.h>
#include <Gleam_IBlendState.h>
#include <Gleam_Transform.h>
#include <Gleam_IProgram.h>
#include <Gleam_IShader.h>
#include <Gleam_IBuffer.h>
#include <Gleam_ILayout.h>
#include <Gleam_IMesh.h>
#include <Gaff_Flags.h>
#include <EAThread/eathread_spinlock.h>

NS_GLEAM
	class IRenderOutput;
	class IKeyboard;
NS_END

NS_SHIBBOLETH

class RenderManagerBase;
class InputManager;
class ECSManager;
struct Time;

class DebugManager final : public IDebugManager
{
public:
	enum class DebugFlag
	{
		DrawFPS,

		Count
	};

	static void SetupModuleToUseImGui(void);

	~DebugManager(void);

	bool initAllModulesLoaded(void) override;

	void update(void);
	void render(uintptr_t thread_id_int);

	ImGuiContext* getImGuiContext(void) const override;

	DebugRenderHandle renderDebugArrow(const Gleam::Vec3& start, const Gleam::Vec3& end, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugLine(const Gleam::Vec3& start, const Gleam::Vec3& end, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugSphere(const Gleam::Vec3& pos, float radius = 1.0f, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugCone(const Gleam::Vec3& pos, const Gleam::Vec3& size = Gleam::Vec3(1.0f), const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false);
	DebugRenderHandle renderDebugPlane(const Gleam::Vec3& pos, const Gleam::Vec3& size = Gleam::Vec3(1.0f), const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugBox(const Gleam::Vec3& pos, const Gleam::Vec3& size = Gleam::Vec3(1.0f), const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugCapsule(const Gleam::Vec3& pos, float radius = 1.0f, float height = 1.0f, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugCylinder(const Gleam::Vec3& pos, float radius = 1.0f, float height = 1.0f, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugModel(const ModelResourcePtr& model, const Gleam::Transform& transform, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;

	void registerDebugMenuItems(void* object, const Gaff::IReflectionDefinition& ref_def) override;
	void unregisterDebugMenuItems(void* object, const Gaff::IReflectionDefinition& ref_def) override;

private:
	struct DebugRenderInstanceData final
	{
		UniquePtr<Gleam::IProgramBuffers> program_buffers;
		UniquePtr<Gleam::IBuffer> constant_buffer;
		UniquePtr<Gleam::IBuffer> vertices[2];
		UniquePtr<Gleam::IBuffer> indices[2];
		UniquePtr<Gleam::IMesh> mesh[2];

		// 0 = no depth test, 1 = depth test
		EA::Thread::SpinLock lock[2];

		// 0 = no depth test, 1 = depth test
		UniquePtr<Gleam::ICommandList> cmd_list[2];

		// 0 = no depth test, 1 = depth test
		Vector< UniquePtr<Gleam::IShaderResourceView> > instance_data_view[2] = {
			Vector< UniquePtr<Gleam::IShaderResourceView> >{ ProxyAllocator("Debug") },
			Vector< UniquePtr<Gleam::IShaderResourceView> >{ ProxyAllocator("Debug") }
		};

		// 0 = no depth test, 1 = depth test
		Vector< UniquePtr<Gleam::IBuffer> > instance_data[2] = {
			Vector< UniquePtr<Gleam::IBuffer> >{ ProxyAllocator("Debug") },
			Vector< UniquePtr<Gleam::IBuffer> >{ ProxyAllocator("Debug") }
		};

		// 0 = no depth test, 1 = depth test
		Vector< UniquePtr<DebugRenderInstance> > render_list[2] = {
			Vector< UniquePtr<DebugRenderInstance> >{ ProxyAllocator("Debug") },
			Vector< UniquePtr<DebugRenderInstance> >{ ProxyAllocator("Debug") }
		};
	};

	struct DebugRenderJobData final
	{
		DebugRenderType type = DebugRenderType::Count;
		DebugManager* debug_mgr = nullptr;
		UniquePtr<Gleam::ICommandList> cmd_list[2][2];
	};

	struct DebugRenderData final
	{
		UniquePtr<Gleam::IProgram> line_program;
		UniquePtr<Gleam::IProgram> program;
		UniquePtr<Gleam::ILayout> layout;

		UniquePtr<Gleam::IShader> line_vertex_shader;
		UniquePtr<Gleam::IShader> vertex_shader;
		UniquePtr<Gleam::IShader> pixel_shader;

		// 0 = no depth test, 1 = depth test
		UniquePtr<Gleam::IDepthStencilState> depth_stencil_state[2];
		UniquePtr<Gleam::IRasterState> raster_state[2];

		DebugRenderJobData render_job_data_cache[static_cast<size_t>(DebugRenderType::Count)];
		Gaff::JobData job_data_cache[static_cast<size_t>(DebugRenderType::Count)];
		DebugRenderInstanceData instance_data[static_cast<size_t>(DebugRenderType::Count)];
		VectorMap<ModelResourcePtr, DebugRenderInstanceData> model_instance_data{ ProxyAllocator("Debug") };

		Gaff::Counter job_counter = 0;
	};

	struct DebugMenuEntry final
	{
		enum class Type : int8_t
		{
			Var,
			Func,
			StaticFunc
		};

		Vector<DebugMenuEntry> children{ ProxyAllocator{ "Debug" } };
		HashString32<> name{ ProxyAllocator{ "Debug" } };
		void* object = nullptr;

		union
		{
			Gaff::IReflectionStaticFunction<void>* static_func = nullptr;
			Gaff::IReflectionFunction<void>* func;
			Gaff::IReflectionVar* var;
		};


		Type type = Type::Var;

		bool operator<(const DebugMenuEntry& rhs) const { return name < rhs.name; }
		bool operator<(const HashString32<>& rhs) const { return name < rhs; }
	};

	enum class Flag
	{
		ShowDebugMenu,

		Count
	};


	const Time* _time = nullptr;
	int32_t _prev_cursor = -1;
	int32_t _char_buffer_cache_index = 0;
	int32_t _render_cache_index = 0;

	Gleam::IRenderOutput* _main_output = nullptr;
	Gleam::IRenderDevice* _main_device = nullptr;

	Vector<uint32_t> _character_buffer[2] = {
		Vector<uint32_t>{ ProxyAllocator("Debug") },
		Vector<uint32_t>{ ProxyAllocator("Debug") }
	};

	UniquePtr<Gleam::ICommandList> _cmd_list[2];

	// ImGui
	UniquePtr<Gleam::IBuffer> _vertex_buffer;
	UniquePtr<Gleam::IBuffer> _index_buffer;
	UniquePtr<Gleam::IMesh> _mesh;

	UniquePtr<Gleam::IDepthStencilState> _depth_stencil_state;
	UniquePtr<Gleam::IProgramBuffers> _program_buffers;
	UniquePtr<Gleam::IShaderResourceView> _font_srv;
	UniquePtr<Gleam::IBuffer> _vert_constant_buffer;
	UniquePtr<Gleam::IRasterState> _raster_state;
	UniquePtr<Gleam::IBlendState> _blend_state;
	UniquePtr<Gleam::ITexture> _font_texture;
	UniquePtr<Gleam::IShader> _vertex_shader;
	UniquePtr<Gleam::ISamplerState> _sampler;
	UniquePtr<Gleam::IShader> _pixel_shader;
	UniquePtr<Gleam::IProgram> _program;
	UniquePtr<Gleam::ILayout> _layout;

	ECSQuery::Output _camera_position{ ProxyAllocator("Debug") };
	ECSQuery::Output _camera_rotation{ ProxyAllocator("Debug") };
	ECSQuery::Output _camera{ ProxyAllocator("Debug") };

	RenderManagerBase* _render_mgr = nullptr;
	InputManager* _input_mgr = nullptr;
	ECSManager* _ecs_mgr = nullptr;

	DebugRenderData _debug_data;
	DebugMenuEntry _debug_menu_root;

	Gaff::Flags<DebugFlag> _debug_flags;
	Gaff::Flags<Flag> _flags;

	static void HandleKeyboardCharacter(Gleam::IKeyboard*, uint32_t character);
	static void RenderDebugShape(uintptr_t thread_id_int, DebugRenderJobData& job_data, const ModelResourcePtr& model, DebugRenderInstanceData& instance_data);
	static void RenderDebugShape(uintptr_t thread_id_int, void* data);

	void renderPostCamera(uintptr_t thread_id_int);
	void renderPreCamera(uintptr_t thread_id_int);

	void removeDebugRender(const DebugRenderHandle& handle) override;

	bool initDebugRender(void);
	bool initImGui(void);

	void renderDebugMenu(const DebugMenuEntry& entry);

	SHIB_REFLECTION_CLASS_DECLARE(DebugManager);
};

class DebugRenderSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	DebugManager* _debug_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(DebugRenderSystem);
};

class DebugSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	DebugManager* _debug_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(DebugSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(DebugManager::DebugFlag)
SHIB_REFLECTION_DECLARE(DebugRenderSystem)
SHIB_REFLECTION_DECLARE(DebugManager)
SHIB_REFLECTION_DECLARE(DebugSystem)
