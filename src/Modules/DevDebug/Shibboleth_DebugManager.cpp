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

#include "Shibboleth_DebugManager.h"
#include "Shibboleth_DebugAttributes.h"
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_InputManager.h>
#include <Shibboleth_GameTime.h>
#include <Shibboleth_AppUtils.h>
#include <Gleam_MeshGeneration.h>
#include <Gleam_RenderOutput.h>
#include <Gleam_Window.h>
#include <Gaff_Function.h>
#include <Gaff_Math.h>
#include <imgui.h>

#ifndef GLM_ENABLE_EXPERIMENTAL
	#define GLM_ENABLE_EXPERIMENTAL
#endif

#include <gtx/euler_angles.hpp>
#include <gtx/transform.hpp>

namespace
{
	static Shibboleth::ProxyAllocator g_allocator{ "Debug" };

	static void* ImGuiAlloc(size_t size, void*)
	{
		return SHIB_ALLOC(size, g_allocator);
	}

	static void ImGuiFree(void* ptr, void*)
	{
		SHIB_FREE(ptr, g_allocator);
	}

	class ModelMapComparison
	{
	public:
		bool operator()(const Shibboleth::ResourcePtr<Shibboleth::ModelResource>& lhs, const Shibboleth::ModelResource* rhs) const
		{
			return lhs.get() < rhs;
		}

		bool operator()(const Shibboleth::ModelResource* lhs, const Shibboleth::ResourcePtr<Shibboleth::ModelResource>& rhs) const
		{
			return lhs < rhs.get();
		}
	};

