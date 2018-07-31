/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_Editor.h"
#include <Shibboleth_EditorFileHandlerAttribute.h>
#include <Shibboleth_EditorFrame.h>
#include <Shibboleth_Utilities.h>

wxIMPLEMENT_APP(Shibboleth::Editor);

NS_SHIBBOLETH

bool Editor::OnInit(void)
{
	Shibboleth::SetApp(_engine_instance);

	const char* args[] = { "cfg/editor.cfg" };
	const bool success = _engine_instance.init(1, args);

	if (!success) {
		return false;
	}

	_engine_instance.setEditor(this);

	EditorFrame* const frame = new EditorFrame("Shibboleth Editor", wxDefaultPosition, wxSize(1024, 768));
	return frame && frame->Show(true);
}

void Editor::CleanUp(void)
{
	_engine_instance.destroy();
	wxApp::CleanUp();
}

void Editor::addEditorWindow(Gaff::IReflectionObject* window)
{
	_editor_windows.emplace_back(window);
}

void Editor::removeEditorWindow(Gaff::IReflectionObject* window)
{
	const auto it = Gaff::Find(_editor_windows, window);

	if (it != _editor_windows.end()) {
		_editor_windows.erase_unsorted(it);
	}
}

void Editor::openEditorWindow(const char* file_extension)
{
	for (const Gaff::IReflectionObject* ref_obj : _editor_windows) {
		const auto result = ref_obj->getReflectionDefinition().getFuncAttr<EditorFileHandlerAttribute>();

		// Already open.
		if (result.second) {
			return;
		}
	}

	const auto result = GetApp().getReflectionManager().getReflectionWithAttribute<EditorFileHandlerAttribute>();
	const size_t size = eastl::CharStrlen(file_extension);

	for (const Gaff::IReflectionDefinition* ref_def : result) {
		if (!eastl::Compare(file_extension, ref_def->getClassAttr<EditorFileHandlerAttribute>()->getExtension(), size)) {
			// Found the window type. Now create it.
			_frame->spawnWindow(ref_def);
			break;
		}
	}
}

NS_END
