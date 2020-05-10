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

	static constexpr const char* const g_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
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

	static constexpr const char* const g_pixel_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
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

	struct VertexConstantBuffer final
	{
		float mvp[4][4];
	};
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

SHIB_REFLECTION_DEFINE_BEGIN(DebugRenderSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugRenderSystem)

SHIB_REFLECTION_DEFINE_BEGIN(DebugSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DebugManager)
SHIB_REFLECTION_CLASS_DEFINE(DebugRenderSystem)
SHIB_REFLECTION_CLASS_DEFINE(DebugSystem)

static void HandleKeyboardCharacter(Gleam::IKeyboard*, uint32_t character)
{
	ImGui::GetIO().AddInputCharacter(character);
}

static void HandleKeyboardInput(Gleam::IInputDevice*, int32_t key_code, float value)
{
	ImGui::GetIO().KeysDown[key_code] = value > 0.0f;

	if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTSHIFT)) ||
		(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTSHIFT))) {

		ImGui::GetIO().KeyShift = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTCONTROL)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTCONTROL))) {

		ImGui::GetIO().KeyCtrl = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTALT)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTALT))) {
		ImGui::GetIO().KeyAlt = value > 0.0f;

	} else if ((key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_LEFTWINDOWS)) ||
				(key_code == static_cast<int32_t>(Gleam::KeyCode::KEY_RIGHTWINDOWS))) {

		ImGui::GetIO().KeySuper = value > 0.0f;
	}
}

static void HandleMouseInput(Gleam::IInputDevice*, int32_t mouse_event, float value)
{
	if (mouse_event < static_cast<int32_t>(Gleam::MouseCode::MOUSE_BUTTON_COUNT)) {
		if (mouse_event < 5) {
			ImGui::GetIO().MouseDown[mouse_event] = (value > 0.0f);
		}

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_WHEEL_HORIZ)) {
		ImGui::GetIO().MouseWheelH = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_WHEEL_VERT)) {
		ImGui::GetIO().MouseWheel = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_POS_X)) {
		ImGui::GetIO().MousePos.x = value;

	} else if (mouse_event == static_cast<int32_t>(Gleam::MouseCode::MOUSE_POS_Y)) {
		ImGui::GetIO().MousePos.y = value;
	}
}