	static constexpr const char* const g_imgui_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			cbuffer vertexBuffer : register(b0)
			{
				float4x4 projection_matrix;
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
				output.pos = mul(projection_matrix, float4(input.pos.xy, 0.f, 1.f));
				output.col = input.col;
				output.uv  = input.uv;
				return output;
			}
		)",
		R"(
			$TODO: NEED TO IMPLEMENT!
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
				float4x4 view_proj_matrix;
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

				float3 line_pos = instance_data[input.instance_id].start +
					instance_data[input.instance_id].end * float3(vert_scale, vert_scale, vert_scale);

				output.position = mul(view_proj_matrix, float4(line_pos, 1.0));
				output.color = instance_data[input.instance_id].color;

				return output;
			}
		)",
		R"(
			$TODO: NEED TO IMPLEMENT!
		)"
	};

	static constexpr const char* const g_debug_vertex_shader[static_cast<size_t>(Gleam::RendererType::Count)] =
	{
		R"(
			struct InstanceData
			{
				float4x4 mvp;
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
			$TODO: NEED TO IMPLEMENT!
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
			$TODO: NEED TO IMPLEMENT!
		)"
	};

	struct VertexConstantBuffer final
	{
		float mvp[4][4];
	};

	static constexpr int32_t k_num_instances_per_buffer = 32;

	static constexpr int32_t k_key_mapping[GLFW_KEY_LAST + 1] =
	{
		ImGuiKey_None, /* 0 */ ImGuiKey_None, /* 1 */ ImGuiKey_None, /* 2 */ ImGuiKey_None, /* 3 */ ImGuiKey_None, /* 4 */
		ImGuiKey_None, /* 5 */ ImGuiKey_None, /* 6 */ ImGuiKey_None, /* 7 */ ImGuiKey_None, /* 8 */ ImGuiKey_None, /* 9 */
		ImGuiKey_None, /* 10 */ ImGuiKey_None, /* 11 */ ImGuiKey_None, /* 12 */ ImGuiKey_None, /* 13 */ ImGuiKey_None, /* 14 */
		ImGuiKey_None, /* 15 */ ImGuiKey_None, /* 16 */ ImGuiKey_None, /* 17 */ ImGuiKey_None, /* 18 */ ImGuiKey_None, /* 19 */
		ImGuiKey_None, /* 20 */ ImGuiKey_None, /* 21 */ ImGuiKey_None, /* 22 */ ImGuiKey_None, /* 23 */ ImGuiKey_None, /* 24 */
		ImGuiKey_None, /* 25 */ ImGuiKey_None, /* 26 */ ImGuiKey_None, /* 27 */ ImGuiKey_None, /* 28 */ ImGuiKey_None, /* 29 */
		ImGuiKey_None, /* 30 */ ImGuiKey_None, /* 31 */
		ImGuiKey_Space,
		ImGuiKey_None, /* 33 */ ImGuiKey_None, /* 34 */ ImGuiKey_None, /* 35 */ ImGuiKey_None, /* 36 */ ImGuiKey_None, /* 37 */
		ImGuiKey_None, /* 38 */
		ImGuiKey_Apostrophe,
		ImGuiKey_None, /* 40 */ ImGuiKey_None, /* 41 */ ImGuiKey_None, /* 42 */ ImGuiKey_None, /* 43 */
		ImGuiKey_Comma,
		ImGuiKey_Minus,
		ImGuiKey_Period,
		ImGuiKey_Slash,
		ImGuiKey_0,
		ImGuiKey_1,
		ImGuiKey_2,
		ImGuiKey_3,
		ImGuiKey_4,
		ImGuiKey_5,
		ImGuiKey_6,
		ImGuiKey_7,
		ImGuiKey_8,
		ImGuiKey_9,
		ImGuiKey_None, /* 58 */
		ImGuiKey_Semicolon,
		ImGuiKey_None, /* 60 */
		ImGuiKey_Equal,
		ImGuiKey_None, /* 62 */ ImGuiKey_None, /* 63 */ ImGuiKey_None, /* 64 */
		ImGuiKey_A,
		ImGuiKey_B,
		ImGuiKey_C,
		ImGuiKey_D,
		ImGuiKey_E,
		ImGuiKey_F,
		ImGuiKey_G,
		ImGuiKey_H,
		ImGuiKey_I,
		ImGuiKey_J,
		ImGuiKey_K,
		ImGuiKey_L,
		ImGuiKey_M,
		ImGuiKey_N,
		ImGuiKey_O,
		ImGuiKey_P,
		ImGuiKey_Q,
		ImGuiKey_R,
		ImGuiKey_S,
		ImGuiKey_T,
		ImGuiKey_U,
		ImGuiKey_V,
		ImGuiKey_W,
		ImGuiKey_X,
		ImGuiKey_Y,
		ImGuiKey_Z,
		ImGuiKey_LeftBracket,
		ImGuiKey_Backslash,
		ImGuiKey_RightBracket,
		ImGuiKey_None, /* 94 */ ImGuiKey_None, /* 95 */
		ImGuiKey_GraveAccent,
		ImGuiKey_None, /* 97 */ ImGuiKey_None, /* 98 */ ImGuiKey_None, /* 99 */ ImGuiKey_None, /* 100 */ ImGuiKey_None, /* 101 */
		ImGuiKey_None, /* 102 */ ImGuiKey_None, /* 103 */ ImGuiKey_None, /* 104 */ ImGuiKey_None, /* 105 */ ImGuiKey_None, /* 106 */
		ImGuiKey_None, /* 107 */ ImGuiKey_None, /* 108 */ ImGuiKey_None, /* 109 */ ImGuiKey_None, /* 110 */ ImGuiKey_None, /* 111 */
		ImGuiKey_None, /* 112 */ ImGuiKey_None, /* 113 */ ImGuiKey_None, /* 114 */ ImGuiKey_None, /* 115 */ ImGuiKey_None, /* 116 */
		ImGuiKey_None, /* 117 */ ImGuiKey_None, /* 118 */ ImGuiKey_None, /* 119 */ ImGuiKey_None, /* 120 */ ImGuiKey_None, /* 121 */
		ImGuiKey_None, /* 122 */ ImGuiKey_None, /* 123 */ ImGuiKey_None, /* 124 */ ImGuiKey_None, /* 125 */ ImGuiKey_None, /* 126 */
		ImGuiKey_None, /* 127 */ ImGuiKey_None, /* 128 */ ImGuiKey_None, /* 129 */ ImGuiKey_None, /* 130 */ ImGuiKey_None, /* 131 */
		ImGuiKey_None, /* 132 */ ImGuiKey_None, /* 133 */ ImGuiKey_None, /* 134 */ ImGuiKey_None, /* 135 */ ImGuiKey_None, /* 136 */
		ImGuiKey_None, /* 137 */ ImGuiKey_None, /* 138 */ ImGuiKey_None, /* 139 */ ImGuiKey_None, /* 140 */ ImGuiKey_None, /* 141 */
		ImGuiKey_None, /* 142 */ ImGuiKey_None, /* 143 */ ImGuiKey_None, /* 144 */ ImGuiKey_None, /* 145 */ ImGuiKey_None, /* 146 */
		ImGuiKey_None, /* 147 */ ImGuiKey_None, /* 148 */ ImGuiKey_None, /* 149 */ ImGuiKey_None, /* 150 */ ImGuiKey_None, /* 151 */
		ImGuiKey_None, /* 152 */ ImGuiKey_None, /* 153 */ ImGuiKey_None, /* 154 */ ImGuiKey_None, /* 155 */ ImGuiKey_None, /* 156 */
		ImGuiKey_None, /* 157 */ ImGuiKey_None, /* 158 */ ImGuiKey_None, /* 159 */ ImGuiKey_None, /* 160 */ ImGuiKey_None, /* 161 */
		ImGuiKey_None, /* 162 */ ImGuiKey_None, /* 163 */ ImGuiKey_None, /* 164 */ ImGuiKey_None, /* 165 */ ImGuiKey_None, /* 166 */
		ImGuiKey_None, /* 167 */ ImGuiKey_None, /* 168 */ ImGuiKey_None, /* 169 */ ImGuiKey_None, /* 170 */ ImGuiKey_None, /* 171 */
		ImGuiKey_None, /* 172 */ ImGuiKey_None, /* 173 */ ImGuiKey_None, /* 174 */ ImGuiKey_None, /* 175 */ ImGuiKey_None, /* 176 */
		ImGuiKey_None, /* 177 */ ImGuiKey_None, /* 178 */ ImGuiKey_None, /* 179 */ ImGuiKey_None, /* 180 */ ImGuiKey_None, /* 181 */
		ImGuiKey_None, /* 182 */ ImGuiKey_None, /* 183 */ ImGuiKey_None, /* 184 */ ImGuiKey_None, /* 185 */ ImGuiKey_None, /* 186 */
		ImGuiKey_None, /* 187 */ ImGuiKey_None, /* 188 */ ImGuiKey_None, /* 189 */ ImGuiKey_None, /* 190 */ ImGuiKey_None, /* 191 */
		ImGuiKey_None, /* 192 */ ImGuiKey_None, /* 193 */ ImGuiKey_None, /* 194 */ ImGuiKey_None, /* 195 */ ImGuiKey_None, /* 196 */
		ImGuiKey_None, /* 197 */ ImGuiKey_None, /* 198 */ ImGuiKey_None, /* 199 */ ImGuiKey_None, /* 200 */ ImGuiKey_None, /* 201 */
		ImGuiKey_None, /* 202 */ ImGuiKey_None, /* 203 */ ImGuiKey_None, /* 204 */ ImGuiKey_None, /* 205 */ ImGuiKey_None, /* 206 */
		ImGuiKey_None, /* 207 */ ImGuiKey_None, /* 208 */ ImGuiKey_None, /* 209 */ ImGuiKey_None, /* 210 */ ImGuiKey_None, /* 211 */
		ImGuiKey_None, /* 212 */ ImGuiKey_None, /* 213 */ ImGuiKey_None, /* 214 */ ImGuiKey_None, /* 215 */ ImGuiKey_None, /* 216 */
		ImGuiKey_None, /* 217 */ ImGuiKey_None, /* 218 */ ImGuiKey_None, /* 219 */ ImGuiKey_None, /* 220 */ ImGuiKey_None, /* 221 */
		ImGuiKey_None, /* 222 */ ImGuiKey_None, /* 223 */ ImGuiKey_None, /* 224 */ ImGuiKey_None, /* 225 */ ImGuiKey_None, /* 226 */
		ImGuiKey_None, /* 227 */ ImGuiKey_None, /* 228 */ ImGuiKey_None, /* 229 */ ImGuiKey_None, /* 230 */ ImGuiKey_None, /* 231 */
		ImGuiKey_None, /* 232 */ ImGuiKey_None, /* 233 */ ImGuiKey_None, /* 234 */ ImGuiKey_None, /* 235 */ ImGuiKey_None, /* 236 */
		ImGuiKey_None, /* 237 */ ImGuiKey_None, /* 238 */ ImGuiKey_None, /* 239 */ ImGuiKey_None, /* 240 */ ImGuiKey_None, /* 241 */
		ImGuiKey_None, /* 242 */ ImGuiKey_None, /* 243 */ ImGuiKey_None, /* 244 */ ImGuiKey_None, /* 245 */ ImGuiKey_None, /* 246 */
		ImGuiKey_None, /* 247 */ ImGuiKey_None, /* 248 */ ImGuiKey_None, /* 249 */ ImGuiKey_None, /* 250 */ ImGuiKey_None, /* 251 */
		ImGuiKey_None, /* 252 */ ImGuiKey_None, /* 253 */ ImGuiKey_None, /* 254 */ ImGuiKey_None, /* 255 */
		ImGuiKey_Escape,
		ImGuiKey_Enter,
		ImGuiKey_Tab,
		ImGuiKey_Backspace,
		ImGuiKey_Insert,
		ImGuiKey_Delete,
		ImGuiKey_RightArrow,
		ImGuiKey_LeftArrow,
		ImGuiKey_DownArrow,
		ImGuiKey_UpArrow,
		ImGuiKey_PageUp,
		ImGuiKey_PageDown,
		ImGuiKey_Home,
		ImGuiKey_End,
		ImGuiKey_None, /* 270 */ ImGuiKey_None, /* 271 */ ImGuiKey_None, /* 272 */ ImGuiKey_None, /* 273 */ ImGuiKey_None, /* 274 */
		ImGuiKey_None, /* 275 */ ImGuiKey_None, /* 276 */ ImGuiKey_None, /* 277 */ ImGuiKey_None, /* 278 */ ImGuiKey_None, /* 279 */
		ImGuiKey_CapsLock,
		ImGuiKey_ScrollLock,
		ImGuiKey_NumLock,
		ImGuiKey_PrintScreen,
		ImGuiKey_Pause,
		ImGuiKey_None, /* 285 */ ImGuiKey_None, /* 286 */ ImGuiKey_None, /* 287 */ ImGuiKey_None, /* 288 */ ImGuiKey_None, /* 289 */
		ImGuiKey_F1,
		ImGuiKey_F2,
		ImGuiKey_F3,
		ImGuiKey_F4,
		ImGuiKey_F5,
		ImGuiKey_F6,
		ImGuiKey_F7,
		ImGuiKey_F8,
		ImGuiKey_F9,
		ImGuiKey_F10,
		ImGuiKey_F11,
		ImGuiKey_F12,
		ImGuiKey_None, /* 302 */ ImGuiKey_None, /* 303 */ ImGuiKey_None, /* 304 */ ImGuiKey_None, /* 305 */ ImGuiKey_None, /* 306 */
		ImGuiKey_None, /* 307 */ ImGuiKey_None, /* 308 */ ImGuiKey_None, /* 309 */ ImGuiKey_None, /* 310 */ ImGuiKey_None, /* 311 */
		ImGuiKey_None, /* 312 */ ImGuiKey_None, /* 313 */ ImGuiKey_None, /* 314 */ ImGuiKey_None, /* 315 */ ImGuiKey_None, /* 316 */
		ImGuiKey_None, /* 317 */ ImGuiKey_None, /* 318 */ ImGuiKey_None, /* 319 */
		ImGuiKey_Keypad0,
		ImGuiKey_Keypad1,
		ImGuiKey_Keypad2,
		ImGuiKey_Keypad3,
		ImGuiKey_Keypad4,
		ImGuiKey_Keypad5,
		ImGuiKey_Keypad6,
		ImGuiKey_Keypad7,
		ImGuiKey_Keypad8,
		ImGuiKey_Keypad9,
		ImGuiKey_KeypadDecimal,
		ImGuiKey_KeypadDivide,
		ImGuiKey_KeypadMultiply,
		ImGuiKey_KeypadSubtract,
		ImGuiKey_KeypadAdd,
		ImGuiKey_KeypadEnter,
		ImGuiKey_KeypadEqual,
		ImGuiKey_None, /* 337 */ ImGuiKey_None, /* 338 */ ImGuiKey_None, /* 339 */
		ImGuiKey_LeftShift,
		ImGuiKey_LeftCtrl,
		ImGuiKey_LeftAlt,
		ImGuiKey_LeftSuper,
		ImGuiKey_RightShift,
		ImGuiKey_RightCtrl,
		ImGuiKey_RightAlt,
		ImGuiKey_RightSuper,
		ImGuiKey_Menu
	};

	static Gleam::KeyCode TranslateUntranslatedKey(Gleam::KeyCode key_code, int32_t scancode)
	{
		// GLFW 3.1+ attempts to "untranslate" keys, which goes the opposite of what every other framework does, making using lettered shortcuts difficult.
		// (It had reasons to do so: namely GLFW is/was more likely to be used for WASD-type game controls rather than lettered shortcuts, but IHMO the 3.1 change could have been done differently)
		// See https://github.com/glfw/glfw/issues/1502 for details.
		// Adding a workaround to undo this (so our keys are translated->untranslated->translated, likely a lossy process).
		// This won't cover edge cases but this is at least going to cover common cases.

		if (static_cast<int32_t>(key_code) >= GLFW_KEY_KP_0 && static_cast<int32_t>(key_code) <= GLFW_KEY_KP_EQUAL) {
			return key_code;
		}

		const char* const key_name = glfwGetKeyName(static_cast<int32_t>(key_code), scancode);

		if (key_name && key_name[0] != 0 && key_name[1] == 0) {
			constexpr const int32_t char_keys[] = { GLFW_KEY_GRAVE_ACCENT, GLFW_KEY_MINUS, GLFW_KEY_EQUAL, GLFW_KEY_LEFT_BRACKET, GLFW_KEY_RIGHT_BRACKET, GLFW_KEY_BACKSLASH, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_APOSTROPHE, GLFW_KEY_PERIOD, GLFW_KEY_SLASH, 0 };
			constexpr const char char_names[] = "`-=[]\\,;\'./";
			static_assert(std::size(char_names) == std::size(char_keys));

			if (key_name[0] >= '0' && key_name[0] <= '9') {
				key_code = static_cast<Gleam::KeyCode>(GLFW_KEY_0 + (key_name[0] - '0'));
			} else if (key_name[0] >= 'A' && key_name[0] <= 'Z') {
				key_code = static_cast<Gleam::KeyCode>(GLFW_KEY_A + (key_name[0] - 'A'));
			} else if (const char* p = strchr(char_names, key_name[0])) {
				key_code = static_cast<Gleam::KeyCode>(char_keys[p - char_names]);
			}
		}

		return key_code;
	}
}


