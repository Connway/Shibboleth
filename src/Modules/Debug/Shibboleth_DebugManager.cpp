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

#include "Shibboleth_DebugManager.h"
#include <Shibboleth_RenderManagerBase.h>
#include <Shibboleth_InputManager.h>
#include <Shibboleth_GameTime.h>
#include <imgui.h>

namespace
{
	static Shibboleth::ProxyAllocator g_allocator;

	static void* ImGuiAlloc(size_t size, void*)
	{
		return SHIB_ALLOC(size, g_allocator);
	}

	static void ImGuiFree(void* ptr, void*)
	{
		SHIB_FREE(ptr, g_allocator);
	}

	static constexpr const char* const g_imgui_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			cbuffer vertexBuffer : register(b0)
			{
				float4x4 ProjectionMatrix;
			};

			struct VS_INPUT
			{
				float2 pos : POSITION;
				float4 col : COLOR0;
				float2 uv  : TEXCOORD0;
			};

			struct PS_INPUT
			{
				float4 pos : SV_POSITION;
				float4 col : COLOR0;
				float2 uv  : TEXCOORD0;
			};

			PS_INPUT VertexMain(VS_INPUT input)
			{
				PS_INPUT output;
				output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
				output.col = input.col;
				output.uv  = input.uv;
				return output;
			}
		)",
		R"(
		)"
	};

	static constexpr const char* const g_imgui_pixel_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			struct PS_INPUT
			{
				float4 pos : SV_POSITION;
				float4 col : COLOR0;
				float2 uv  : TEXCOORD0;
			};

			Texture2D texture0;
			sampler sampler0;

			float4 PixelMain(PS_INPUT input) : SV_Target
			{
				float4 out_col = input.col * texture0.Sample(sampler0, input.uv); 
				return out_col; 
			}
		)",
		R"(
		)"
	};

	static constexpr const char* const g_line_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			cbuffer InstanceBuffer
			{
				matrix projection_matrix;
			};

			struct InstanceData
			{
				float3 color;
				float3 start;
				float3 end;
			};

			struct VertexInputType
			{
				uint instance_id : SV_InstanceID;
				uint vertex_id : SV_VertexID;
			};

			struct PixelInputType
			{
				float4 position : SV_POSITION;
				float3 color : COLOR0;
			};

			StructuredBuffer<InstanceData> instance_data;

			PixelInputType VertexMain(VertexInputType input)
			{
				PixelInputType output;

				float vert_scale = float(input.vertex_id);

				float3 line_pos = instance_data[input.instance_id].start + instance_data[input.instance_id].end * float3(vert_scale, vert_scale, vert_scale);
				output.position = mul(projection_matrix, float4(line_pos, 1.0));
				output.color = instance_data[input.instance_id].color;

				return output;
			}
		)",
		R"(
		)"
	};

	static constexpr const char* const g_debug_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			struct InstanceData
			{
				matrix mvp;
				float3 color;
			};

			struct VertexInputType
			{
				float3 position : POSITION;
				uint instance_id : SV_InstanceID;
			};

			struct PixelInputType
			{
				float4 position : SV_POSITION;
				float3 color : COLOR0;
			};

			StructuredBuffer<InstanceData> instance_data;

			PixelInputType VertexMain(VertexInputType input)
			{
				PixelInputType output;

				output.position = mul(instance_data[input.instance_id].mvp, float4(input.position, 1.0));
				output.color = instance_data[input.instance_id].color;

				return output;
			}
		)",
		R"(
		)"
	};
	static constexpr const char* const g_debug_pixel_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			struct PixelInputType
			{
				float4 position : SV_POSITION;
				float3 color : COLOR0;
			};

			float4 PixelMain(PixelInputType input) : SV_TARGET0
			{
				return float4(input.color, 1.0);
			}
		)",
		R"(
		)"
	};

	struct VertexConstantBuffer final
	{
		float mvp[4][4];
	};

	static constexpr int32_t k_num_instances_per_buffer = 128;
}

GAFF_STATIC_FILE_FUNC
{
	ImGui::SetAllocatorFunctions(ImGuiAlloc, ImGuiFree);
}

SHIB_REFLECTION_DEFINE_BEGIN(DebugManager)
	.BASE(IDebugManager)
	.base<IManager>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugManager)

SHIB_REFLECTION_DEFINE_BEGIN(DebugRenderPostCameraSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugRenderPostCameraSystem)

SHIB_REFLECTION_DEFINE_BEGIN(DebugRenderPreCameraSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugRenderPreCameraSystem)

