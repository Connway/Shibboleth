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
#include <Shibboleth_IManager.h>
#include <Shibboleth_ISystem.h>
#include <Shibboleth_JobPool.h>
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
#include <EAThread/eathread_spinlock.h>

NS_GLEAM
	class IRenderOutput;
	class IKeyboard;
NS_END

NS_SHIBBOLETH

class RenderManagerBase;
struct Time;

class DebugManager final : public IManager, public IDebugManager
{
public:
	enum class DebugRenderType
	{
		Line,
		Sphere,
		Box,
		Capsule,
		Arrow,
		Mesh,

		Count
	};

	struct DebugRenderInstance final
	{
		Gleam::Transform transform;
		Gleam::Color color;
	};

	class DebugRenderHandle final
	{
	public:
		~DebugRenderHandle(void)
		{
			if (_instance) {
				GetApp().getManagerTFast<DebugManager>().removeDebugRender(*this);
			}
		}

		const DebugRenderHandle& operator=(const DebugRenderHandle& rhs)
		{
			_instance = rhs._instance;
			_type = rhs._type;
			_depth = rhs._depth;

			const_cast<DebugRenderHandle&>(rhs)._instance = nullptr;
		}

	private:
		DebugRenderHandle(DebugRenderInstance* instance, DebugRenderType type, bool depth) :
			_instance(instance),
			_type(type),
			_depth(depth)
		{
		}

		DebugRenderInstance* _instance = nullptr;
		DebugRenderType _type = DebugRenderType::Count;
		bool _depth = false;

		friend class DebugManager;
	};

	static void SetupModuleToUseImGui(void);

	bool initAllModulesLoaded(void) override;

	void update(void);
	void renderPostCamera(uintptr_t thread_id_int);
	void renderPreCamera(uintptr_t thread_id_int);

	ImGuiContext* getImGuiContext(void) const override;

	DebugRenderHandle renderDebugLine(const glm::vec3& start, const glm::vec3& end, const Gleam::Color& color = Gleam::COLOR_WHITE, bool has_depth = false);

private:
	struct DebugRenderData final
	{
		UniquePtr<Gleam::IProgramBuffers> program_buffers;
		UniquePtr<Gleam::IShader> pixel_shader;
		UniquePtr<Gleam::IShader> vert_shader;
		UniquePtr<Gleam::IProgram> program;
		UniquePtr<Gleam::IMesh> mesh;

		EA::Thread::SpinLock lock[2];

		Vector< UniquePtr<Gleam::IShaderResourceView> > instance_data_view[2] = {
			Vector< UniquePtr<Gleam::IShaderResourceView> >{ ProxyAllocator("Debug") },
			Vector< UniquePtr<Gleam::IShaderResourceView> >{ ProxyAllocator("Debug") }
		};

		Vector< UniquePtr<Gleam::IBuffer> > instance_data[2] = {
			Vector< UniquePtr<Gleam::IBuffer> >{ ProxyAllocator("Debug") },
			Vector< UniquePtr<Gleam::IBuffer> >{ ProxyAllocator("Debug") }
		};

		Vector< UniquePtr<DebugRenderInstance> > render_list[2] = { // 0 = no depth test, 1 = depth test
			Vector< UniquePtr<DebugRenderInstance> >{ ProxyAllocator("Debug") },
			Vector< UniquePtr<DebugRenderInstance> >{ ProxyAllocator("Debug") }
		};
	};

	struct DebugRenderJobData final
	{
		Gleam::ICommandList* cmd_list = nullptr;
		DebugRenderType type = DebugRenderType::Count;
		DebugManager* debug_mgr = nullptr;
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

	RenderManagerBase* _render_mgr = nullptr;
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

	DebugRenderJobData _render_job_data_cache[static_cast<size_t>(DebugRenderType::Count)];
	Gaff::JobData _job_data_cache[static_cast<size_t>(DebugRenderType::Count)];
	DebugRenderData _debug_data[static_cast<size_t>(DebugRenderType::Count)];

	static void HandleKeyboardCharacter(Gleam::IKeyboard*, uint32_t character);
	static void RenderDebugShape(uintptr_t thread_id_int, void* data);
	void removeDebugRender(const DebugRenderHandle& handle);

	SHIB_REFLECTION_CLASS_DECLARE(DebugManager);
	friend class DebugRenderHandle;
};

class DebugRenderPostCameraSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	DebugManager* _debug_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(DebugRenderPostCameraSystem);
};

class DebugRenderPreCameraSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(uintptr_t thread_id_int) override;

private:
	DebugManager* _debug_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(DebugRenderPostCameraSystem);
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

SHIB_REFLECTION_DECLARE(DebugRenderPostCameraSystem)
SHIB_REFLECTION_DECLARE(DebugRenderPreCameraSystem)
SHIB_REFLECTION_DECLARE(DebugManager)
SHIB_REFLECTION_DECLARE(DebugSystem)