GAFF_STATIC_FILE_FUNC
{
	ImGui::SetAllocatorFunctions(ImGuiAlloc, ImGuiFree);
}

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::DebugManager::DebugFlag)
	.entry("Draw FPS", Shibboleth::DebugManager::DebugFlag::DrawFPS)
SHIB_REFLECTION_DEFINE_END(Shibboleth::DebugManager::DebugFlag)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::DebugManager)
	.BASE(Shibboleth::IDebugManager)
	.template base<Shibboleth::IManager>()
	.template ctor<>()

	.var(
		"Debug Flags",
		&Shibboleth::DebugManager::_debug_flags,
		Shibboleth::DebugMenuItemAttribute(u8"Debug")
	)
SHIB_REFLECTION_DEFINE_END(Shibboleth::DebugManager)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DebugManager)

void DebugManager::HandleKeyboardCharacterInput(Gleam::Window& /*window*/, uint32_t char_code)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharacter(char_code);
}

void DebugManager::HandleKeyboardInput(Gleam::Window& /*window*/, Gleam::KeyCode key_code, bool pressed, Gaff::Flags<Gleam::Modifier> modifiers, int32_t scan_code)
{
	const ImGuiKey key = k_key_mapping[static_cast<size_t>(key_code)];

	key_code = TranslateUntranslatedKey(key_code, scan_code);

	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(ImGuiKey_ModCtrl,  modifiers.testAll(Gleam::Modifier::Control));
	io.AddKeyEvent(ImGuiKey_ModShift, modifiers.testAll(Gleam::Modifier::Shift));
	io.AddKeyEvent(ImGuiKey_ModAlt, modifiers.testAll(Gleam::Modifier::Alt));
	io.AddKeyEvent(ImGuiKey_ModSuper, modifiers.testAll(Gleam::Modifier::Super));

	io.AddKeyEvent(key, pressed);
}

void DebugManager::HandleMouseButtonInput(Gleam::Window& /*window*/, Gleam::MouseButton button_code, bool pressed, Gaff::Flags<Gleam::Modifier> modifiers)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddKeyEvent(ImGuiKey_ModCtrl, modifiers.testAll(Gleam::Modifier::Control));
	io.AddKeyEvent(ImGuiKey_ModShift, modifiers.testAll(Gleam::Modifier::Shift));
	io.AddKeyEvent(ImGuiKey_ModAlt, modifiers.testAll(Gleam::Modifier::Alt));
	io.AddKeyEvent(ImGuiKey_ModSuper, modifiers.testAll(Gleam::Modifier::Super));

	if (static_cast<int32_t>(button_code) >= 0 && static_cast<int32_t>(button_code) < ImGuiMouseButton_COUNT) {
		io.AddMouseButtonEvent(static_cast<int32_t>(button_code), pressed);
	}
}

void DebugManager::HandleMouseWheelInput(Gleam::Window& /*window*/, const Gleam::Vec2& wheel)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent(wheel.x, wheel.y);
}

void DebugManager::HandleMousePosInput(Gleam::Window& /*window*/, const Gleam::Vec2& pos)
{
	ImGuiIO& io = ImGui::GetIO();
	reinterpret_cast<DebugManager*>(io.BackendPlatformUserData)->_last_mouse_pos = pos;
	io.AddMousePosEvent(pos.x, pos.y);
}

void DebugManager::HandleMouseEnterLeave(Gleam::Window& window, bool entered)
{
	GAFF_REF(window, entered);

	ImGuiIO& io = ImGui::GetIO();
	DebugManager& debug_mgr = *reinterpret_cast<DebugManager*>(io.BackendPlatformUserData);

	if (entered) {
		io.AddMousePosEvent(debug_mgr._last_mouse_pos.x, debug_mgr._last_mouse_pos.y);

	} else if (!entered && debug_mgr._main_window == &window) {
		debug_mgr._last_mouse_pos.x = io.MousePos.x;
		debug_mgr._last_mouse_pos.y = io.MousePos.y;
		io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
	}
}

void DebugManager::HandleMainWindowClosed(Gleam::Window& /*window*/)
{
	DebugManager& dbg_mgr = GetManagerTFast<DebugManager>();
	dbg_mgr._main_output = nullptr;
	dbg_mgr._main_window = nullptr;
}

void DebugManager::HandleFocus(Gleam::Window& /*window*/, bool focused)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddFocusEvent(focused);
}

void DebugManager::SetClipboardText(void* user_data, const char* text)
{
	DebugManager& dbg_mgr = *reinterpret_cast<DebugManager*>(user_data);

	if (dbg_mgr._main_window) {
		dbg_mgr._main_window->setClipboardText(text);
	}
}

const char* DebugManager::GetClipboardText(void* user_data)
{
	DebugManager& dbg_mgr = *reinterpret_cast<DebugManager*>(user_data);

	if (dbg_mgr._main_window) {
		return dbg_mgr._main_window->getClipboardText();
	}

	return "";
}

