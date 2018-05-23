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

	EditorFrame* const frame = new EditorFrame("Shibboleth Editor", wxDefaultPosition, wxSize(800, 600));
	return frame && frame->Show(true);
}

void Editor::CleanUp(void)
{
	_engine_instance.destroy();
	wxApp::CleanUp();
}

NS_END
