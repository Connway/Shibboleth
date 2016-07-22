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

#include "Shibboleth_ImGuiManager.h"
#include "Shibboleth_InputManager.h"
#include <Shibboleth_IncludeImgui.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Window_Defines.h>
#include <Gleam_IKeyboard.h>

NS_SHIBBOLETH

REF_IMPL_REQ(ImGuiManager);
SHIB_REF_IMPL(ImGuiManager)
.addBaseClassInterfaceOnly<ImGuiManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

static size_t g_pool_index = GetPoolIndex("ImGui");

static void* ImGuiAllocate(size_t size)
{
	return ShibbolethAllocate(size, g_pool_index);
}

const char* ImGuiManager::GetFriendlyName(void)
{
	return "ImGui Manager";
}

ImGuiManager::ImGuiManager(void)
{
}

ImGuiManager::~ImGuiManager(void)
{
	ImGui::Shutdown();

	if (_imgui_context) {
		ImGui::DestroyContext(_imgui_context);
	}
}

const char* ImGuiManager::getName(void) const
{
	return GetFriendlyName();
}

void ImGuiManager::allManagersCreated(void)
{
	//GetApp().getManagerT<InputManager>().getKeyboard()->addCharacterHandler(Gaff::Bind(this, &ImGuiManager::handleCharacter));
}

void ImGuiManager::getUpdateEntries(Array<UpdateEntry>& /*entries*/)
{
	//entries.emplacePush(AString("ImGui Manager: Generate Draw Commands"), Gaff::Bind(this, &ImGuiManager::generateDrawCommands));
	//entries.emplacePush(AString("ImGui Manager: Submit Draw Commands"), Gaff::Bind(this, &ImGuiManager::submitDrawCommands));
}

bool ImGuiManager::init(void)
{
	_imgui_context = ImGui::CreateContext(ImGuiAllocate, ShibbolethFree);

	if (!_imgui_context) {
		return false;
	}

	ImGui::SetCurrentContext(_imgui_context);

	// Setup input codes
	ImGuiIO& io = ImGui::GetIO();

	io.KeyMap[ImGuiKey_Tab] = Gleam::KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = Gleam::KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = Gleam::KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = Gleam::KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = Gleam::KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = Gleam::KEY_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = Gleam::KEY_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = Gleam::KEY_HOME;
	io.KeyMap[ImGuiKey_End] = Gleam::KEY_END;
	io.KeyMap[ImGuiKey_Delete] = Gleam::KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = Gleam::KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = Gleam::KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = Gleam::KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = Gleam::KEY_A;
	io.KeyMap[ImGuiKey_C] = Gleam::KEY_C;
	io.KeyMap[ImGuiKey_V] = Gleam::KEY_V;
	io.KeyMap[ImGuiKey_X] = Gleam::KEY_X;
	io.KeyMap[ImGuiKey_Y] = Gleam::KEY_Y;
	io.KeyMap[ImGuiKey_Z] = Gleam::KEY_Z;

	return true;
}

void ImGuiManager::setImGuiContext(void)
{
	ImGui::SetCurrentContext(_imgui_context);
}

void ImGuiManager::generateDrawCommands(double /*dt*/, void* /*frame_data*/)
{
	
}

void ImGuiManager::submitDrawCommands(double, void* /*frame_data*/)
{
	
}

void ImGuiManager::handleCharacter(Gleam::IKeyboard*, unsigned int char_code)
{
	if (char_code > 0 && char_code < 0x10000)
		ImGui::GetIO().AddInputCharacter(static_cast<ImWchar>(char_code));
}

NS_END