bool DebugManager::initAllModulesLoaded(void)
{
	IMGUI_CHECKVERSION();

	if (!ImGui::CreateContext()) {
		// $TODO: Log error.
		return false;
	}

	ImGui::StyleColorsDark();

	_render_mgr = &GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	const Gleam::IWindow* const window = _render_mgr->getWindow("main");
	_main_output = _render_mgr->getOutput("main");

	GAFF_ASSERT(_main_output);
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

	_time = &GetApp().getManagerTFast<GameTimeManager>().getRealTime();

	const auto* const devices = _render_mgr->getDevicesByTag("main");
	GAFF_ASSERT(devices && devices->size() == 1);

	_main_device = devices->front();

	// Shaders.
	_vertex_shader.reset(_render_mgr->createShader());

	if (!_vertex_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_vertex_shader->initVertexSource(*_main_device, g_vertex_shader[static_cast<size_t>(_render_mgr->getRendererType())])) {
		// $TODO: Log error.
		return false;
	}

	_pixel_shader.reset(_render_mgr->createShader());

	if (!_pixel_shader) {
		// $TODO: Log error.
		return false;
	}

	if (!_pixel_shader->initPixelSource(*_main_device, g_pixel_shader[static_cast<size_t>(_render_mgr->getRendererType())])) {
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


	// Blend State.
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
			0,
			Gleam::ILayout::PerDataType::Vertex
		},
		{
			Gleam::ILayout::Semantic::Color,
			0,
			Gleam::ITexture::Format::RGBA_8_UNORM,
			1,
			0,
			Gleam::ILayout::PerDataType::Vertex
		},
		{
			Gleam::ILayout::Semantic::TexCoord,
			0,
			Gleam::ITexture::Format::RG_32_F,
			2,
			0,
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

	return true;
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

	//ImGui::NewFrame();
}

void DebugManager::render(void)
{
	//ImGui::Render();
	const ImDrawData* const draw_data = ImGui::GetDrawData();

	// Avoid rendering when minimized
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
		return;
	}

	_mesh->setIndexCount(draw_data->TotalIdxCount);

	if (const size_t size = (_vertex_buffer) ? _vertex_buffer->getSize() : 0;
		size < (draw_data->TotalVtxCount * sizeof(ImDrawVert))) {

		_mesh->clear();

		_vertex_buffer.reset(_render_mgr->createBuffer());

		if (!_vertex_buffer) {
			// $TODO: Log error periodic.
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
			return;
		}

		_mesh->setIndiceBuffer(_index_buffer.get());
		_mesh->addBuffer(_vertex_buffer.get());
	}

	if (const size_t size = (_index_buffer) ? _index_buffer->getSize() : 0;
		size < (draw_data->TotalIdxCount * sizeof(ImDrawIdx))) {

		_mesh->clear();

		_index_buffer.reset(_render_mgr->createBuffer());

		if (!_index_buffer) {
			// $TODO: Log error periodic.
			return;
		}

		const Gleam::IBuffer::Settings settings = {
			nullptr,
			draw_data->TotalIdxCount * sizeof(ImDrawIdx),
			static_cast<int32_t>(sizeof(ImDrawIdx)),
			static_cast<int32_t>(sizeof(ImDrawIdx)),
			Gleam::IBuffer::Type::VertexData,
			Gleam::IBuffer::MapType::Write,
			true
		};

		if (!_index_buffer->init(*_main_device, settings)) {
			// $TODO: Log error periodic.
			return;
		}

		_mesh->setIndiceBuffer(_index_buffer.get());
		_mesh->addBuffer(_vertex_buffer.get());
	}

	ImDrawVert* verts = reinterpret_cast<ImDrawVert*>(_vertex_buffer->map(*_main_device));
	ImDrawIdx* indices = reinterpret_cast<ImDrawIdx*>(_index_buffer->map(*_main_device));

	if (!verts) {
		// $TODO: Log error periodic.
		return;
	}

	if (!indices) {
		// $TODO: Log error periodic.
		_vertex_buffer->unmap(*_main_device);
		return;
	}

	for (int32_t i = 0; i < draw_data->CmdListsCount; ++i) {
		const ImDrawList* const cmd_list = draw_data->CmdLists[i];

		memcpy(verts, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
		memcpy(indices, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));

		verts += cmd_list->VtxBuffer.Size;
		indices += cmd_list->IdxBuffer.Size;
	}

	_vertex_buffer->unmap(*_main_device);
	_index_buffer->unmap(*_main_device);

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

	void* const vert_const_data = _vert_constant_buffer->map(*_main_device);

	if (!vert_const_data) {
		// $TODO: Log error periodic.
		return;
	}

	memcpy(vert_const_data, mvp, sizeof(mvp));
	_vert_constant_buffer->unmap(*_main_device);

	_program_buffers->clearResourceViews();
	_program_buffers->addResourceView(Gleam::IShader::Type::Pixel, _font_srv.get());

	_main_output->getRenderTarget().bind(*_main_device);
	_program->bind(*_main_device);
	_program_buffers->bind(*_main_device);
	_blend_state->bind(*_main_device);
	_layout->bind(*_main_device);

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

				_program_buffers->bind(*_main_device);
			}

			_mesh->render(*_main_device, index_offset_start + cmd->IdxOffset, vert_offset_start + cmd->VtxOffset);

			index_offset_start += cmd_list->IdxBuffer.Size;
			vert_offset_start += cmd_list->VtxBuffer.Size;
		}
	}

	// submit command list
}

ImGuiContext* DebugManager::getImGuiContext(void) const
{
	return ImGui::GetCurrentContext();
}



bool DebugRenderSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();
	return true;
}

void DebugRenderSystem::update(void)
{
	_debug_mgr->render();
}

bool DebugSystem::init(void)
{
	_debug_mgr = &GetApp().getManagerTFast<DebugManager>();
	return true;
}

void DebugSystem::update(void)
{
	_debug_mgr->update();
}

NS_END