void DebugManager::RenderDebugShape(uintptr_t thread_id_int, DebugRenderJobData& job_data, const ResourcePtr<ModelResource>& model, DebugRenderInstanceData& debug_data)
{
	GAFF_ASSERT(job_data.type != DebugRenderType::Model || model);

	GAFF_REF(thread_id_int, job_data, model, debug_data);

/*	const int32_t num_cameras = static_cast<int32_t>(job_data.debug_mgr->_camera.size());
	Gleam::Mat4x4 final_camera = glm::identity<Gleam::Mat4x4>();
	ECSEntityID camera_id = ECSEntityID_None;

	// Nothing to render.
	if (!debug_data.render_list[0].size() && !debug_data.render_list[1].size()) {
		return;
	}

	for (int32_t i = 0; i < num_cameras; ++i) {
		job_data.debug_mgr->_ecs_mgr->iterate<Position, Rotation, Camera>(
			job_data.debug_mgr->_camera_position[i],
			job_data.debug_mgr->_camera_rotation[i],
			job_data.debug_mgr->_camera[i],
			[&](ECSEntityID id, const Position& position, const Rotation& rotation, const Camera& camera) -> void
			{
				constexpr Gaff::Hash32 main_tag = Gaff::FNV1aHash32Const(u8"main");

				if (camera.device_tag == main_tag) {
					const Gleam::IVec2 size = job_data.debug_mgr->_main_output->getSize();
					const Gleam::Mat4x4 projection = glm::perspectiveFovLH(
						camera.GetVerticalFOV() * Gaff::TurnsToRad,
						static_cast<float>(size.x),
						static_cast<float>(size.y),
						camera.z_near, camera.z_far
					);

					const Gleam::Vec3 euler_angles = rotation.value * Gaff::TurnsToRad;
					Gleam::Mat4x4 camera_transform = glm::yawPitchRoll(euler_angles.y, euler_angles.x, euler_angles.z);
					camera_transform[3] = Gleam::Vec4(position.value, 1.0f);

					final_camera = projection * glm::inverse(camera_transform);
					camera_id = id;
				}
			}
		);
	}

	if (camera_id < 0) {
		// $TODO: Log error.
		return;
	}

	const auto* const devices = job_data.debug_mgr->_render_mgr->getDevicesByTag(u8"main");
	GAFF_ASSERT(devices && devices->size() > 0);

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	Gleam::IRenderDevice* const rd = job_data.debug_mgr->_render_mgr->getDeferredDevice(*devices->front(), thread_id);

	const RenderManagerBase::GBufferData* const g_buffer = job_data.debug_mgr->_render_mgr->getGBuffer(camera_id, *devices->front());

	if (!g_buffer) {
		// $TODO: Log error.
		return;
	}

	if (job_data.type == DebugRenderType::Line) {
		Gleam::IBuffer* const const_buffer =
			job_data.debug_mgr->_debug_data.instance_data[static_cast<int32_t>(job_data.type)].constant_buffer.get();

		void* const buffer = const_buffer->map(*rd);

		if (buffer) {
			Gleam::Mat4x4* const view_proj_matrix = reinterpret_cast<Gleam::Mat4x4*>(buffer);
			*view_proj_matrix = final_camera;

			const_buffer->unmap(*rd);
		}
	}

	RenderManagerBase::RenderCommandList* const cmds[] = {
		&job_data.debug_mgr->_render_mgr->getRenderCommands(
			*job_data.debug_mgr->_main_device,
			RenderManagerBase::RenderOrder::InWorldNoDepthTest,
			job_data.debug_mgr->_render_cache_index
		),
		&job_data.debug_mgr->_render_mgr->getRenderCommands(
			*job_data.debug_mgr->_main_device,
			RenderManagerBase::RenderOrder::InWorldWithDepthTest,
			job_data.debug_mgr->_render_cache_index
		)
	};

	for (int32_t i = 0; i < 2; ++i) {
		const int32_t num_buffers_needed = static_cast<int32_t>(
			ceilf(static_cast<float>(debug_data.render_list[i].size()) / k_num_instances_per_buffer)
		);

		// Nothing to render.
		if (!num_buffers_needed) {
			continue;
		}

		int32_t secondary_buffer_size_scalar = 0;

		switch (job_data.type) {
			case DebugRenderType::Capsule:
				secondary_buffer_size_scalar = 2;
				break;

			case DebugRenderType::Arrow:
				secondary_buffer_size_scalar = 1;
				break;

			default:
				break;
		}

		const int32_t size_scalar = (secondary_buffer_size_scalar > 0) ? 2 : 1;

		// Make sure we have enough buffers on the GPU to hold our instance data.
		while (static_cast<int32_t>(debug_data.instance_data[i].size() / size_scalar) < num_buffers_needed) {
			Gleam::IBuffer* const buffer = job_data.debug_mgr->_render_mgr->createBuffer();

			if (!buffer) {
				// $TODO: Log error.
				continue;
			}

			const int32_t data_size = (job_data.type == DebugRenderType::Line) ?
				static_cast<int32_t>(sizeof(Gleam::Color::RGB) + sizeof(Gleam::Vec3) * 2) :
				static_cast<int32_t>(sizeof(Gleam::Mat4x4) + sizeof(Gleam::Color::RGB));

			Gleam::IBuffer::Settings settings = {
				nullptr,
				static_cast<size_t>(data_size) * static_cast<size_t>(k_num_instances_per_buffer),
				data_size,
				data_size,
				Gleam::IBuffer::Type::StructuredData,
				Gleam::IBuffer::MapType::Write,
				true
			};

			if (!buffer->init(*job_data.debug_mgr->_main_device, settings)) {
				SHIB_FREET(buffer, g_allocator);
				// $TODO: Log error.
				continue;
			}

			Gleam::IShaderResourceView* const view = job_data.debug_mgr->_render_mgr->createShaderResourceView();

			if (!view) {
				SHIB_FREET(buffer, g_allocator);
				// $TODO: Log error.
				continue;
			}

			if (!view->init(*job_data.debug_mgr->_main_device, buffer)) {
				SHIB_FREET(buffer, g_allocator);
				SHIB_FREET(view, g_allocator);
				// $TODO: Log error.
				continue;
			}

			Gleam::IShaderResourceView* view2 = nullptr;
			Gleam::IBuffer* buffer2 = nullptr;

			if (secondary_buffer_size_scalar > 0) {
				buffer2 = job_data.debug_mgr->_render_mgr->createBuffer();

				if (!buffer2) {
					// $TODO: Log error.
					continue;
				}

				settings.size = static_cast<size_t>(data_size) * static_cast<size_t>(k_num_instances_per_buffer) * secondary_buffer_size_scalar;

				if (!buffer2->init(*job_data.debug_mgr->_main_device, settings)) {
					SHIB_FREET(buffer, g_allocator);
					SHIB_FREET(view, g_allocator);
					SHIB_FREET(buffer2, g_allocator);

					// $TODO: Log error.
					continue;
				}

				view2 = job_data.debug_mgr->_render_mgr->createShaderResourceView();

				if (!view2) {
					SHIB_FREET(buffer, g_allocator);
					SHIB_FREET(view, g_allocator);
					SHIB_FREET(buffer2, g_allocator);

					// $TODO: Log error.
					continue;
				}

				if (!view2->init(*job_data.debug_mgr->_main_device, buffer2)) {
					SHIB_FREET(buffer, g_allocator);
					SHIB_FREET(view, g_allocator);
					SHIB_FREET(buffer2, g_allocator);
					SHIB_FREET(view2, g_allocator);

					// $TODO: Log error.
					continue;
				}
			}

			if (job_data.type == DebugRenderType::Model) {
				debug_data.instance_data_view[i].emplace_back(view);
				debug_data.instance_data[i].emplace_back(buffer);

			} else {
				debug_data.instance_data_view[i].emplace_back(view);
				debug_data.instance_data[i].emplace_back(buffer);
			}

			if (buffer2 && view2) {
				debug_data.instance_data_view[i].emplace_back(view2);
				debug_data.instance_data[i].emplace_back(buffer2);
			}
		}

		g_buffer->render_target->bind(*rd);
		job_data.debug_mgr->_debug_data.depth_stencil_state[i]->bind(*rd);
		job_data.debug_mgr->_debug_data.raster_state[i]->bind(*rd);
		job_data.debug_mgr->_blend_state->bind(*rd);

		if (job_data.type == DebugRenderType::Line) {
			job_data.debug_mgr->_debug_data.line_program->bind(*rd);
		} else {
			job_data.debug_mgr->_debug_data.program->bind(*rd);
			job_data.debug_mgr->_debug_data.layout->bind(*rd);
		}

		int32_t total_items = static_cast<int32_t>(debug_data.render_list[i].size());
		int32_t curr_index = 0;

		for (int32_t j = 0; j < static_cast<int32_t>(debug_data.instance_data[i].size() / size_scalar); ++j) {
			const int32_t count = Gaff::Min(total_items, k_num_instances_per_buffer);
			int8_t* mapped_buffer = reinterpret_cast<int8_t*>(debug_data.instance_data[i][j * size_scalar]->map(*rd));

			if (!mapped_buffer) {
				// $TODO: Log error.
				continue;
			}

			int8_t* secondary_mapped_buffer = nullptr;

			if (size_scalar > 1) {
				secondary_mapped_buffer = reinterpret_cast<int8_t*>(debug_data.instance_data[i][j * size_scalar + 1]->map(*rd));

				if (!secondary_mapped_buffer) {
					// $TODO: Log error.
					continue;
				}
			}

			// Update instance buffers(s).
			for (int32_t k = 0; k < count; ++k) {
				const DebugRenderInstance& inst = *debug_data.render_list[i][curr_index];

				switch (job_data.type) {
					case DebugRenderType::Line: {
						constexpr size_t k_instance_size = 2 * sizeof(Gleam::Vec3) + sizeof(Gleam::Color::RGB);

						memcpy(mapped_buffer, &inst.color, sizeof(Gleam::Color::RGB));
						memcpy(mapped_buffer + sizeof(Gleam::Vec3), &inst.transform.getTranslation(), sizeof(Gleam::Vec3));
						memcpy(mapped_buffer + 2 * sizeof(Gleam::Vec3), &inst.transform.getScale(), sizeof(Gleam::Vec3));

						mapped_buffer += k_instance_size;
					} break;

					case DebugRenderType::Capsule: {
						constexpr size_t k_instance_size = sizeof(Gleam::Mat4x4) + sizeof(Gleam::Color::RGB);

						// Copy data for cylinder.
						Gleam::Mat4x4 transform = final_camera * inst.transform.toMatrix();

						memcpy(mapped_buffer, &transform, sizeof(Gleam::Mat4x4));
						memcpy(mapped_buffer + sizeof(Gleam::Mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						// Copy data for top half-sphere of capsule.
						const Gleam::Vec3& scale = inst.transform.getScale(); // scale = (radius, height, radius)
						Gleam::Transform modified_transform = inst.transform;

						modified_transform.setScale(1.0f);
						const Gleam::Mat4x4 sphere_base_transform = modified_transform.toMatrix();

						transform = glm::scale(Gleam::Vec3(scale.x));
						transform[3] = Gleam::Vec4(0.0f, scale.y * 0.5f, 0.0f, 1.0f);

						transform = final_camera * sphere_base_transform * transform;

						memcpy(secondary_mapped_buffer, &transform, sizeof(Gleam::Mat4x4));
						memcpy(secondary_mapped_buffer + sizeof(Gleam::Mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						secondary_mapped_buffer += k_instance_size;

						// Copy data for bottom half-sphere of capsule.
						transform = glm::eulerAngleX(Gaff::Pi); // Flip upside down.
						transform[3] = Gleam::Vec4(0.0f, scale.y * -0.5f, 0.0f, 1.0f);
						transform = glm::scale(transform, Gleam::Vec3(scale.x));

						transform = final_camera * sphere_base_transform * transform;

						memcpy(secondary_mapped_buffer, &transform, sizeof(Gleam::Mat4x4));
						memcpy(secondary_mapped_buffer + sizeof(Gleam::Mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						secondary_mapped_buffer += k_instance_size;
						mapped_buffer += k_instance_size;
					} break;

					case DebugRenderType::Arrow: {
						constexpr size_t k_instance_size = sizeof(Gleam::Mat4x4) + sizeof(Gleam::Color::RGB);

						// Copy data for cylinder.
						const Gleam::Vec3& dir = inst.transform.getScale();
						Gleam::Vec3 up = Gleam::Vec3(0.0f, 1.0f, 0.0f);
						const float length = glm::length(dir);

						if (const auto result = glm::equal(dir, up, Gaff::Epsilon); result.x == true && result.y == true && result.z == true) {
							up = Gleam::Vec3(0.0f, 0.0f, 1.0f);
						}

						constexpr float k_cylinder_scale = 0.025f;

						const Gleam::Mat4x4 look_at = glm::lookAt(glm::zero<Gleam::Vec3>(), dir, up);
						const Gleam::Mat4x4 rotate_forward = glm::eulerAngleX(Gaff::Pi * 0.5f); // Orient top facing forward;

						Gleam::Mat4x4 transform =
							final_camera *
							glm::translate(inst.transform.getTranslation() + dir * 0.5f) *
							look_at *
							glm::scale(rotate_forward, Gleam::Vec3(k_cylinder_scale, length, k_cylinder_scale));

						memcpy(mapped_buffer, &transform, sizeof(Gleam::Mat4x4));
						memcpy(mapped_buffer + sizeof(Gleam::Mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						// Copy data for cone.
						constexpr float k_cone_scale = 0.085f;

						transform =
							final_camera *
							glm::translate(inst.transform.getTranslation() + dir + glm::normalize(dir) * k_cone_scale * 0.5f) *
							look_at *
							glm::scale(rotate_forward, Gleam::Vec3(k_cone_scale));

						memcpy(secondary_mapped_buffer, &transform, sizeof(Gleam::Mat4x4));
						memcpy(secondary_mapped_buffer + sizeof(Gleam::Mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						secondary_mapped_buffer += k_instance_size;
						mapped_buffer += k_instance_size;
					} break;

					default: {
						const Gleam::Mat4x4 transform = final_camera * inst.transform.toMatrix();

						memcpy(mapped_buffer, &transform, sizeof(Gleam::Mat4x4));
						memcpy(mapped_buffer + sizeof(Gleam::Mat4x4), &inst.color, sizeof(Gleam::Color::RGB));

						mapped_buffer += sizeof(Gleam::Mat4x4) + sizeof(Gleam::Color::RGB);
					} break;
				}

				++curr_index;
			}

			debug_data.instance_data[i][j * size_scalar]->unmap(*rd);

			if (secondary_mapped_buffer) {
				debug_data.instance_data[i][j * size_scalar + 1]->unmap(*rd);
			}

			switch (job_data.type) {
				case DebugRenderType::Line:
					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j].get());
					debug_data.program_buffers->bind(*rd);

					rd->renderLineNoVertexInputInstanced(count);
					break;

				case DebugRenderType::Capsule:
					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j * 2].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[0]->renderInstanced(*rd, count);

					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j * 2 + 1].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[1]->renderInstanced(*rd, count * 2);
					break;

				case DebugRenderType::Arrow:
					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j * 2].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[0]->renderInstanced(*rd, count);

					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j * 2 + 1].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[1]->renderInstanced(*rd, count);
					break;

				case DebugRenderType::Model: {
					auto& program_buffers = job_data.debug_mgr->_debug_data.instance_data[static_cast<size_t>(DebugRenderType::Model)].program_buffers;

					program_buffers->clearResourceViews();
					program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j].get());
					program_buffers->bind(*rd);

					const int32_t num_meshes = model->getNumMeshes();

					for (int32_t k = 0; k < num_meshes; ++k) {
						Gleam::IMesh* const mesh = model->getMesh(k)->getMesh(*devices->front());
						mesh->renderInstanced(*rd, count);
					}
				} break;

				default:
					debug_data.program_buffers->clearResourceViews();
					debug_data.program_buffers->addResourceView(Gleam::IShader::Type::Vertex, debug_data.instance_data_view[i][j].get());
					debug_data.program_buffers->bind(*rd);

					debug_data.mesh[0]->renderInstanced(*rd, count);
					break;
			}

			total_items -= k_num_instances_per_buffer;
		}

		if (!rd->finishCommandList(*job_data.cmd_list[job_data.debug_mgr->_render_cache_index][i])) {
			// $TODO: Log error
			continue;
		}

		cmds[i]->lock.Lock();
		auto& cmd = cmds[i]->command_list.emplace_back();
		cmd.cmd_list.reset(job_data.cmd_list[job_data.debug_mgr->_render_cache_index][i].get());
		cmd.owns_command = false;
		cmds[i]->lock.Unlock();
	}*/
}

