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

SHIB_REFLECTION_DEFINE_BEGIN(DebugSystem)
	.base<ISystem>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(DebugSystem)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DebugManager)
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

	RenderManagerBase& rm = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	const Gleam::IWindow* const window = rm.getWindow("main");

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

	return true;
}

void DebugManager::update(void)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = _time->getDeltaFloat();

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
}

ImGuiContext* DebugManager::getImGuiContext(void) const
{
	return ImGui::GetCurrentContext();
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
