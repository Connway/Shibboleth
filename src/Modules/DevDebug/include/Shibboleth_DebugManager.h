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

#include "Shibboleth_IDebugManager.h"
#include <Model/Shibboleth_ModelResource.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_JobPool.h>
#include <Gleam_ShaderResourceView.h>
#include <Gleam_DepthStencilState.h>
#include <Gleam_ProgramBuffers.h>
#include <Gleam_RenderDevice.h>
#include <Gleam_SamplerState.h>
#include <Gleam_CommandList.h>
#include <Gleam_RasterState.h>
#include <Gleam_BlendState.h>
#include <Gleam_Transform.h>
#include <Gleam_Texture.h>
#include <Gleam_Program.h>
#include <Gleam_Shader.h>
#include <Gleam_Buffer.h>
#include <Gleam_Layout.h>
#include <Gleam_Mesh.h>
#include <Gaff_Flags.h>
#include <eathread/eathread_spinlock.h>

NS_GLEAM
	class RenderOutput;
	class Window;

	enum class MouseButton : uint8_t;
	enum class MouseCode : uint8_t;
	enum class KeyCode : uint16_t;
	enum class Modifier : uint8_t;
NS_END

struct GLFWcursor;

NS_SHIBBOLETH

class RenderManager;
class InputManager;
struct Time;