void DebugManager::RenderDebugShape(uintptr_t thread_id_int, void* data)
{
	DebugRenderJobData& job_data = *reinterpret_cast<DebugRenderJobData*>(data);

	if (job_data.type == DebugRenderType::Model) {
		for (auto& inst : job_data.debug_mgr->_debug_data.model_instance_data) {
			RenderDebugShape(thread_id_int, job_data, inst.first, inst.second);
		}

	} else {
		DebugRenderInstanceData& debug_data = job_data.debug_mgr->_debug_data.instance_data[static_cast<int32_t>(job_data.type)];
		static const ResourcePtr<ModelResource> model_ptr;

		RenderDebugShape(thread_id_int, job_data, model_ptr, debug_data);
	}
}

void DebugManager::SetupModuleToUseImGui(void)
{
	// Go through IDebugManager so that we get DebugModule's ImGui context.
	const IDebugManager& dbg_mgr = GETMANAGERT(Shibboleth::IDebugManager, Shibboleth::DebugManager);
	ImGui::SetCurrentContext(dbg_mgr.getImGuiContext());
}

DebugManager::~DebugManager(void)
{
	ImGui::DestroyContext();

	for (GLFWcursor* cursor : _mouse_cursors) {
		if (cursor) {
			glfwDestroyCursor(cursor);
		}
	}
}

bool DebugManager::initAllModulesLoaded(void)
{
	_time = &GetManagerTFast<GameTimeManager>().getRealTime();
	_render_mgr = &GetManagerTFast<RenderManager>();
	_input_mgr = &GetManagerTFast<InputManager>();
	_main_output = _render_mgr->getOutput("main");
	_main_window = _render_mgr->getWindow("main");

	const auto* const devices = _render_mgr->getDevicesByTag("main");
	GAFF_ASSERT(devices && devices->size() == 1);

	_main_device = devices->front();

	if (_main_window) {
		_main_window->addCloseCallback(Gaff::Func(HandleMainWindowClosed));
	}

/*	ECSQuery camera_query;
	camera_query.add<Position>(_camera_position);
	camera_query.add<Rotation>(_camera_rotation);
	camera_query.add<Camera>(_camera);

	_ecs_mgr = &GetManagerTFast<ECSManager>();
	_ecs_mgr->registerQuery(std::move(camera_query));
*/
	return initDebugRender() && initImGui();
}

void DebugManager::update(void)
{
	if (!_main_output) {
		_main_output = _render_mgr->getOutput("main");
		_main_window = _render_mgr->getWindow("main");

		if (!_main_output) {
			return;
		}

		if (_main_window) {
			_main_window->addCloseCallback(Gaff::Func(HandleMainWindowClosed));

		#ifdef PLATFORM_WINDOWS
			ImGui::GetMainViewport()->PlatformHandleRaw = _main_window->getHWnd();
		#endif
		}
	}

	constexpr Gaff::Hash32 k_debug_menu_toggle = Gaff::FNV1aHash32Const("Debug_Menu_Toggle");
	constexpr Gaff::Hash32 k_debug_input_mode = Gaff::FNV1aHash32Const("Debug");
/*	const float toggle = _input_mgr->getAliasValue(k_debug_menu_toggle, _input_mgr->getKeyboardMousePlayerID());

	if (toggle > 0.0f) {
		if (_flags.toggle(Flag::ShowDebugMenu)) {
			_input_mgr->setMode(k_debug_input_mode);
		} else {
			_input_mgr->setModeToPrevious();
		}
	}
*/
	const Gleam::IVec2& size = _main_output->getSize();

	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = _time->getDeltaFloat();
	io.DisplaySize.x = static_cast<float>(size.x);
	io.DisplaySize.y = static_cast<float>(size.y);

	const ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();

	// Update mouse cursor.
	if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
		if (_prev_cursor != mouse_cursor) {
			_prev_cursor = mouse_cursor;

			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			if (mouse_cursor == ImGuiMouseCursor_None) {
				_main_window->hideCursor();
			} else {
				_main_window->showCursor();
				_main_window->setCursor(_mouse_cursors[mouse_cursor] ? _mouse_cursors[mouse_cursor] : _mouse_cursors[ImGuiMouseCursor_Arrow]);
			}
		}
	}

	if (io.WantSetMousePos) {
		_main_window->setMousePos(Gleam::Vec2(io.MousePos.x, io.MousePos.y));
	}


	//const int32_t char_buf_index = _char_buffer_cache_index;
	//_char_buffer_cache_index = (_char_buffer_cache_index + 1) % 2;

	//for (uint32_t character : _character_buffer[char_buf_index]) {
	//	ImGui::GetIO().AddInputCharacter(character);
	//}

	//_character_buffer[char_buf_index].clear();

	ImGui::NewFrame();

	if (_flags.testAll(Flag::ShowDebugMenu)) {
		if (ImGui::BeginMainMenuBar()) {
			for (DebugMenuEntry& entry : _debug_menu_root.children) {
				renderDebugMenu(entry);
			}

			ImGui::EndMainMenuBar();
		}

		// $TODO: Turn this into a helper function.
		for (auto it = _update_functions.begin(); it != _update_functions.end();) {
			GAFF_ASSERT((*it)->type == DebugMenuEntry::Type::FuncImGuiUpdate);

			bool open = true;

			if (ImGui::Begin(reinterpret_cast<const char*>((*it)->name.getBuffer()), &open)) {
				(*it)->func->call((*it)->object);
			}

			ImGui::End();

			if (open) {
				++it;

			} else {
				(*it)->flags.clear(DebugMenuEntry::Flag::Updating);
				it = _update_functions.erase_unsorted(it);
			}
		}

	} else {
		for (auto it = _update_functions.begin(); it != _update_functions.end();) {
			GAFF_ASSERT((*it)->type == DebugMenuEntry::Type::FuncImGuiUpdate);

			if ((*it)->flags.testAll(DebugMenuEntry::Flag::AlwaysRender)) {
				bool open = true;

				if (ImGui::Begin(reinterpret_cast<const char*>((*it)->name.getBuffer()), &open, ImGuiWindowFlags_NoInputs)) {
					(*it)->func->call((*it)->object);
				}

				ImGui::End();

				if (open) {
					++it;

				} else {
					(*it)->flags.clear(DebugMenuEntry::Flag::Updating);
					it = _update_functions.erase_unsorted(it);
				}

			} else {
				++it;
			}
		}
	}
}

