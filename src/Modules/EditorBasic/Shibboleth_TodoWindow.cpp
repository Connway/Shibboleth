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

#include "Shibboleth_TodoWindow.h"
#include <Shibboleth_EditorWindowAttribute.h>
#include <Shibboleth_Utilities.h>
#include <Gaff_JSON.h>

#include <wx/listctrl.h>
#include <wx/sizer.h>

#include <filesystem>

SHIB_REFLECTION_DEFINE(TodoWindow)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TodoWindow)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(wxWindow*, wxWindowID)
	.CTOR(wxWindow*)

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)

	.classAttrs(
		EditorWindowAttribute("&Todo Window", "Window_Editors")
	)
SHIB_REFLECTION_CLASS_DEFINE_END(TodoWindow)

TodoWindow::TodoWindow(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
) :
	wxPanel(parent, id, pos, size)
{
	_list_view = new wxListView(this);
	_list_view->AppendColumn("File", wxLIST_FORMAT_LEFT, 400);
	_list_view->AppendColumn("Comment", wxLIST_FORMAT_LEFT, 400);

	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(_list_view, 1, wxEXPAND | wxALL);
	sizer->SetSizeHints(this);

	SetSizer(sizer);

	const char* const dir = GetApp().getConfigs()["source_dir"].getString("../src");

	if (std::filesystem::is_directory(dir)) {
		_fs_watcher.AddTree(wxFileName(dir));
		_fs_watcher.SetOwner(this);

		Bind(wxEVT_FSWATCHER, &TodoWindow::fileChanged, this);
	}
}

TodoWindow::~TodoWindow(void)
{
}

void TodoWindow::fileChanged(const wxFileSystemWatcherEvent& event)
{
	GAFF_REF(event);
	int i = 0;
	i += 5;
}

NS_END