SHIB_REFLECTION_DEFINE_BEGIN(DebugSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DebugRenderPostCameraSystem)
SHIB_REFLECTION_CLASS_DEFINE(DebugRenderPreCameraSystem)
SHIB_REFLECTION_CLASS_DEFINE(DebugManager)
SHIB_REFLECTION_CLASS_DEFINE(DebugSystem)

static void HandleKeyboardInput(Gleam::IInputDevice*, int32_t key_code, float value)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key_code] = value > 0.0f;

	if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTSHIFT)) ||
		(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTSHIFT))) {

		io.KeyShift = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTCONTROL)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTCONTROL))) {

		io.KeyCtrl = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTALT)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTALT))) {
		io.KeyAlt = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTWINDOWS)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTWINDOWS))) {

		io.KeySuper = value > 0.0f;
	}
}

static void HandleMouseInput(Gleam::IInputDevice*, int32_t mouse_event, float value)
{
	ImGuiIO& io = ImGui::GetIO();

	if (mouse_event < static_cast<int32_t>(Gleam::MouseCode::MOUSE_BUTTON_COUNT)) {
		if (mouse_event < 5) {
			io.MouseDown[mouse_event] = (value > 0.0f);
		}

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_WHEEL_HORIZ)) {
		io.MouseWheelH = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_WHEEL_VERT)) {
		io.MouseWheel = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_POS_X)) {
		io.MousePos.x = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_POS_Y)) {
		io.MousePos.y = value;
	}
}

void DebugManager::HandleKeyboardCharacter(Gleam::IKeyboard*, uint32_t character)
{
	DebugManager& dbg_mgr = GetApp().getManagerTFast<DebugManager>();
	dbg_mgr._character_buffer[dbg_mgr._char_buffer_cache_index].emplace_back(character);
}

void DebugManager::RenderDebugShape(uintptr_t /*thread_id_int*/, void* /*data*/)
{
	//DebugRenderJobData& job_data = *reinterpret_cast<DebugRenderJobData*>(data);

	//const auto* const devices = job_data.debug_mgr->_render_mgr->getDevicesByTag("main");
	//GAFF_ASSERT(devices && devices->size() > 0);

	//const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	//Gleam::IRenderDevice* const rd = job_data.debug_mgr->_render_mgr->getDeferredDevice(*devices->front(), thread_id);

	//for (int32_t i = 0; i < 2; ++i) {
	//	auto& debug_data = job_data.debug_mgr->_debug_data.instance_data[static_cast<int32_t>(job_data.type)];

	//	const int32_t num_buffers_needed = static_cast<int32_t>(
	//		ceilf(static_cast<float>(debug_data.render_list[i].size()) / k_num_instances_per_buffer)
	//	);

	//	// Make sure we have enough buffers on the GPU to hold our instance data.
	//	while (static_cast<int32_t>(debug_data.instance_data[i].size()) < num_buffers_needed) {
	//		Gleam::IBuffer* const buffer = job_data.debug_mgr->_render_mgr->createBuffer();

	//		if (!buffer) {
	//			// $TODO: Log error.
	//			continue;
	//		}

	//		const Gleam::IBuffer::Settings settings = {
	//			nullptr,
	//			sizeof(glm::mat4x3) * k_num_instances_per_buffer,
	//			static_cast<int32_t>(sizeof(glm::mat4x3) + sizeof(float) * 3),
	//			static_cast<int32_t>(sizeof(glm::mat4x3) + sizeof(float) * 3),
	//			Gleam::IBuffer::Type::StructuredData,
	//			Gleam::IBuffer::MapType::Write,
	//			true
	//		};

	//		if (!buffer->init(*job_data.debug_mgr->_main_device, settings)) {
	//			SHIB_FREET(buffer, g_allocator);
	//			// $TODO: Log error.
	//			continue;
	//		}

	//		Gleam::IShaderResourceView* const view = job_data.debug_mgr->_render_mgr->createShaderResourceView();

	//		if (!view) {
	//			SHIB_FREET(buffer, g_allocator);
	//			// $TODO: Log error.
	//			continue;
	//		}

	//		if (!view->init(*job_data.debug_mgr->_main_device, buffer)) {
	//			SHIB_FREET(buffer, g_allocator);
	//			SHIB_FREET(view, g_allocator);
	//			// $TODO: Log error.
	//			continue;
	//		}

	//		debug_data.instance_data_view[i].emplace_back(view);
	//		debug_data.instance_data[i].emplace_back(buffer);
	//	}

	//	// $TODO: Bind depth stencil state.
	//	// $TODO: Bind raster state.
	//	debug_data.program->bind(*rd);

	//	int32_t total_items = static_cast<int32_t>(debug_data.render_list[i].size());
	//	int32_t curr_index = 0;

	//	for (int32_t j = 0; j < static_cast<int32_t>(debug_data.instance_data[i].size()); ++j) {
	//		const int32_t count = Gaff::Min(total_items, k_num_instances_per_buffer);
	//		int8_t* mapped_buffer = reinterpret_cast<int8_t*>(debug_data.instance_data[i][j]->map(*rd));

	//		if (!mapped_buffer) {
	//			// $TODO: Log error.
	//			continue;
	//		}

	//		// Update instance buffers(s).
	//		for (int32_t k = 0; k < count; ++k) {
	//			const DebugRenderInstance& inst = *debug_data.render_list[i][curr_index];

	//			if (job_data.type == DebugRenderType::Line) {
	//				memcpy(mapped_buffer, &inst.color, sizeof(glm::vec3)); // We don't care about alpha.
	//				memcpy(mapped_buffer + sizeof(glm::vec3), &inst.transform.getTranslation(), sizeof(glm::vec3));
	//				memcpy(mapped_buffer + 2 * sizeof(glm::vec3), &inst.transform.getScale(), sizeof(glm::vec3));

	//			} else {
	//				const glm::mat4x3 transform = inst.transform.toMatrix();

	//				memcpy(mapped_buffer, &transform, sizeof(glm::mat4x3));
	//				memcpy(mapped_buffer + sizeof(glm::mat4x3), &inst.color, sizeof(glm::vec3)); // We don't care about alpha.
	//			}

	//			mapped_buffer += sizeof(glm::mat4x3) + sizeof(float) * 3;
	//			++curr_index;
	//		}

	//		debug_data.instance_data[i][j]->unmap(*rd);

	//		debug_data.program_buffers->clearResourceViews();
	//		debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j].get());
	//		debug_data.program_buffers->bind(*rd);

	//		if (job_data.type == DebugRenderType::Line) {
	//			rd->renderLineNoVertexInputInstanced(1, count);
	//		} else {
	//			debug_data.mesh->renderInstanced(*rd, count);
	//		}

	//		total_items -= k_num_instances_per_buffer;
	//	}
	//}
}

