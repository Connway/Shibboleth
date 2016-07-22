/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Shibboleth_NuklearManager.h"
#include "Shibboleth_InputManager.h"
//#include <Shibboleth_IncludeImgui.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Window_Defines.h>
#include <Gleam_IKeyboard.h>
#include <Gleam_IMouse.h>

#define NK_IMPLEMENTATION
#include <Shibboleth_IncludeNuklear.h>

NS_SHIBBOLETH

REF_IMPL_REQ(NuklearManager);
SHIB_REF_IMPL(NuklearManager)
.addBaseClassInterfaceOnly<NuklearManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

static size_t g_pool_index = GetPoolIndex("Nuklear");
static Map<Gleam::KeyCode, nk_keys> g_key_map;
static bool g_first_init = true;

static void* NuklearAllocate(nk_handle, void*, nk_size size)
{
	return ShibbolethAllocate(size, g_pool_index);
}

static void NuklearFree(nk_handle, void* data)
{
	ShibbolethFree(data);
}

const char* NuklearManager::GetFriendlyName(void)
{
	return "Nuklear Manager";
}

NuklearManager::NuklearManager(void)
{
}

NuklearManager::~NuklearManager(void)
{
	//ImGui::Shutdown();

	//if (_imgui_context) {
	//	ImGui::DestroyContext(_imgui_context);
	//}
}

const char* NuklearManager::getName(void) const
{
	return GetFriendlyName();
}

void NuklearManager::allManagersCreated(void)
{
	InputManager& inputMgr = GetApp().getManagerT<InputManager>();
	Gleam::IKeyboard* keyboard = inputMgr.getKeyboard();
	Gleam::IMouse* mouse = inputMgr.getMouse();

	keyboard->addCharacterHandler(Gaff::Bind(this, &NuklearManager::handleCharacter));
	keyboard->addInputHandler(Gaff::Bind(this, &NuklearManager::handleKeyboard));
	mouse->addInputHandler(Gaff::Bind(this, &NuklearManager::handleMouse));
}

void NuklearManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(AString("Nuklear Manager: Generate Draw Commands"), Gaff::Bind(this, &NuklearManager::generateDrawCommands));
	entries.emplacePush(AString("Nuklear Manager: Submit Draw Commands"), Gaff::Bind(this, &NuklearManager::submitDrawCommands));
}

bool NuklearManager::init(void)
{
	if (g_first_init) {
		g_key_map[Gleam::KEY_LEFTSHIFT] = NK_KEY_SHIFT;
		g_key_map[Gleam::KEY_RIGHTSHIFT] = NK_KEY_SHIFT;
		g_key_map[Gleam::KEY_LEFTCONTROL] = NK_KEY_CTRL;
		g_key_map[Gleam::KEY_RIGHTCONTROL] = NK_KEY_CTRL;
		g_key_map[Gleam::KEY_DELETE] = NK_KEY_DEL;
		g_key_map[Gleam::KEY_ENTER] = NK_KEY_ENTER;
		g_key_map[Gleam::KEY_TAB] = NK_KEY_TAB;
		g_key_map[Gleam::KEY_BACKSPACE] = NK_KEY_BACKSPACE;
		g_key_map[Gleam::KEY_UP] = NK_KEY_UP;
		g_key_map[Gleam::KEY_DOWN] = NK_KEY_DOWN;
		g_key_map[Gleam::KEY_PAGEUP] = NK_KEY_SCROLL_UP;
		g_key_map[Gleam::KEY_PAGEDOWN] = NK_KEY_SCROLL_DOWN;

		g_first_init = false;
	}

	//_imgui_context = ImGui::CreateContext(ImGuiAllocate, ShibbolethFree);

	//if (!_imgui_context) {
	//	return false;
	//}

	//ImGui::SetCurrentContext(_imgui_context);

	//// Setup input codes
	//ImGuiIO& io = ImGui::GetIO();

	//io.KeyMap[ImGuiKey_Tab] = Gleam::KEY_TAB;
	//io.KeyMap[ImGuiKey_LeftArrow] = Gleam::KEY_LEFT;
	//io.KeyMap[ImGuiKey_RightArrow] = Gleam::KEY_RIGHT;
	//io.KeyMap[ImGuiKey_UpArrow] = Gleam::KEY_UP;
	//io.KeyMap[ImGuiKey_DownArrow] = Gleam::KEY_DOWN;
	//io.KeyMap[ImGuiKey_PageUp] = Gleam::KEY_PAGEUP;
	//io.KeyMap[ImGuiKey_PageDown] = Gleam::KEY_PAGEDOWN;
	//io.KeyMap[ImGuiKey_Home] = Gleam::KEY_HOME;
	//io.KeyMap[ImGuiKey_End] = Gleam::KEY_END;
	//io.KeyMap[ImGuiKey_Delete] = Gleam::KEY_DELETE;
	//io.KeyMap[ImGuiKey_Backspace] = Gleam::KEY_BACKSPACE;
	//io.KeyMap[ImGuiKey_Enter] = Gleam::KEY_ENTER;
	//io.KeyMap[ImGuiKey_Escape] = Gleam::KEY_ESCAPE;
	//io.KeyMap[ImGuiKey_A] = Gleam::KEY_A;
	//io.KeyMap[ImGuiKey_C] = Gleam::KEY_C;
	//io.KeyMap[ImGuiKey_V] = Gleam::KEY_V;
	//io.KeyMap[ImGuiKey_X] = Gleam::KEY_X;
	//io.KeyMap[ImGuiKey_Y] = Gleam::KEY_Y;
	//io.KeyMap[ImGuiKey_Z] = Gleam::KEY_Z;

	_allocator.alloc = NuklearAllocate;
	_allocator.free = NuklearFree;
	_allocator.userdata.ptr = nullptr;

	if (!nk_init(&_context, &_allocator, nullptr)) {
		// log error
		return false;
	}

	return true;
}