void DebugManager::render(uintptr_t thread_id_int)
{
	if (!_main_output) {
		return;
	}

	renderPostCamera(thread_id_int);
	renderPreCamera(thread_id_int);

	const EA::Thread::ThreadId thread_id = *((EA::Thread::ThreadId*)thread_id_int);
	GetApp().getJobPool().helpWhileWaiting(thread_id, _debug_data.job_counter);

	_render_cache_index = (_render_cache_index + 1) % 2;
}

ImGuiContext* DebugManager::getImGuiContext(void) const
{
	return ImGui::GetCurrentContext();
}

DebugManager::DebugRenderHandle DebugManager::renderDebugArrow(const Gleam::Vec3& start, const Gleam::Vec3& end, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Arrow)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(start);
	instance->transform.setScale(end - start);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Arrow, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugLine(const Gleam::Vec3& start, const Gleam::Vec3& end, const Gleam::Color::RGB& color, bool has_depth)
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

DebugManager::DebugRenderHandle DebugManager::renderDebugSphere(const Gleam::Vec3& pos, float radius, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Sphere)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(radius * 2.0f); // Sphere is unit sphere (radius = 0.5). Double radius to get correct scale.
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Sphere, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugCone(const Gleam::Vec3& pos, const Gleam::Vec3& size, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Cone)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(size);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Cone, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugPlane(const Gleam::Vec3& pos, const Gleam::Vec3& size, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Plane)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(size);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Plane, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugBox(const Gleam::Vec3& pos, const Gleam::Vec3& size, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Box)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(size);
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Box, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugCapsule(const Gleam::Vec3& pos, float radius, float height, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Capsule)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(Gleam::Vec3(radius * 2.0f, height, radius * 2.0f)); // Sphere/Cylinder is unit (radius = 0.5). Double radius to get correct scale.
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Capsule, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugCylinder(const Gleam::Vec3& pos, float radius, float height, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.instance_data[static_cast<int32_t>(DebugRenderType::Cylinder)];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform.setTranslation(pos);
	instance->transform.setScale(Gleam::Vec3(radius * 2.0f, height, radius * 2.0f)); // Cylinder is unit (radius = 0.5). Double radius to get correct scale.
	instance->color = color;

	return DebugRenderHandle(instance, DebugRenderType::Cylinder, has_depth);
}

DebugManager::DebugRenderHandle DebugManager::renderDebugModel(const ResourcePtr<ModelResource>& model, const Gleam::Transform& transform, const Gleam::Color::RGB& color, bool has_depth)
{
	auto& debug_data = _debug_data.model_instance_data[model];
	auto* const instance = SHIB_ALLOCT(DebugRenderInstance, g_allocator);

	debug_data.lock[has_depth].Lock();
	debug_data.render_list[has_depth].emplace_back(instance);
	debug_data.lock[has_depth].Unlock();

	instance->transform = transform;
	instance->color = color;

	return DebugRenderHandle(instance, model.get(), has_depth);
}

void DebugManager::registerDebugMenuItems(void* object, const Refl::IReflectionDefinition& ref_def)
{
	const ProxyAllocator allocator("Debug");
	DebugMenuEntry menu_entry;

	// Register variables.
	Vector< eastl::pair<HashStringView32<>, const DebugMenuItemAttribute*> > results(allocator);
	ref_def.getVarAttrs(results);

	for (const auto& entry : results) {
		Refl::IReflectionVar* const var = ref_def.getVar(entry.first.getHash());
		DebugMenuEntry* root = &_debug_menu_root;

		if (!var->isFlags() && &var->getReflection().getReflectionDefinition() != &Refl::Reflection<bool>::GetReflectionDefinition()) {
			// Menu items only support flags and bools.
			// $TODO: Log error.
			continue;
		}

		const U8String& path = entry.second->getPath();
		size_t prev_index = 0;
		size_t index = 0;

		while (index != U8String::npos) {
			menu_entry.name = (index == 0) ?
				path.substr(0) :
				path.substr(prev_index, index - prev_index);

			auto it = Gaff::LowerBound(root->children, menu_entry.name);

			if (it == root->children.end() || it->name != menu_entry.name) {
				it = root->children.insert(it, menu_entry);
			}

			prev_index = index;
			root = &(*it);

			index = path.find(u8"/", index + 1);
		}

		menu_entry.name = entry.first;

		// Search for an entry that already has this name.
		auto it = Gaff::LowerBound(root->children, menu_entry.name);
		U8String base_name = menu_entry.name.getString();
		int32_t i = 2;

		// If an entry with this name already exists, then add numbers to the end until we don't find a match.
		while (it != root->children.end() && it->name == menu_entry.name) {
			menu_entry.name = base_name + U8String(U8String::CtorSprintf(), u8" %i", i++);
			it = Gaff::LowerBound(root->children, menu_entry.name);
		}

		menu_entry.type = DebugMenuEntry::Type::Var;
		menu_entry.object = object;
		menu_entry.var = var;

		root->children.insert(it, menu_entry);
	}

	// Register functions.
	results.clear();
	ref_def.getFuncAttrs<void>(results);

	for (const auto& entry : results) {
		Refl::IReflectionFunction<void>* const func = ref_def.getFunc<void>(entry.first.getHash());

		if (!func) {
			// Only allow functions with signature void func(void).
			// $TODO: Log error.
			continue;
		}

		const U8String& path = entry.second->getPath();
		DebugMenuEntry* root = &_debug_menu_root;
		size_t prev_index = 0;
		size_t index = 0;

		while (index != U8String::npos) {
			menu_entry.name = (index == 0) ?
				path.substr(0) :
				path.substr(prev_index, index - prev_index);

			auto it = Gaff::LowerBound(root->children, menu_entry.name);

			if (it == root->children.end() || it->name != menu_entry.name) {
				it = root->children.insert(it, menu_entry);
			}

			prev_index = index;
			root = &(*it);

			index = path.find(u8"/", index + 1);
		}

		menu_entry.name = entry.first;

		// Search for an entry that already has this name.
		auto it = Gaff::LowerBound(root->children, menu_entry.name);
		U8String base_name = menu_entry.name.getString();
		int32_t i = 2;

		// If an entry with this name already exists, then add numbers to the end until we don't find a match.
		while (it != root->children.end() && it->name == menu_entry.name) {
			menu_entry.name = base_name + U8String(U8String::CtorSprintf(), u8" %i", i++);
			it = Gaff::LowerBound(root->children, menu_entry.name);
		}

		menu_entry.type = entry.second->isImGuiUpdateFunction() ? DebugMenuEntry::Type::FuncImGuiUpdate : DebugMenuEntry::Type::Func;
		menu_entry.object = object;
		menu_entry.func = func;

		root->children.insert(it, menu_entry);
	}

	// Register static functions.
	// $TODO: Need to rethink this for static funcs.
	//results.clear();
	//ref_def.getStaticFuncAttrs<void>(results);

	//for (const auto& entry : results) {
	//	Gaff::IReflectionStaticFunction<void>* const func = ref_def.getStaticFunc<void>(entry.first.getHash());
	//	DebugMenuEntry* root = &_debug_menu_root;

	//	if (!func) {
	//		// Only allow functions with signature void func(void).
	//		// $TODO: Log error.
	//		continue;
	//	}

	//	const U8String& path = entry.second->getPath();
	//	size_t prev_index = 0;
	//	size_t index = 0;

	//	while (index != U8String::npos) {
	//		menu_entry.name = (index == 0) ?
	//			path.substr(0) :
	//			path.substr(prev_index, index - prev_index);

	//		auto it = Gaff::LowerBound(root->children, menu_entry.name);

	//		if (it == root->children.end() || it->name != menu_entry.name) {
	//			it = root->children.insert(it, menu_entry);
	//		}

	//		prev_index = index;
	//		root = &(*it);

	//		index = path.find("/", index + 1);
	//	}

	//	menu_entry.name = entry.first;

	//	// Search for an entry that already has this name.
	//	auto it = Gaff::LowerBound(root->children, menu_entry.name);
	//	U8String base_name = menu_entry.name.getString();
	//	int32_t i = 2;

	//	// If an entry with this name already exists, then add numbers to the end until we don't find a match.
	//	while (it != root->children.end() && it->name == menu_entry.name) {
	//		menu_entry.name = base_name + U8String(U8String::CtorSprintf(), " %i", i++);
	//		it = Gaff::LowerBound(root->children, menu_entry.name);
	//	}

	//	menu_entry.type = DebugMenuEntry::Type::StaticFunc;
	//	menu_entry.static_func = func;
	//	menu_entry.object = object;

	//	root->children.insert(it, menu_entry);
	//}
}

