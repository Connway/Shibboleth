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
#include "Shibboleth_IInputManager.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Window_Defines.h>
#include <Gleam_IKeyboard.h>
#include <Gleam_IMouse.h>

NS_SHIBBOLETH

REF_IMPL_REQ(NuklearManager);
SHIB_REF_IMPL(NuklearManager)
.addBaseClassInterfaceOnly<NuklearManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(INuklearManager)
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
	return INuklearManager::GetFriendlyName();
}

NuklearManager::NuklearManager(void)
{
}

NuklearManager::~NuklearManager(void)
{
	if (_init) {
		nk_free(&_context);
	}
}

const char* NuklearManager::getName(void) const
{
	return GetFriendlyName();
}

void NuklearManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.emplacePush(U8String("Nuklear Manager: Generate Draw Commands"), Gaff::Bind(this, &NuklearManager::generateDrawCommands));
	entries.emplacePush(U8String("Nuklear Manager: Submit Draw Commands"), Gaff::Bind(this, &NuklearManager::submitDrawCommands));
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

	IInputManager& inputMgr = GetApp().getManagerT<IInputManager>();
	Gleam::IKeyboard* keyboard = inputMgr.getKeyboard();
	Gleam::IMouse* mouse = inputMgr.getMouse();

	keyboard->addCharacterHandler(Gaff::Bind(this, &NuklearManager::handleCharacter));
	keyboard->addInputHandler(Gaff::Bind(this, &NuklearManager::handleKeyboard));
	mouse->addInputHandler(Gaff::Bind(this, &NuklearManager::handleMouse));

	_allocator.alloc = NuklearAllocate;
	_allocator.free = NuklearFree;
	_allocator.userdata.ptr = nullptr;

	if (!nk_init(&_context, &_allocator, nullptr)) {
		// log error
		return false;
	}

	_init = true;
	return true;
}

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

	switch (input_code) {
		case Gleam::MOUSE_LEFT:
			nk_input_button(&_context, NK_BUTTON_LEFT, x, y, value > 0.0f);
			break;

		case Gleam::MOUSE_RIGHT:
			nk_input_button(&_context, NK_BUTTON_RIGHT, x, y, value > 0.0f);
			break;

		case Gleam::MOUSE_MIDDLE:
			nk_input_button(&_context, NK_BUTTON_MIDDLE, x, y, value > 0.0f);
			break;
	}
}

void NuklearManager::handleCharacter(Gleam::IKeyboard*, unsigned int char_code)
{
	if (char_code >= 32) {
		nk_input_unicode(&_context, static_cast<nk_rune>(char_code));
	}
}

NS_END