void DebugManager::SetupModuleToUseImGui(void)
{
	const DebugManager& dbg_mgr = GetApp().getManagerTFast<DebugManager>();
	ImGui::SetCurrentContext(dbg_mgr.getImGuiContext());
}

bool DebugManager::initAllModulesLoaded(void)
{
	_time = &GetApp().getManagerTFast<GameTimeManager>().getRealTime();
	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	_main_output = _render_mgr->getOutput("main");

	GAFF_ASSERT(_main_output);

	const auto* const devices = _render_mgr->getDevicesByTag("main");
	GAFF_ASSERT(devices && devices->size() == 1);

	_main_device = devices->front();


	bool success = true;
	success = initDebugRender() && success;
	success = initImGui() && success;

	return success;
}

void DebugManager::update(void)
{
	const glm::ivec2& size = _main_output->getSize();

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = _time->getDeltaFloat();
	io.DisplaySize.x = static_cast<float>(size.x);
	io.DisplaySize.y = static_cast<float>(size.y);

	const ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();

	// Update mouse cursor.
	if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
		if (_prev_cursor != mouse_cursor) {
			_prev_cursor = mouse_cursor;

			if (mouse_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor) {
				Gleam::SetCursor(Gleam::Cursor::None);

			} else {
				Gleam::Cursor cursor = Gleam::Cursor::Arrow;

				switch (mouse_cursor)
				{
					case ImGuiMouseCursor_Arrow:
						cursor = Gleam::Cursor::Arrow;
						break;

					case ImGuiMouseCursor_TextInput:
						cursor = Gleam::Cursor::IBeam;
						break;

					case ImGuiMouseCursor_ResizeAll:
						cursor = Gleam::Cursor::SizeAll;
						break;

					case ImGuiMouseCursor_ResizeEW:
						cursor = Gleam::Cursor::SizeWE;
						break;

					case ImGuiMouseCursor_ResizeNS:
						cursor = Gleam::Cursor::SizeNS;
						break;

					case ImGuiMouseCursor_ResizeNESW:
						cursor = Gleam::Cursor::SizeNESW;
						break;

					case ImGuiMouseCursor_ResizeNWSE:
						cursor = Gleam::Cursor::SizeNWSE;
						break;

					case ImGuiMouseCursor_Hand:
						cursor = Gleam::Cursor::Hand;
						break;

					case ImGuiMouseCursor_NotAllowed:
						cursor = Gleam::Cursor::No;
						break;
				}

				Gleam::SetCursor(cursor);
			}
		}
	}

	const int32_t char_buf_index = _char_buffer_cache_index;
	_char_buffer_cache_index = (_char_buffer_cache_index + 1) % 2;

	for (uint32_t character : _character_buffer[char_buf_index]) {
		ImGui::GetIO().AddInputCharacter(character);
	}

	_character_buffer[char_buf_index].clear();

	ImGui::NewFrame();
}