// $TODO: Port from ECS.
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
	//void lockImGui(void) override;
	//void unlockImGui(void) override;

	DebugRenderHandle renderDebugArrow(const Gleam::Vec3& start, const Gleam::Vec3& end, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugLine(const Gleam::Vec3& start, const Gleam::Vec3& end, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugSphere(const Gleam::Vec3& pos, float radius = 1.0f, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugCone(const Gleam::Vec3& pos, const Gleam::Vec3& size = Gleam::Vec3(1.0f), const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugPlane(const Gleam::Vec3& pos, const Gleam::Vec3& size = Gleam::Vec3(1.0f), const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugBox(const Gleam::Vec3& pos, const Gleam::Vec3& size = Gleam::Vec3(1.0f), const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugCapsule(const Gleam::Vec3& pos, float radius = 1.0f, float height = 1.0f, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugCylinder(const Gleam::Vec3& pos, float radius = 1.0f, float height = 1.0f, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;
	DebugRenderHandle renderDebugModel(const ResourcePtr<ModelResource>& model, const Gleam::Transform& transform, const Gleam::Color::RGB& color = Gleam::Color::White, bool has_depth = false) override;

	void registerDebugMenuItems(void* object, const Refl::IReflectionDefinition& ref_def) override;
	void unregisterDebugMenuItems(void* object, const Refl::IReflectionDefinition& ref_def) override;

private:
	enum class DepthTest
	{
		Disabled,
		Enabled,

		Count
	};

	struct DebugRenderInstanceData final
	{
		UniquePtr<Gleam::ProgramBuffers> program_buffers;
		UniquePtr<Gleam::Buffer> constant_buffer;
		UniquePtr<Gleam::Buffer> vertices[2];
		UniquePtr<Gleam::Buffer> indices[2];
		UniquePtr<Gleam::Mesh> mesh[2];

		// 0 = no depth test, 1 = depth test
		EA::Thread::SpinLock lock[static_cast<size_t>(DepthTest::Count)];

		// 0 = no depth test, 1 = depth test
		UniquePtr<Gleam::CommandList> cmd_list[static_cast<size_t>(DepthTest::Count)];

		// 0 = no depth test, 1 = depth test
		Vector< UniquePtr<Gleam::IShaderResourceView> > instance_data_view[static_cast<size_t>(DepthTest::Count)] = {
			Vector< UniquePtr<Gleam::IShaderResourceView> >{ ProxyAllocator("Debug") },
			Vector< UniquePtr<Gleam::IShaderResourceView> >{ ProxyAllocator("Debug") }
		};

		// 0 = no depth test, 1 = depth test
		Vector< UniquePtr<Gleam::IBuffer> > instance_data[static_cast<size_t>(DepthTest::Count)] = {
			Vector< UniquePtr<Gleam::IBuffer> >{ ProxyAllocator("Debug") },
			Vector< UniquePtr<Gleam::IBuffer> >{ ProxyAllocator("Debug") }
		};

		// 0 = no depth test, 1 = depth test
		Vector< UniquePtr<DebugRenderInstance> > render_list[static_cast<size_t>(DepthTest::Count)] = {
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
		UniquePtr<Gleam::Program> line_program;
		UniquePtr<Gleam::Program> program;
		UniquePtr<Gleam::Layout> layout;

		UniquePtr<Gleam::Shader> line_vertex_shader;
		UniquePtr<Gleam::Shader> vertex_shader;
		UniquePtr<Gleam::Shader> pixel_shader;

		// 0 = no depth test, 1 = depth test
		UniquePtr<Gleam::DepthStencilState> depth_stencil_state[static_cast<size_t>(DepthTest::Count)];
		UniquePtr<Gleam::RasterState> raster_state[static_cast<size_t>(DepthTest::Count)];

		DebugRenderJobData render_job_data_cache[static_cast<size_t>(DebugRenderType::Count)];
		Gaff::JobData job_data_cache[static_cast<size_t>(DebugRenderType::Count)];
		DebugRenderInstanceData instance_data[static_cast<size_t>(DebugRenderType::Count)];
		VectorMap<ResourcePtr<ModelResource>, DebugRenderInstanceData> model_instance_data{ ProxyAllocator("Debug") };

		Gaff::Counter job_counter = 0;
	};

	struct DebugMenuEntry final
	{
		enum class Type : int8_t
		{
			Var,
			Func,
			StaticFunc,
			FuncImGuiUpdate,
			StaticFuncImGuiUpdate
		};

		enum class Flag
		{
			AlwaysRender,
			Updating,

			Count
		};

		Vector<DebugMenuEntry> children{ ProxyAllocator{ "Debug" } };
		HashString32<> name{ ProxyAllocator{ "Debug" } };
		void* object = nullptr;

		union
		{
			Refl::IReflectionStaticFunction<void>* static_func = nullptr;
			Refl::IReflectionFunction<void>* func;
			Refl::IReflectionVar* var;
		};


		Gaff::Flags<Flag> flags;
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
	//int32_t _char_buffer_cache_index = 0;
	int32_t _render_cache_index = 0;

	Gleam::RenderOutput* _main_output = nullptr;
	Gleam::RenderDevice* _main_device = nullptr;
	Gleam::Window* _main_window = nullptr;

	//Vector<uint32_t> _character_buffer[2] = {
	//	Vector<uint32_t>{ ProxyAllocator("Debug") },
	//	Vector<uint32_t>{ ProxyAllocator("Debug") }
	//};

	GLFWcursor* _mouse_cursors[9] = { nullptr };

	UniquePtr<Gleam::CommandList> _cmd_list[2];

	// ImGui
	UniquePtr<Gleam::Buffer> _vertex_buffer;
	UniquePtr<Gleam::Buffer> _index_buffer;
	UniquePtr<Gleam::Mesh> _mesh;

	UniquePtr<Gleam::DepthStencilState> _depth_stencil_state;
	UniquePtr<Gleam::ProgramBuffers> _program_buffers;
	UniquePtr<Gleam::ShaderResourceView> _font_srv;
	UniquePtr<Gleam::Buffer> _vert_constant_buffer;
	UniquePtr<Gleam::RasterState> _raster_state;
	UniquePtr<Gleam::BlendState> _blend_state;
	UniquePtr<Gleam::Texture> _font_texture;
	UniquePtr<Gleam::Shader> _vertex_shader;
	UniquePtr<Gleam::SamplerState> _sampler;
	UniquePtr<Gleam::Shader> _pixel_shader;
	UniquePtr<Gleam::Program> _program;
	UniquePtr<Gleam::Layout> _layout;

	//ECSQuery::Output _camera_position{ ProxyAllocator("Debug") };
	//ECSQuery::Output _camera_rotation{ ProxyAllocator("Debug") };
	//ECSQuery::Output _camera{ ProxyAllocator("Debug") };

	Vector<DebugMenuEntry*> _update_functions{ ProxyAllocator("Debug") };

	RenderManager* _render_mgr = nullptr;
	InputManager* _input_mgr = nullptr;

	DebugRenderData _debug_data;
	DebugMenuEntry _debug_menu_root;

	Gleam::Vec2 _last_mouse_pos{ 0.0f, 0.0f };

	Gaff::Flags<DebugFlag> _debug_flags;
	Gaff::Flags<Flag> _flags;

	static void HandleKeyboardCharacterInput(Gleam::Window& window, uint32_t char_code);
	static void HandleKeyboardInput(Gleam::Window& window, Gleam::KeyCode key_code, bool pressed, Gaff::Flags<Gleam::Modifier> modifiers, int32_t scan_code);
	static void HandleMouseButtonInput(Gleam::Window& window, Gleam::MouseButton button_code, bool pressed, Gaff::Flags<Gleam::Modifier> modifiers);
	static void HandleMouseWheelInput(Gleam::Window& window, const Gleam::Vec2& wheel);
	static void HandleMousePosInput(Gleam::Window& window, const Gleam::Vec2& pos);
	static void HandleMouseEnterLeave(Gleam::Window& window, bool entered);
	static void HandleMainWindowClosed(Gleam::Window& window);
	static void HandleFocus(Gleam::Window& window, bool focused);

	static void SetClipboardText(void* user_data, const char* text);
	static const char* GetClipboardText(void* user_data);

	static void RenderDebugShape(uintptr_t thread_id_int, DebugRenderJobData& job_data, const ResourcePtr<ModelResource>& model, DebugRenderInstanceData& instance_data);
	static void RenderDebugShape(uintptr_t thread_id_int, void* data);

	void renderPostCamera(uintptr_t thread_id_int);
	void renderPreCamera(uintptr_t thread_id_int);

	void removeDebugRender(const DebugRenderHandle& handle) override;

	bool initDebugRender(void);
	bool initImGui(void);

	void renderDebugMenu(DebugMenuEntry& entry);

	SHIB_REFLECTION_CLASS_DECLARE(DebugManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::DebugManager::DebugFlag)
SHIB_REFLECTION_DECLARE(Shibboleth::DebugManager)