void DebugManager::unregisterDebugMenuItems(void* object, const Refl::IReflectionDefinition& ref_def)
{
	const ProxyAllocator allocator("Debug");

	Vector< eastl::pair<HashStringView32<>, const DebugMenuItemAttribute*> > results(allocator);
	ref_def.getVarAttrs(results);
	ref_def.getFuncAttrs<void>(results);

	for (const auto& entry : results) {
		Refl::IReflectionVar* const var = ref_def.getVar(entry.first.getHash());

		if (!var->isFlags() && &var->getReflection().getReflectionDefinition() != &Refl::Reflection<bool>::GetReflectionDefinition()) {
			// Menu items only support flags and bools.
			// $TODO: Log error.
			continue;
		}

		Vector<DebugMenuEntry*> entries(allocator);
		entries.emplace_back(&_debug_menu_root);

		const U8String& path = entry.second->getPath();
		HashString32<> name(allocator);
		bool path_find_failed = false;
		size_t prev_index = 0;
		size_t index = 0;

		// Get full path to leaf nodes.
		while (index != U8String::npos) {
			name = (index == 0) ?
				path.substr(0) :
				path.substr(prev_index, index - prev_index);

			auto it = Gaff::LowerBound(entries.back()->children, name);

			// Leaf nodes don't exist. Continue to next variable.
			if (it == entries.back()->children.end() || it->name != name) {
				path_find_failed = true;
				break;
			}

			prev_index = index;
			index = path.find(u8"/", index + 1);

			entries.emplace_back(it);
		}

		if (path_find_failed) {
			continue;
		}

		// Remove all entries that contain this object.
		for (auto it = entries.back()->children.begin(); it != entries.back()->children.end(); ++it) {
			if (it->object == object) {
				it = entries.back()->children.erase(it);
			} else {
				++it;
			}
		}

		// Remove all empty submenus.
		while (entries.size() > 1 && entries.back()->children.empty()) {
			(*(entries.end() - 2))->children.erase(entries.back());
			entries.pop_back();
		}
	}
}

void DebugManager::renderPostCamera(uintptr_t thread_id_int)
{
	GAFF_REF(thread_id_int);

/*	ImGui::Render();
	const ImDrawData* const draw_data = ImGui::GetDrawData();

	// Avoid rendering when minimized
	if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
		return;
	}

	if (draw_data->TotalIdxCount <= 0 || draw_data->TotalVtxCount <= 0) {
		return;
	}

	bool buffers_changed = false;

	if (const size_t size = (_vertex_buffer) ? _vertex_buffer->getSize() : 0;
		size < (draw_data->TotalVtxCount * sizeof(ImDrawVert))) {

		_vertex_buffer.reset(_render_mgr->createBuffer());
		buffers_changed = true;

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
	}

	if (const size_t size = (_index_buffer) ? _index_buffer->getSize() : 0;
		size < (draw_data->TotalIdxCount * sizeof(ImDrawIdx))) {

		_index_buffer.reset(_render_mgr->createBuffer());
		buffers_changed = true;

		if (!_index_buffer) {
			// $TODO: Log error periodic.
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
		return;
	}

	if (!indices) {
		// $TODO: Log error periodic.
		_vertex_buffer->unmap(*deferred_device);
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
			const Gleam::IVec4 rect{
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
		return;
	}

	// Submit command
	auto& render_cmds = _render_mgr->getRenderCommands(*_main_device, RenderManagerBase::RenderOrder::ScreenSpaceDirect, _render_cache_index);

	render_cmds.lock.Lock();
	auto& cmd = render_cmds.command_list.emplace_back();
	cmd.cmd_list.reset(_cmd_list[_render_cache_index].get());
	cmd.owns_command = false;
	render_cmds.lock.Unlock();*/
}

void DebugManager::renderPreCamera(uintptr_t /*thread_id_int*/)
{
	auto& job_pool = GetApp().getJobPool();
	job_pool.addJobs(_debug_data.job_data_cache, static_cast<int32_t>(DebugRenderType::Count), _debug_data.job_counter);
}

void DebugManager::removeDebugRender(const DebugRenderHandle& handle)
{
	if (handle._type == DebugRenderType::Model) {
		const auto inst_it = _debug_data.model_instance_data.find_as(handle._model, ModelMapComparison());

		if (inst_it != _debug_data.model_instance_data.end()) {
			auto& render_list = inst_it->second.render_list[handle._depth];

			const auto it = eastl::lower_bound(render_list.begin(), render_list.end(), handle._instance, [](const auto& lhs, auto rhs) -> bool { return lhs.get() < rhs; });

			if (it != render_list.end() && it->get() == handle._instance) {
				render_list.erase(it);
			}

			if (inst_it->second.render_list[0].empty() && inst_it->second.render_list[1].empty()) {
				_debug_data.model_instance_data.erase(inst_it);
			}
		}

	} else {
		auto& render_list = _debug_data.instance_data[static_cast<int32_t>(handle._type)].render_list[handle._depth];
		const auto it = eastl::lower_bound(render_list.begin(), render_list.end(), handle._instance, [](const auto& lhs, auto rhs) -> bool { return lhs.get() < rhs; });

		if (it != render_list.end() && it->get() == handle._instance) {
			render_list.erase(it);
		}
	}
}