void DebugManager::renderPostCamera(uintptr_t thread_id_int)
{
	ImGui::Render();
	const ImDrawData* const draw_data = ImGui::GetDrawData();

	// Avoid rendering when minimized
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
		_render_cache_index = (_render_cache_index + 1) % 2;
		return;
	}

	if (draw_data->TotalIdxCount <= 0 || draw_data->TotalVtxCount <= 0) {
		_render_cache_index = (_render_cache_index + 1) % 2;
		return;
	}

	bool buffers_changed = false;

	if (const size_t size = (_vertex_buffer) ? _vertex_buffer->getSize() : 0;
		size < (draw_data->TotalVtxCount * sizeof(ImDrawVert))) {

		_vertex_buffer.reset(_render_mgr->createBuffer());
		buffers_changed = true;

		if (!_vertex_buffer) {
			// $TODO: Log error periodic.
			_render_cache_index = (_render_cache_index + 1) % 2;
			return;
		}

		const Gleam::IBuffer::Settings settings = {
			nullptr,
			draw_data->TotalVtxCount * sizeof(ImDrawVert),
			static_cast<int32_t>(sizeof(ImDrawVert)),
			static_cast<int32_t>(sizeof(ImDrawVert)),
			Gleam::IBuffer::Type::VertexData,
			Gleam::IBuffer::MapType::Write,
			true
		};

		if (!_vertex_buffer->init(*_main_device, settings)) {
			// $TODO: Log error periodic.
			_render_cache_index = (_render_cache_index + 1) % 2;
			return;
		}
	}

	if (const size_t size = (_index_buffer) ? _index_buffer->getSize() : 0;
		size < (draw_data->TotalIdxCount * sizeof(ImDrawIdx))) {

		_index_buffer.reset(_render_mgr->createBuffer());
		buffers_changed = true;

		if (!_index_buffer) {
			// $TODO: Log error periodic.
			_render_cache_index = (_render_cache_index + 1) % 2;
			return;
		}

		const Gleam::IBuffer::Settings settings = {
			nullptr,
			draw_data->TotalIdxCount * sizeof(ImDrawIdx),
			static_cast<int32_t>(sizeof(ImDrawIdx)),
			static_cast<int32_t>(sizeof(ImDrawIdx)),
			Gleam::IBuffer::Type::IndexData,
			Gleam::IBuffer::MapType::Write,
			true
		};

		if (!_index_buffer->init(*_main_device, settings)) {
			// $TODO: Log error periodic.
			_render_cache_index = (_render_cache_index + 1) % 2;
			return;
		}
	}

	if (buffers_changed) {
		_mesh->clear();

		if (_vertex_buffer) {
			_mesh->addBuffer(_vertex_buffer.get());
		}

		_mesh->setIndiceBuffer(_index_buffer.get());
	}

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	Gleam::IRenderDevice* const deferred_device = _render_mgr->getDeferredDevice(*_main_device, thread_id);

	ImDrawVert* verts = reinterpret_cast<ImDrawVert*>(_vertex_buffer->map(*deferred_device));
	ImDrawIdx* indices = reinterpret_cast<ImDrawIdx*>(_index_buffer->map(*deferred_device));

	if (!verts) {
		// $TODO: Log error periodic.
		_render_cache_index = (_render_cache_index + 1) % 2;
		return;
	}

	if (!indices) {
		// $TODO: Log error periodic.
		_vertex_buffer->unmap(*deferred_device);
		_render_cache_index = (_render_cache_index + 1) % 2;
		return;
	}

	for (int32_t i = 0; i < draw_data->CmdListsCount; ++i) {
		const ImDrawList* const cmd_list = draw_data->CmdLists[i];

		memcpy(indices, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		memcpy(verts, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));

		indices += cmd_list->IdxBuffer.Size;
		verts += cmd_list->VtxBuffer.Size;
	}

	_vertex_buffer->unmap(*deferred_device);
	_index_buffer->unmap(*deferred_device);

	const float L = draw_data->DisplayPos.x;
	const float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	const float T = draw_data->DisplayPos.y;
	const float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	const float mvp[4][4] =
	{
		{ 2.0f / (R - L),		0.0f,				0.0f,	0.0f },
		{ 0.0f,					2.0f / (T - B),		0.0f,	0.0f },
		{ 0.0f,					0.0f,				0.5f,	0.0f },
		{ (R + L) / (L - R),	(T + B) / (B - T),	0.5f,	1.0f },
	};

	void* const vert_const_data = _vert_constant_buffer->map(*deferred_device);

	if (!vert_const_data) {
		// $TODO: Log error periodic.
		_render_cache_index = (_render_cache_index + 1) % 2;
		return;
	}

	memcpy(vert_const_data, mvp, sizeof(mvp));
	_vert_constant_buffer->unmap(*deferred_device);

	_program_buffers->clearResourceViews();
	_program_buffers->addResourceView(Gleam::IShader::Type::Pixel, _font_srv.get());

	_main_output->getRenderTarget().bind(*deferred_device);
	_depth_stencil_state->bind(*deferred_device);
	_raster_state->bind(*deferred_device);
	_blend_state->bind(*deferred_device);
	_program->bind(*deferred_device);
	_program_buffers->bind(*deferred_device);
	_layout->bind(*deferred_device);

	int32_t index_offset_start = 0;
	int32_t vert_offset_start = 0;

	for (int32_t i = 0; i < draw_data->CmdListsCount; ++i) {
		const ImDrawList* const cmd_list = draw_data->CmdLists[i];

		for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; ++j) {
			const ImDrawCmd* const cmd = &cmd_list->CmdBuffer[j];

			if (_program_buffers->getResourceView(Gleam::IShader::Type::Pixel, 0) != cmd->TextureId) {
				_program_buffers->popResourceView(Gleam::IShader::Type::Pixel);

				_program_buffers->addResourceView(
					Gleam::IShader::Type::Pixel,
					reinterpret_cast<Gleam::IShaderResourceView*>(cmd->TextureId)
				);

				_program_buffers->bind(*deferred_device);
			}

			// Apply scissor/clipping rectangle
			const glm::ivec4 rect {
				static_cast<int32_t>(cmd->ClipRect.x - draw_data->DisplayPos.x),
				static_cast<int32_t>(cmd->ClipRect.y - draw_data->DisplayPos.y),
				static_cast<int32_t>(cmd->ClipRect.z - draw_data->DisplayPos.x),
				static_cast<int32_t>(cmd->ClipRect.w - draw_data->DisplayPos.y)
			};

			deferred_device->setScissorRect(rect);

			_mesh->setIndexCount(cmd->ElemCount);
			_mesh->render(*deferred_device, index_offset_start + cmd->IdxOffset, vert_offset_start + cmd->VtxOffset);
		}

		index_offset_start += cmd_list->IdxBuffer.Size;
		vert_offset_start += cmd_list->VtxBuffer.Size;
	}

	if (!deferred_device->finishCommandList(*_cmd_list[_render_cache_index])) {
		// $TODO: Log error periodic.
		_render_cache_index = (_render_cache_index + 1) % 2;
		return;
	}

	// Submit command
	auto& render_cmds = _render_mgr->getRenderCommands(*_main_device, _render_cache_index);

	render_cmds.lock.Lock();
	auto& cmd = render_cmds.command_list.emplace_back();
	cmd.cmd_list.reset(_cmd_list[_render_cache_index].get());
	cmd.owns_command = false;
	render_cmds.lock.Unlock();

	_render_cache_index = (_render_cache_index + 1) % 2;
}

