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

#include "Shibboleth_LoadedModulesWindow.h"
#include <Shibboleth_EditorWindowAttribute.h>
#include <Shibboleth_IApp.h>
#include <Gaff_JSON.h>

#include <wx/treectrl.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/event.h>

SHIB_REFLECTION_EXTERNAL_DEFINE(Shibboleth::LoadedModulesWindow)

SHIB_REFLECTION_BUILDER_BEGIN(Shibboleth::LoadedModulesWindow)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(wxWindow*, wxWindowID)
	.CTOR(wxWindow*)

	.BASE(wxWindow)

	.classAttrs(
		EditorWindowAttribute("&Modules", "Loaded Modules")
	)

SHIB_REFLECTION_BUILDER_END(Shibboleth::LoadedModulesWindow)

NS_SHIBBOLETH

LoadedModulesWindow::LoadedModulesWindow(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size)
{
	_modules_list = new wxListBox(this, wxID_ANY);
	_modules_list->SetWindowStyleFlag(wxLB_SINGLE | wxLB_NEEDED_SB | wxLB_SORT);

	_modules_list->Append("All");

	for (const HashString64& module : GetApp().getReflectionManager().getModules()) {
		_modules_list->Append(module.getBuffer());
	}

	_reflection_tree = new wxTreeCtrl(this);
	_reflection_tree->SetWindowStyleFlag(wxTR_HIDE_ROOT);
	_reflection_tree->AddRoot(wxT(""));

	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

	sizer->Add(_modules_list, 1, wxEXPAND | wxALL, 5);
	sizer->Add(_reflection_tree, 1, wxEXPAND | wxALL, 5);

	sizer->SetSizeHints(this);
	SetSizer(sizer);

	Bind(
		wxEVT_LISTBOX,
		&LoadedModulesWindow::onModuleSelected,
		this
	);

	initTree();

	_modules_list->SetSelection(0);
}

void LoadedModulesWindow::initTree(void)
{
	const wxTreeItemId root_id = _reflection_tree->GetRootItem();

	Gaff::JSON config;

	const Gaff::JSON& module_display = GetApp().getConfigs()["module_display"];

	if (module_display.isArray()) {
		const ReflectionManager& refl_mgr = GetApp().getReflectionManager();

		for (int32_t i = 0; i < module_display.size(); ++i) {
			const Gaff::JSON entry = module_display[i];
			const Gaff::JSON refl_name = entry["reflection_name"];
			const Gaff::JSON name = entry["name"];

			_reflection_types.Add(refl_name.getString());

			const Vector<const Gaff::IReflectionDefinition*>* const bucket = refl_mgr.getTypeBucket(Gaff::FNV1aHash64String(refl_name.getString()));

			if (!bucket) {
				continue;
			}

			const wxTreeItemId id = _reflection_tree->AppendItem(root_id, name.getString());
			_tree_ids.Add(id);

			for (const Gaff::IReflectionDefinition* ref_def : *bucket) {
				_reflection_tree->AppendItem(id, ref_def->getReflectionInstance().getName());
			}
		}
	}
}

void LoadedModulesWindow::onModuleSelected(wxCommandEvent& event)
{
	const ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	const wxString module_name = event.GetString();
	const Gaff::Hash64 module_hash = Gaff::FNV1aHash64String(module_name.GetData().AsChar());
	const bool is_all_modules = module_name == "All";

	for (size_t i = 0; i < _reflection_types.size(); ++i) {
		const wxString& type = _reflection_types[i];
		const Gaff::Hash64 type_hash = Gaff::FNV1aHash64String(type.c_str().AsChar());
		const Vector<const Gaff::IReflectionDefinition*>* const bucket = (is_all_modules) ?
			refl_mgr.getTypeBucket(type_hash) :
			refl_mgr.getTypeBucket(type_hash, module_hash);

		const wxTreeItemId& id = _tree_ids[i];
		_reflection_tree->DeleteChildren(id);

		if (!bucket) {
			continue;
		}

		for (const Gaff::IReflectionDefinition* ref_def : *bucket) {
			_reflection_tree->AppendItem(id, ref_def->getReflectionInstance().getName());
		}
	}
}

NS_END