bool DebugManager::initDebugRender(void)
{
/*	const size_t renderer_index = static_cast<size_t>(_render_mgr->getRendererType());

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


	// Init layout for meshes.
	_debug_data.layout.reset(_render_mgr->createLayout());

	if (!_debug_data.layout) {
		// $TODO: Log error.
		return false;
	}

	if (!_debug_data.layout->init(*_main_device, *_debug_data.vertex_shader)) {
		// $TODO: Log error.
		return false;
	}


	// Init Depth-Stencil states
	_debug_data.depth_stencil_state[0].reset(_render_mgr->createDepthStencilState());
	_debug_data.depth_stencil_state[1].reset(_render_mgr->createDepthStencilState());

	if (!_debug_data.depth_stencil_state[0] || !_debug_data.depth_stencil_state[1]) {
		// $TODO: Log error.
		return false;
	}

	Gleam::IDepthStencilState::Settings depth_stencil_settings;

	if (!_debug_data.depth_stencil_state[1]->init(*_main_device, depth_stencil_settings)) {
		// $TODO: Log error.
		return false;
	}

	depth_stencil_settings.depth_test = false;

	if (!_debug_data.depth_stencil_state[0]->init(*_main_device, depth_stencil_settings)) {
		// $TODO: Log error.
		return false;
	}

	// Init Raster states
	_debug_data.raster_state[0].reset(_render_mgr->createRasterState());
	_debug_data.raster_state[1].reset(_render_mgr->createRasterState());

	if (!_debug_data.raster_state[0] || !_debug_data.raster_state[1]) {
		// $TODO: Log error.
		return false;
	}

	Gleam::IRasterState::Settings raster_settings;
	raster_settings.two_sided = true;
	raster_settings.wireframe = true;

	if (!_debug_data.raster_state[1]->init(*_main_device, raster_settings)) {
		// $TODO: Log error.
		return false;
	}

	raster_settings.depth_clip_enabled = false;

	if (!_debug_data.raster_state[0]->init(*_main_device, raster_settings)) {
		// $TODO: Log error.
		return false;
	}

	for (int32_t i = 0; i < static_cast<int32_t>(DebugRenderType::Count); ++i) {
		_debug_data.render_job_data_cache[i].type = static_cast<DebugRenderType>(i);
		_debug_data.render_job_data_cache[i].debug_mgr = this;
		_debug_data.job_data_cache[i].job_data = &_debug_data.render_job_data_cache[i];
		_debug_data.job_data_cache[i].job_func = RenderDebugShape;

		_debug_data.render_job_data_cache[i].cmd_list[0][0].reset(_render_mgr->createCommandList());
		_debug_data.render_job_data_cache[i].cmd_list[1][0].reset(_render_mgr->createCommandList());
		_debug_data.render_job_data_cache[i].cmd_list[0][1].reset(_render_mgr->createCommandList());
		_debug_data.render_job_data_cache[i].cmd_list[1][1].reset(_render_mgr->createCommandList());

		if (!_debug_data.render_job_data_cache[i].cmd_list[0][0] || !_debug_data.render_job_data_cache[i].cmd_list[1][0] ||
			!_debug_data.render_job_data_cache[i].cmd_list[0][1] || !_debug_data.render_job_data_cache[i].cmd_list[1][1]) {
			// $TODO: Log error.
			return false;
		}

		DebugRenderInstanceData& instance_data =_debug_data.instance_data[i];

		instance_data.program_buffers.reset(_render_mgr->createProgramBuffers());

		if (!instance_data.program_buffers) {
			// $TODO: Log error.
			return false;
		}

		// Generate shape data.
		Gleam::Vector<Gleam::Vec3> points[2];
		Gleam::Vector<int16_t> indices[2];

		constexpr int32_t k_subdivisions = 16;

		switch (static_cast<DebugRenderType>(i)) {
			case DebugRenderType::Line: {
				instance_data.constant_buffer.reset(_render_mgr->createBuffer());

				if (!instance_data.constant_buffer) {
					// $TODO: Log error.
					return false;
				}

				Gleam::IBuffer::Settings cb_settings;
				cb_settings.size = sizeof(Gleam::Mat4x4);
				cb_settings.cpu_access = Gleam::IBuffer::MapType::Write;

				if (!instance_data.constant_buffer->init(*_main_device, cb_settings)) {
					// $TODO: Log error.
					return false;
				}

				instance_data.program_buffers->addConstantBuffer(
					Gleam::IShader::Type::Vertex,
					instance_data.constant_buffer.get()
				);
			} break;

			case DebugRenderType::Plane:
				Gleam::GenerateDebugPlane(k_subdivisions * 10, points[0], indices[0]);
				break;

			case DebugRenderType::Sphere:
				Gleam::GenerateDebugSphere(k_subdivisions, points[0], indices[0]);
				break;

			case DebugRenderType::Box:
				Gleam::GenerateDebugBox(1, points[0], indices[0]);
				break;

			case DebugRenderType::Cone:
				Gleam::GenerateDebugCone(k_subdivisions, points[0], indices[0]);
				break;

			case DebugRenderType::Capsule:
				Gleam::GenerateDebugCylinder(k_subdivisions, points[0], indices[0], false);
				Gleam::GenerateDebugHalfSphere(k_subdivisions, points[1], indices[1]);
				break;

			case DebugRenderType::Arrow:
				Gleam::GenerateDebugCylinder(k_subdivisions, points[0], indices[0]);
				Gleam::GenerateDebugCone(k_subdivisions, points[1], indices[1]);
				break;

			case DebugRenderType::Cylinder:
				Gleam::GenerateDebugCylinder(k_subdivisions, points[0], indices[0]);
				break;

			case DebugRenderType::Model:
				break;

			default:
				// This should never happen.
				GAFF_ASSERT(false);
				break;
		}

		for (int32_t j = 0; j < 2; ++j) {
			if (!points[j].empty() && !indices[j].empty()) {
				instance_data.mesh[j].reset(_render_mgr->createMesh());

				if (!instance_data.mesh[j]) {
					// $TODO: Log error.
					return false;
				}

				// Vertices
				instance_data.vertices[j].reset(_render_mgr->createBuffer());

				if (!instance_data.vertices[j]) {
					// $TODO: Log error.
					return false;
				}

				Gleam::IBuffer::Settings buffer_settings;
				buffer_settings.type = Gleam::IBuffer::Type::VertexData;
				buffer_settings.size = sizeof(Gleam::Vec3) * points[j].size();
				buffer_settings.element_size = sizeof(Gleam::Vec3);
				buffer_settings.stride = sizeof(Gleam::Vec3);
				buffer_settings.data = points[j].data();

				if (!instance_data.vertices[j]->init(*_main_device, buffer_settings)) {
					// $TODO: Log error.
					return false;
				}

				// Indices
				instance_data.indices[j].reset(_render_mgr->createBuffer());

				if (!instance_data.indices[j]) {
					// $TODO: Log error.
					return false;
				}

				buffer_settings.type = Gleam::IBuffer::Type::IndexData;
				buffer_settings.size = sizeof(int16_t) * indices[j].size();
				buffer_settings.element_size = sizeof(int16_t);
				buffer_settings.stride = sizeof(int16_t);
				buffer_settings.data = indices[j].data();

				if (!instance_data.indices[j]->init(*_main_device, buffer_settings)) {
					// $TODO: Log error.
					return false;
				}

				instance_data.mesh[j]->setTopologyType(Gleam::IMesh::TopologyType::TriangleList);
				instance_data.mesh[j]->setIndexCount(static_cast<int32_t>(indices[j].size()));
				instance_data.mesh[j]->setIndiceBuffer(instance_data.indices[j].get());
				instance_data.mesh[j]->addBuffer(instance_data.vertices[j].get());
			}
		}
	}*/

	return true;
}

bool DebugManager::initImGui(void)
{
/*	IMGUI_CHECKVERSION();

	if (!ImGui::CreateContext()) {
		// $TODO: Log error.
		return false;
	}

	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendPlatformUserData = this;
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;	// We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;	// We can honor io.WantSetMousePos requests (optional, rarely used)
	io.BackendRendererName = "Gleam";
	io.BackendPlatformName = "Gleam";
	io.IniFilename = nullptr;

	io.GetClipboardTextFn = GetClipboardText;
	io.SetClipboardTextFn = SetClipboardText;
	io.ClipboardUserData = this;

#ifdef PLATFORM_WINDOWS
	if (_main_window) {
		ImGui::GetMainViewport()->PlatformHandleRaw = _main_window->getHWnd();
	}
#endif

	static_assert(std::size(_mouse_cursors) == static_cast<size_t>(ImGuiMouseCursor_COUNT));

	_mouse_cursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
	_mouse_cursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
	_mouse_cursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	_mouse_cursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif

	// Register for all the input callbacks.
	for (int32_t i = 0; i < _render_mgr.getNumWindows(); ++i) {
		Gleam::Window* const window = _render_mgr.getWindow(i);

		window->addCharacterCallback(Gaff::Func(HandleKeyboardCharacterInput));
		window->addKeyCallback(Gaff::Func(HandleKeyboardInput));
		window->addMouseEnterLeaveCallback(Gaff::Func(HandleMouseEnterLeave));
		window->addMouseButtonCallback(Gaff::Func(HandleMouseButtonInput));
		window->addMouseWheelCallback(Gaff::Func(HandleMouseWheelInput));
		window->addMousePosCallback(Gaff::Func(HandleMousePosInput));
		window->addFocusCallback(Gaff::Func(HandleFocus));
		//window->addGamepadCallback();
	}

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

	if (!_layout->init(*_main_device, layout_description, std::size(layout_description), *_vertex_shader)) {
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
		Gleam::Color::Black,
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

	_mesh->setTopology(Gleam::IMesh::Topology::TriangleList);*/
	return true;
}

void DebugManager::renderDebugMenu(DebugMenuEntry& entry)
{
	if (entry.var) {
		switch (entry.type) {
			case DebugMenuEntry::Type::Var:
				if (entry.var->isFlags()) {
					const Refl::IEnumReflectionDefinition& ref_def = entry.var->getReflection().getEnumReflectionDefinition();
					const int32_t num_entries = ref_def.getNumEntries();

					if (ImGui::BeginMenu(reinterpret_cast<const char*>(entry.name.getBuffer()))) {
						for (int32_t i = 0; i < num_entries; ++i) {
							const HashStringView32<> flag_name = ref_def.getEntryNameFromIndex(i);
							bool value = entry.var->getFlagValue(entry.object, i);

							if (ImGui::MenuItem(reinterpret_cast<const char*>(flag_name.getBuffer()), nullptr, value)) {
								entry.var->setFlagValue(entry.object, i, !value);
							}
						}

						ImGui::EndMenu();
					}

				} else {
					bool* const var = reinterpret_cast<bool*>(entry.var->getData(entry.object));
					ImGui::MenuItem(reinterpret_cast<const char*>(entry.name.getBuffer()), nullptr, var);
				}
					break;

			case DebugMenuEntry::Type::Func:
				if (ImGui::MenuItem(reinterpret_cast<const char*>(entry.name.getBuffer()))) {
					entry.func->call(entry.object);
				}
				break;

			case DebugMenuEntry::Type::StaticFunc:
				if (ImGui::MenuItem(reinterpret_cast<const char*>(entry.name.getBuffer()))) {
					entry.static_func->call();
				}
				break;

			case DebugMenuEntry::Type::FuncImGuiUpdate: {
				const bool always_render = entry.flags.testAll(DebugMenuEntry::Flag::AlwaysRender);
				bool updating = entry.flags.testAll(DebugMenuEntry::Flag::Updating);

				const U8String always_render_name = entry.name.getString() + u8" (Always Render)";

				if (ImGui::MenuItem(reinterpret_cast<const char*>(always_render_name.data()), nullptr, always_render)) {
					entry.flags.set(!always_render, DebugMenuEntry::Flag::AlwaysRender);
				}

				if (ImGui::MenuItem(reinterpret_cast<const char*>(entry.name.getBuffer()), nullptr, updating)) {
					updating = !updating;
					entry.flags.set(updating, DebugMenuEntry::Flag::Updating);

					entry.func->call(entry.object);

					if (updating) {
						_update_functions.emplace_back(&entry);

					} else {
						const auto it = Gaff::Find(_update_functions, &entry);
						GAFF_ASSERT(it != _update_functions.end());

						_update_functions.erase_unsorted(it);
					}
				}
			} break;

			case DebugMenuEntry::Type::StaticFuncImGuiUpdate:
				break;
		}

	} else {
		if (ImGui::BeginMenu(reinterpret_cast<const char*>(entry.name.getBuffer()))) {
			for (DebugMenuEntry& child_entry : entry.children) {
				renderDebugMenu(child_entry);
			}

			ImGui::EndMenu();
		}
	}
}

NS_END