void DebugManager::renderPreCamera(uintptr_t thread_id_int)
{
	GAFF_REF(thread_id_int);

	// Update instance data.

	for (int32_t i = 0; i < static_cast<int32_t>(DebugRenderType::Count); ++i) {
		//const auto& debug_data = _debug_data[i];

		//_main_output->getRenderTarget().bind(*deferred_device);
		//_depth_stencil_state->bind(*deferred_device);
		//_raster_state->bind(*deferred_device);
		//_blend_state->bind(*deferred_device);
		//_program->bind(*deferred_device);
		//_program_buffers->bind(*deferred_device);
		//_layout->bind(*deferred_device);

		if (i == static_cast<int32_t>(DebugRenderType::Line)) {
			//debug_data.program->bind(*deferred_device);
			//debug_data.program_buffers->bind(*deferred_device);
		}
	}
}

ImGuiContext* DebugManager::getImGuiContext(void) const
{
	return ImGui::GetCurrentContext();
}

DebugManager::DebugRenderHandle DebugManager::renderDebugLine(const glm::vec3& start, const glm::vec3& end, const Gleam::Color& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Line)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(start);
	instance->transform.setScale(end - start);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Line, has_depth);
}

void DebugManager::removeDebugRender(const DebugRenderHandle& handle)
{
	auto& render_list = _debug_data.instance_data[static_cast<int32_t>(handle._type)].render_list[handle._depth];
	const auto it = eastl::lower_bound(render_list.begin(), render_list.end(), handle._instance, [](const auto& lhs, auto rhs) -> bool { return lhs.get() < rhs; });

	if (it != render_list.end() && it->get() == handle._instance) {
		render_list.erase(it);
	}
}

