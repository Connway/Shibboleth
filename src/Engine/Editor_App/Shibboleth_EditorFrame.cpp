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
#include <Shibboleth_IEditor.h>
#include <Shibboleth_IApp.h>
#include <Gaff_JSON.h>
#include <EASTL/sort.h>
#include <wx/msgdlg.h>
#include <wx/menu.h>

NS_SHIBBOLETH

wxBEGIN_EVENT_TABLE(EditorFrame, wxFrame)
	EVT_MENU(wxID_EXIT, EditorFrame::onExit)
	EVT_MENU(wxID_ABOUT, EditorFrame::onAbout)
	EVT_AUI_PANE_CLOSE(EditorFrame::onWindowClose)
wxEND_EVENT_TABLE()

EditorFrame::EditorFrame(const wxString& title, const wxPoint& pos, const wxSize& size):
	wxFrame(nullptr, wxID_ANY, title, pos, size)
{
	_menu_bar = new wxMenuBar;

	wxMenu* const menu_file = new wxMenu;
	menu_file->Append(wxID_EXIT, "E&xit");

	wxMenu* const menu_help = new wxMenu;
	menu_help->Append(wxID_ABOUT);

	_menu_bar->Append(menu_file, "&File");
	initMenu();
	_menu_bar->Append(menu_help, "&Help");

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

	//Bind(wxEVT_UPDATE_UI, &EditorFrame::onUpdate, this, GetId());
}

EditorFrame::~EditorFrame(void)
{
	_aui_mgr.UnInit();
}

void EditorFrame::spawnWindow(const Gaff::IReflectionDefinition* ref_def)
{
	const EditorWindowAttribute* const ew_attr = ref_def->getClassAttr<EditorWindowAttribute>();
	GAFF_ASSERT(ew_attr);

	Gaff::IReflectionObject* const instance = ref_def->createT<Gaff::IReflectionObject>(
		CLASS_HASH(Gaff::IReflectionObject),
		ARG_HASH(wxWindow*),
		ProxyAllocator::GetGlobal(),
		this
	);

	GAFF_ASSERT(instance);
	wxWindow* const window = ref_def->getInterface<wxWindow>(CLASS_HASH(wxWindow), instance->getBasePointer());
	GAFF_ASSERT(window);

	wxAuiPaneInfo pane;

	U8String name = ew_attr->getName();
	Gaff::EraseAllOccurences(name, '&');

	pane.Caption(name.data());
	pane.BestSize(wxSize(800, 600));
	pane.DestroyOnClose();
	pane.Float();

	_aui_mgr.AddPane(window, pane);
	_aui_mgr.Update();

	IEditor* const editor = GetApp().getEditor();
	editor->addEditorWindow(instance);
	window->Show();
}

