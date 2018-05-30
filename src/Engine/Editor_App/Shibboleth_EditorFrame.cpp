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

#include "Shibboleth_EditorFrame.h"
#include "Shibboleth_EditorWindowAttribute.h"
#include <Shibboleth_ReflectionManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gaff_JSON.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>

NS_SHIBBOLETH

wxBEGIN_EVENT_TABLE(EditorFrame, wxFrame)
	EVT_MENU(wxID_EXIT, EditorFrame::onExit)
	EVT_MENU(wxID_ABOUT, EditorFrame::onAbout)
wxEND_EVENT_TABLE()

EditorFrame::EditorFrame(const wxString& title, const wxPoint& pos, const wxSize& size):
	wxFrame(nullptr, wxID_ANY, title, pos, size)
{
	_menu_bar = new wxMenuBar;

	wxMenu* const menu_file = new wxMenu;
	menu_file->Append(wxID_EXIT, "E&xit");

	wxMenu* const menu_help = new wxMenu;
	menu_help->Append(wxID_ABOUT);

	_window_menu = new wxMenu;

	_menu_bar->Append(menu_file, "&File");
	_menu_bar->Append(_window_menu, "&Window");
	_menu_bar->Append(menu_help, "&Help");

	// Check reflection for editor windows.

	SetMenuBar(_menu_bar);
	CreateStatusBar();

	_aui_mgr.SetManagedWindow(this);
	_aui_mgr.SetFlags(
		wxAUI_MGR_ALLOW_FLOATING |
		wxAUI_MGR_TRANSPARENT_HINT |
		wxAUI_MGR_VENETIAN_BLINDS_HINT |
		wxAUI_MGR_RECTANGLE_HINT |
		wxAUI_MGR_HINT_FADE
	);

	_aui_mgr.Update();

	// Add all IEditorWindows to the Window menu.
	ReflectionManager& refl_mgr = GetApp().getReflectionManager();

	const Vector<const Gaff::IReflectionDefinition*> editor_windows = refl_mgr.getReflectionWithAttribute(Reflection<EditorWindowAttribute>::GetHash());

	for (const Gaff::IReflectionDefinition* const ref_def : editor_windows) {
		const EditorWindowAttribute* const ew_attr = ref_def->getClassAttribute<EditorWindowAttribute>();
		const int id = _next_id++;

		_window_menu->Append(id, ew_attr->getPath());

		Bind(
			wxEVT_MENU,
			&EditorFrame::onSpawnWindow,
			this,
			id,
			-1,
			// wxWidgets doesn't try to de-allocate this, so it's safe. Still kind of hacky.
			const_cast<wxObject*>(reinterpret_cast<const wxObject*>(ref_def))
		);
	}
}

EditorFrame::~EditorFrame(void)
{
	_aui_mgr.UnInit();
}

void EditorFrame::onExit(wxCommandEvent&)
{
	Close(true);
}

void EditorFrame::onAbout(wxCommandEvent&)
{
	wxMessageBox(
		"This is the Shibboleth Editor!\nEverything is WIP!",
		"About Shibboleth Editor",
		wxOK | wxICON_INFORMATION
	);
}

void EditorFrame::onSpawnWindow(wxCommandEvent& event)
{
	const Gaff::IReflectionDefinition* const ref_def = reinterpret_cast<Gaff::IReflectionDefinition*>(event.GetEventUserData());
	const EditorWindowAttribute* const ew_attr = ref_def->getClassAttribute<EditorWindowAttribute>();

	wxWindow* const window = ref_def->createAllocT<wxWindow>(Gaff::FNV1aHash64Const("wxWindow"), ARG_HASH(wxWindow*), GetAllocator(), this);
	wxAuiPaneInfo pane;

	pane.Caption(ew_attr->getCaption());
	pane.Float();

	_aui_mgr.AddPane(window, pane);
	_aui_mgr.Update();
}

NS_END