bool DebugManager::initDebugRender(void)
{
	const size_t renderer_index = static_cast<size_t>(_render_mgr->getRendererType());

	// Init line shaders and program.
	_debug_data.line_vertex_shader.reset(_render_mgr->createShader());

	if (!_debug_data.line_vertex_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_debug_data.line_vertex_shader->initVertexSource(*_main_device, g_line_vertex_shader[renderer_index])) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.pixel_shader.reset(_render_mgr->createShader());

	if (!_debug_data.pixel_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_debug_data.pixel_shader->initPixelSource(*_main_device, g_debug_pixel_shader[renderer_index])) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.line_program.reset(_render_mgr->createProgram());

	if (!_debug_data.line_program) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.line_program->attach(_debug_data.line_vertex_shader.get());
	_debug_data.line_program->attach(_debug_data.pixel_shader.get());


	// Init regular shaders and program.
	_debug_data.vertex_shader.reset(_render_mgr->createShader());

	if (!_debug_data.vertex_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_debug_data.vertex_shader->initVertexSource(*_main_device, g_debug_vertex_shader[renderer_index])) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.program.reset(_render_mgr->createProgram());

	if (!_debug_data.program) {
		// $TODO: Log error.
		return false;
	}

	_debug_data.program->attach(_debug_data.vertex_shader.get());
	_debug_data.program->attach(_debug_data.pixel_shader.get());


	// Shared program buffers.
	_debug_data.program_buffers.reset(_render_mgr->createProgramBuffers());

	if (!_debug_data.program_buffers) {
		// $TODO: Log error.
		return false;
	}

	// $TODO: Init meshes.

	for (int32_t i = 0; i < static_cast<int32_t>(DebugRenderType::Count); ++i) {
		_debug_data.render_job_data_cache[i].type = static_cast<DebugRenderType>(i);
		_debug_data.render_job_data_cache[i].debug_mgr = this;
		_debug_data.job_data_cache[i].job_data = &_debug_data.render_job_data_cache[i];
		_debug_data.job_data_cache[i].job_func = RenderDebugShape;
	}

	return true;
}