// This function is not very pretty, but it gets the job done.
void EditorFrame::initMenu(void)
{
	Gaff::JSON group_cfg;

	if (!group_cfg.parseFile("cfg/editor_menus.cfg")) {
		// $TODO: Log error.
		return;
 	}

	if (!group_cfg.isObject()) {
		// $TODO: Log error
		return;
	}

	using GroupData = eastl::pair<wxMenu*, const Gaff::IReflectionDefinition*>;

	ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	VectorMap< HashString32, Vector<GroupData> > group_map;
	VectorMap<HashString32, wxMenu*> group_menus;

	// Populate group_map with all ref_defs.
	const Vector<const Gaff::IReflectionDefinition*> editor_windows = refl_mgr.getReflectionWithAttribute<EditorWindowAttribute>();

	for (const Gaff::IReflectionDefinition* const ref_def : editor_windows) {
		const EditorWindowAttribute* const ew_attr = ref_def->getClassAttr<EditorWindowAttribute>();

		const char* const group_name = ew_attr->getGroup();
		auto& group = group_map[HashString32(group_name)];

		group.emplace_back(eastl::make_pair(nullptr, ref_def));
	}

	// Populate group_map and group_menus with menus.
	group_cfg.forEachInObject([&](const char* name, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			// $TODO: Log error
			return false;
		}

		const Gaff::JSON groups = value["Groups"];

		// Menu with no groups has no entries, don't process.
		if (!groups.isArray()) {
			// $TODO: Log error
			return false;
		}

		const Gaff::JSON parent_group = value["Parent Group"];
		wxMenu* const menu = new wxMenu();

		// Add to group list for our parent group.
		if (parent_group.isString()) {
			const HashString32 parent_group_name(parent_group.getString());

			group_map[parent_group_name].emplace_back(eastl::make_pair(menu, nullptr));
			menu->SetClientData(reinterpret_cast<void*>(const_cast<char*>(name)));

		// No parent group, add to menu bar.
		} else {
			_menu_bar->Append(menu, name);
		}

		groups.forEachInArray([&](int32_t, const Gaff::JSON& group_value) -> bool
		{
			if (!group_value.isString()) {
				// $TODO: Log error
				return false;
			}

			const HashString32 group_name(group_value.getString());

			// Cyclical or duplicate group reference.
			if (group_menus[group_name]) {
				// $TODO: Log error
				return false;
			}

			group_menus[group_name] = menu;
			return false;
		});

		return false;
	});

	// Sort each group list.
	for (auto& data : group_map) {
		eastl::sort(data.second.begin(), data.second.end(), [](const GroupData& lhs, const GroupData& rhs) -> bool
		{
			const EditorWindowAttribute* const lhs_attr = (lhs.second) ? lhs.second->getClassAttr<EditorWindowAttribute>() : nullptr;
			const EditorWindowAttribute* const rhs_attr = (rhs.second) ? rhs.second->getClassAttr<EditorWindowAttribute>() : nullptr;

			U8String lhs_name = (lhs_attr) ? lhs_attr->getName() : reinterpret_cast<const char*>(lhs.first->GetClientData());
			U8String rhs_name = (rhs_attr) ? rhs_attr->getName() : reinterpret_cast<const char*>(rhs.first->GetClientData());

			Gaff::EraseAllOccurences(lhs_name, '&');
			Gaff::EraseAllOccurences(rhs_name, '&');

			return lhs_name < rhs_name;
		});
	}

	// For each group, and the list of entries to the appropriate menu.
	for (const auto& group_pair : group_map) {
		wxMenu* const menu = group_menus[group_pair.first];

		// Add a separator if there are things to separate.
		if (menu->GetMenuItemCount() > 0 && !group_pair.second.empty()) {
			menu->AppendSeparator();
		}

		for (const auto& group_data : group_pair.second) {
			// Add the submenu and clear the client data.
			if (group_data.first) {
				menu->AppendSubMenu(group_data.first, reinterpret_cast<const char*>(group_data.first->GetClientData()));
				group_data.first->SetClientData(nullptr);

			// Add entry and bind the button.
			} else {
				const EditorWindowAttribute* const ew_attr = group_data.second->getClassAttr<EditorWindowAttribute>();
				const int id = menu->Append(wxID_ANY, ew_attr->getName())->GetId();

				Bind(
					wxEVT_MENU,
					&EditorFrame::onSpawnWindow,
					this,
					id,
					-1,
					// wxWidgets doesn't try to de-allocate this, so it's safe. Still kind of hacky.
					const_cast<wxObject*>(reinterpret_cast<const wxObject*>(group_data.second))
				);
			}
		}
	}
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
	spawnWindow(ref_def);
}

void EditorFrame::onWindowClose(wxAuiManagerEvent& event)
{
	Gaff::IReflectionObject* const instance = reinterpret_cast<Gaff::IReflectionObject*>(reinterpret_cast<char*>(event.GetPane()->window) - sizeof(Gaff::IReflectionObject));
	GetApp().getEditor()->removeEditorWindow(instance);
}

//void EditorFrame::onUpdate(wxUpdateUIEvent& /*event*/)
//{
//}

NS_END