//void NuklearManager::setImGuiContext(void)
//{
//	ImGui::SetCurrentContext(_imgui_context);
//}

void NuklearManager::generateDrawCommands(double /*dt*/, void* /*frame_data*/)
{
	
}

void NuklearManager::submitDrawCommands(double, void* /*frame_data*/)
{
	
}

void NuklearManager::handleKeyboard(Gleam::IInputDevice* input_device, unsigned int input_code, float value)
{
	auto it = g_key_map.findElementWithKey(static_cast<Gleam::KeyCode>(input_code));

	if (it == g_key_map.end()) {
		Gleam::IKeyboard* keyboard = reinterpret_cast<Gleam::IKeyboard*>(input_device);
		bool ctrl_down = keyboard->isKeyDown(Gleam::KEY_LEFTCONTROL) || keyboard->isKeyDown(Gleam::KEY_RIGHTCONTROL);

		if (input_code == Gleam::KEY_LEFT) {
			nk_keys key = (ctrl_down) ? NK_KEY_TEXT_WORD_LEFT : NK_KEY_LEFT;
			nk_input_key(&_context, key, value > 0.0f);
		} else if (input_code == Gleam::KEY_RIGHT) {
			nk_keys key = (ctrl_down) ? NK_KEY_TEXT_WORD_RIGHT : NK_KEY_RIGHT;
			nk_input_key(&_context, key, value > 0.0f);
		} else if (input_code == Gleam::KEY_HOME) {
			nk_input_key(&_context, NK_KEY_SCROLL_START, value > 0.0f);
			nk_input_key(&_context, NK_KEY_TEXT_START, value > 0.0f);
		} else if (input_code == Gleam::KEY_END) {
			nk_input_key(&_context, NK_KEY_SCROLL_END, value > 0.0f);
			nk_input_key(&_context, NK_KEY_TEXT_END, value > 0.0f);
		} else if (input_code == Gleam::KEY_Z) {
			bool shift_down = keyboard->isKeyDown(Gleam::KEY_LEFTSHIFT) || keyboard->isKeyDown(Gleam::KEY_RIGHTSHIFT);

			if (ctrl_down && shift_down) {
				nk_input_key(&_context, NK_KEY_TEXT_REDO, value > 0.0f);
			} else if (ctrl_down) {
				nk_input_key(&_context, NK_KEY_TEXT_UNDO, value > 0.0f);
			}

		} else if (input_code == Gleam::KEY_X) {
			if (ctrl_down) {
				nk_input_key(&_context, NK_KEY_CUT, value > 0.0f);
			}

		} else if (input_code == Gleam::KEY_C) {
			if (ctrl_down) {
				nk_input_key(&_context, NK_KEY_COPY, value > 0.0f);
			}

		} else if (input_code == Gleam::KEY_V) {
			if (ctrl_down) {
				nk_input_key(&_context, NK_KEY_PASTE, value > 0.0f);
			}

		} else if (input_code == Gleam::KEY_Y) {
			if (ctrl_down) {
				nk_input_key(&_context, NK_KEY_TEXT_REDO, value > 0.0f);
			}
		}

	} else {
		nk_input_key(&_context, it->second, value > 0.0f);
	}
}

void NuklearManager::handleMouse(Gleam::IInputDevice* input_device, unsigned int input_code, float value)
{
	Gleam::IMouse* mouse = reinterpret_cast<Gleam::IMouse*>(input_device);
	int x = 0;
	int y = 0;

	mouse->getRelativePosition(x, y);

	//nk_input_button(&_context, button, x, y, value > 0.0f);
	(input_code);
	(value);
}

void NuklearManager::handleCharacter(Gleam::IKeyboard*, unsigned int char_code)
{
	if (char_code >= 32) {
		nk_input_unicode(&_context, static_cast<nk_rune>(char_code));
	}
}

NS_END