bool DebugManager::initImGui(void)
{
	IMGUI_CHECKVERSION();

	if (!ImGui::CreateContext()) {
		// $TODO: Log error.
		return false;
	}

	ImGui::StyleColorsDark();

	const Gleam::IWindow* const window = _render_mgr->getWindow("main");
	GAFF_ASSERT(window);

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;	// We can honor GetMouseCursor() values (optional)
	//io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;	// We can honor io.WantSetMousePos requests (optional, rarely used)
	io.ImeWindowHandle = window->getPlatformHandle();
	io.BackendRendererName = "Gleam";
	io.BackendPlatformName = "Gleam";

	io.KeyMap[ImGuiKey_Tab] = static_cast<int32_t>(Gleam::KeyCode::KEY_TAB);
	io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int32_t>(Gleam::KeyCode::KEY_LEFT);
	io.KeyMap[ImGuiKey_RightArrow] = static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHT);
	io.KeyMap[ImGuiKey_UpArrow] = static_cast<int32_t>(Gleam::KeyCode::KEY_UP);
	io.KeyMap[ImGuiKey_DownArrow] = static_cast<int32_t>(Gleam::KeyCode::KEY_DOWN);
	io.KeyMap[ImGuiKey_PageUp] = static_cast<int32_t>(Gleam::KeyCode::KEY_PAGEUP);
	io.KeyMap[ImGuiKey_PageDown] = static_cast<int32_t>(Gleam::KeyCode::KEY_PAGEDOWN);
	io.KeyMap[ImGuiKey_Home] = static_cast<int32_t>(Gleam::KeyCode::KEY_HOME);
	io.KeyMap[ImGuiKey_End] = static_cast<int32_t>(Gleam::KeyCode::KEY_END);
	io.KeyMap[ImGuiKey_Insert] = static_cast<int32_t>(Gleam::KeyCode::KEY_INSERT);
	io.KeyMap[ImGuiKey_Delete] = static_cast<int32_t>(Gleam::KeyCode::KEY_DELETE);
	io.KeyMap[ImGuiKey_Backspace] = static_cast<int32_t>(Gleam::KeyCode::KEY_BACKSPACE);
	io.KeyMap[ImGuiKey_Space] = static_cast<int32_t>(Gleam::KeyCode::KEY_SPACE);
	io.KeyMap[ImGuiKey_Enter] = static_cast<int32_t>(Gleam::KeyCode::KEY_ENTER);
	io.KeyMap[ImGuiKey_Escape] = static_cast<int32_t>(Gleam::KeyCode::KEY_ESCAPE);
	io.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<int32_t>(Gleam::KeyCode::KEY_NUMPADENTER);
	io.KeyMap[ImGuiKey_A] = static_cast<int32_t>(Gleam::KeyCode::KEY_A);
	io.KeyMap[ImGuiKey_C] = static_cast<int32_t>(Gleam::KeyCode::KEY_C);
	io.KeyMap[ImGuiKey_V] = static_cast<int32_t>(Gleam::KeyCode::KEY_V);
	io.KeyMap[ImGuiKey_X] = static_cast<int32_t>(Gleam::KeyCode::KEY_X);
	io.KeyMap[ImGuiKey_Y] = static_cast<int32_t>(Gleam::KeyCode::KEY_Y);
	io.KeyMap[ImGuiKey_Z] = static_cast<int32_t>(Gleam::KeyCode::KEY_Z);

	InputManager& input = GetApp().getManagerTFast<InputManager>();
	input.getKeyboard()->addCharacterHandler(Gaff::Func(HandleKeyboardCharacter));
	input.getKeyboard()->addInputHandler(Gaff::Func(HandleKeyboardInput));
	input.getMouse()->addInputHandler(Gaff::Func(HandleMouseInput));

	// Command List
	_cmd_list[0].reset(_render_mgr->createCommandList());
	_cmd_list[1].reset(_render_mgr->createCommandList());

	if (!_cmd_list[0] || !_cmd_list[1]) {
		// $TODO: Log error.
		return false;
	}


	// Shaders
	_vertex_shader.reset(_render_mgr->createShader());

	if (!_vertex_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_vertex_shader->initVertexSource(*_main_device, g_imgui_vertex_shader[static_cast<size_t>(_render_mgr->getRendererType())])) {
		// $TODO: Log error.
		return false;
	}

	_pixel_shader.reset(_render_mgr->createShader());

	if (!_pixel_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_pixel_shader->initPixelSource(*_main_device, g_imgui_pixel_shader[static_cast<size_t>(_render_mgr->getRendererType())])) {
		// $TODO: Log error.
		return false;
	}

	_program.reset(_render_mgr->createProgram());

	if (!_program) {
		// $TODO: Log error.
		return false;
	}

	_program->attach(_vertex_shader.get());
	_program->attach(_pixel_shader.get());


	// Blend State
	_blend_state.reset(_render_mgr->createBlendState());

	if (!_blend_state) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::IBlendState::Settings blend_settings = {
		Gleam::IBlendState::BlendFactor::SourceAlpha,
		Gleam::IBlendState::BlendFactor::InverseSourceAlpha,
		Gleam::IBlendState::BlendOp::Add,
		Gleam::IBlendState::BlendFactor::InverseSourceAlpha,
		Gleam::IBlendState::BlendFactor::Zero,
		Gleam::IBlendState::BlendOp::Add,
		Gleam::IBlendState::ColorMask::All,
		true
	};

	if (!_blend_state->init(*_main_device, blend_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Raster State
	_raster_state.reset(_render_mgr->createRasterState());

	if (!_raster_state) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::IRasterState::Settings raster_settings = {
		0.0f,
		0.0f,
		0,
		true,
		false,
		true,
		false,
		false
	};

	if (!_raster_state->init(*_main_device, raster_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Depth Stencil State
	_depth_stencil_state.reset(_render_mgr->createDepthStencilState());

	if (!_depth_stencil_state) {
		// $TODO: Log error.
		return false;
	}

	Gleam::IDepthStencilState::Settings depth_stencil_settings;
	depth_stencil_settings.depth_func = Gleam::ComparisonFunc::Always;
	depth_stencil_settings.depth_test = false;

	if (!_depth_stencil_state->init(*_main_device, depth_stencil_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Layout
	_layout.reset(_render_mgr->createLayout());

	if (!_layout) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::ILayout::Description layout_description[] = {
		{
			Gleam::ILayout::Semantic::Position,
			0,
			Gleam::ITexture::Format::RG_32_F,
			0,
			static_cast<int32_t>((size_t)(&((ImDrawVert*)0)->pos)),
			Gleam::ILayout::PerDataType::Vertex
		},
		{
			Gleam::ILayout::Semantic::TexCoord,
			0,
			Gleam::ITexture::Format::RG_32_F,
			0,
			static_cast<int32_t>((size_t)(&((ImDrawVert*)0)->uv)),
			Gleam::ILayout::PerDataType::Vertex
		},
		{
			Gleam::ILayout::Semantic::Color,
			0,
			Gleam::ITexture::Format::RGBA_8_UNORM,
			0,
			static_cast<int32_t>((size_t)(&((ImDrawVert*)0)->col)),
			Gleam::ILayout::PerDataType::Vertex
		}
	};

	if (!_layout->init(*_main_device, layout_description, ARRAY_SIZE(layout_description), *_vertex_shader)) {
		// $TODO: Log error.
		return false;
	}


	// Constant Buffer
	_vert_constant_buffer.reset(_render_mgr->createBuffer());

	if (!_vert_constant_buffer) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::IBuffer::Settings constant_buffer_settings = {
		nullptr,
		sizeof(VertexConstantBuffer),
		0,
		sizeof(VertexConstantBuffer),
		Gleam::IBuffer::Type::ShaderConstantData,
		Gleam::IBuffer::MapType::Write,
		true
	};

	if (!_vert_constant_buffer->init(*_main_device, constant_buffer_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Font Texture
	_font_srv.reset(_render_mgr->createShaderResourceView());
	_font_texture.reset(_render_mgr->createTexture());

	if (!_font_texture) {
		// $TODO: Log error.
		return false;
	}

	if (!_font_srv) {
		// $TODO: Log error.
		return false;
	}

	int32_t bytes_per_pixel = 0;
	uint8_t* font = nullptr;
	int32_t height = 0;
	int32_t width = 0;

	io.Fonts->GetTexDataAsRGBA32(&font, &width, &height, &bytes_per_pixel);
	GAFF_ASSERT(bytes_per_pixel == 4);
	GAFF_REF(bytes_per_pixel);

	if (!_font_texture->init2D(*_main_device, width, height, Gleam::ITexture::Format::RGBA_8_UNORM, 1, font)) {
		// $TODO: Log error.
		return false;
	}

	if (!_font_srv->init(*_main_device, _font_texture.get())) {
		// $TODO: Log error.
		return false;
	}

	io.Fonts->TexID = _font_srv.get();


	// Sampler
	_sampler.reset(_render_mgr->createSamplerState());

	if (!_sampler) {
		// $TODO: Log error.
		return false;
	}

	const Gleam::ISamplerState::Settings sampler_settings = {
		Gleam::ISamplerState::Filter::LinearLinearLinear,
		Gleam::ISamplerState::Wrap::Repeat,
		Gleam::ISamplerState::Wrap::Repeat,
		Gleam::ISamplerState::Wrap::Repeat,
		0.0f,
		0.0f,
		0.0f,
		1,
		Gleam::COLOR_BLACK,
		Gleam::ComparisonFunc::Always
	};

	if (!_sampler->init(*_main_device, sampler_settings)) {
		// $TODO: Log error.
		return false;
	}


	// Program Buffers
	_program_buffers.reset(_render_mgr->createProgramBuffers());

	if (!_program_buffers) {
		// $TODO: Log error.
		return false;
	}

	_program_buffers->addConstantBuffer(Gleam::IShader::Type::Vertex, _vert_constant_buffer.get());
	_program_buffers->addResourceView(Gleam::IShader::Type::Pixel, _font_srv.get());
	_program_buffers->addSamplerState(Gleam::IShader::Type::Pixel, _sampler.get());

	_mesh.reset(_render_mgr->createMesh());

	if (!_mesh) {
		// $TODO: Log error.
		return false;
	}

	_mesh->setTopologyType(Gleam::IMesh::TopologyType::TriangleList);
	return true;
}



bool DebugRenderPostCameraSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();
	return true;
}

void DebugRenderPostCameraSystem::update(uintptr_t thread_id_int)
{
	_debug_mgr->renderPostCamera(thread_id_int);
}

bool DebugRenderPreCameraSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();
	return true;
}

void DebugRenderPreCameraSystem::update(uintptr_t thread_id_int)
{
	_debug_mgr->renderPreCamera(thread_id_int);
}

bool DebugSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();
	return true;
}

void DebugSystem::update(uintptr_t /*thread_id_int*/)
{
	_debug_mgr->update();
}

NS_END
