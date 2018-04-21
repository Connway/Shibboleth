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

#include "Shibboleth_ModulesWindow.h"
#include <Shibboleth_App.h>
#include <Gaff_JSON.h>

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

#include <wx/treectrl.h>
#include <wx/sizer.h>

NS_SHIBBOLETH

ModulesWindow::ModulesWindow(wxWindow* parent, App& app):
	wxPanel(parent), _app(app)
{
	_tree = new wxTreeCtrl(this);
	_tree->SetWindowStyleFlag(wxTR_HIDE_ROOT);
	_tree->AddRoot(wxT(""));

	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

	sizer->Add(_tree, 1, wxEXPAND | wxALL, 5);

	sizer->SetSizeHints(this);
	SetSizer(sizer);

	initTree();
}

void ModulesWindow::initTree(void)
{
	const wxTreeItemId root_id = _tree->GetRootItem();

	Gaff::JSON config;

	if (config.parseFile("cfg/editor.cfg")) {
		const Gaff::JSON module_display = config["module_display"];

		if (module_display.isArray()) {
			const ReflectionManager& refl_mgr = _app.getReflectionManager();
			_module_names.Clear();
			_refl_names.Clear();

			for (int32_t i = 0; i < module_display.size(); ++i) {
				const Gaff::JSON entry = module_display[i];
				const Gaff::JSON refl_name = entry["reflection_name"];
				const Gaff::JSON name = entry["name"];

				_module_names.Add(name.getString());
				_refl_names.Add(refl_name.getString());


				const Vector<const Gaff::IReflectionDefinition*>* const bucket = refl_mgr.getTypeBucket(Gaff::FNV1aHash64String(refl_name.getString()));

				if (!bucket) {
					continue;
				}

				const wxTreeItemId id = _tree->AppendItem(root_id, wxString(name.getString()));

				for (const Gaff::IReflectionDefinition* ref_data : *bucket) {
					_tree->AppendItem(id, wxString(ref_data->getReflectionInstance().getName()));
				}
			}
		}
	}
}

NS_END
