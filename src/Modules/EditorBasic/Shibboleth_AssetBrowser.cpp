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

#include "Shibboleth_AssetBrowser.h"
#include <Shibboleth_EditorWindowAttribute.h>
#include "Shibboleth_DirectoryControl.h"
#include <wx/splitter.h>
#include <wx/sizer.h>
#include <filesystem>

SHIB_REFLECTION_EXTERNAL_DEFINE(Shibboleth::AssetBrowser)

SHIB_REFLECTION_BUILDER_BEGIN(Shibboleth::AssetBrowser)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(wxWindow*, wxWindowID)
	.CTOR(wxWindow*)

	.BASE(wxWindow)

	.classAttrs(
		EditorWindowAttribute("Asset &Browser", "Asset Browser")
	)
SHIB_REFLECTION_BUILDER_END(Shibboleth::ArchetypeEditor)

NS_SHIBBOLETH

AssetBrowser::AssetBrowser(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size)
{
	wxSplitterWindow* const splitter = new wxSplitterWindow(this);
	splitter->SetWindowStyleFlag(wxSP_3D | wxSP_LIVE_UPDATE);

	const std::filesystem::path abs_path = std::filesystem::absolute((GetApp().getProjectDirectory() + "/Resources").c_str());
	_dir_ctrl = new DirectoryControl(splitter, wxID_ANY, abs_path.c_str(), wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL | wxDIRCTRL_DIR_ONLY);
	_file_ctrl = new DirectoryControl(splitter, wxID_ANY, abs_path.c_str(), wxDefaultPosition, wxDefaultSize, wxDIRCTRL_3D_INTERNAL | DC_SINGLE_DEPTH);

	splitter->SplitVertically(_dir_ctrl, _file_ctrl, 300);

	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(splitter, 1, wxEXPAND | wxALL);
	sizer->SetSizeHints(this);

	SetSizer(sizer);

	Bind(wxEVT_DIRCTRL_SELECTIONCHANGED, &AssetBrowser::onDirectorySelection, this, _dir_ctrl->GetId());
	Bind(wxEVT_DIRCTRL_SELECTIONCHANGED, &AssetBrowser::onFileSelection, this, _file_ctrl->GetId());
	Bind(wxEVT_TREE_ITEM_ACTIVATED, &AssetBrowser::onFileActivated, this, _file_ctrl->GetTreeCtrl()->GetId());
}

AssetBrowser::~AssetBrowser(void)
{
}

void AssetBrowser::onDirectorySelection(wxTreeEvent& event)
{
	wxDirItemData* const data = reinterpret_cast<wxDirItemData*>(_dir_ctrl->GetTreeCtrl()->GetItemData(event.GetItem()));
	_file_ctrl->reset(data->m_path);
}

void AssetBrowser::onFileSelection(wxTreeEvent& event)
{
	wxDirItemData* const data = reinterpret_cast<wxDirItemData*>(_file_ctrl->GetTreeCtrl()->GetItemData(event.GetItem()));

	if (data->m_isDir) {
		return;
	}

	// If it's a file, send an event out that it has been selected.
}

void AssetBrowser::onFileActivated(wxTreeEvent& event)
{
	wxDirItemData* const data = reinterpret_cast<wxDirItemData*>(_file_ctrl->GetTreeCtrl()->GetItemData(event.GetItem()));
	
	if (!data->m_isDir) {
		return;
	}

	_dir_ctrl->SelectPath(data->m_path);
}

NS_END
