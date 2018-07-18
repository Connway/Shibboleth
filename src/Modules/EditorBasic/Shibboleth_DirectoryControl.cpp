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

#include "Shibboleth_DirectoryControl.h"
#include <Gaff_Defines.h>
#include <wx/dir.h>

bool wxIsDriveAvailable(const wxString& dirName);

NS_SHIBBOLETH

DirectoryControl::DirectoryControl(
	wxWindow *parent,
	const wxWindowID id,
	const wxString& dir,
	const wxPoint& pos,
	const wxSize& size,
	long style,
	const wxString& filter,
	int defaultFilter,
	const wxString& name
):
	wxGenericDirCtrl()
{
	Init();
	Create(parent, id, dir, pos, size, style, filter, defaultFilter, name);
}

void DirectoryControl::reset(const wxString& path)
{
	GetTreeCtrl()->DeleteChildren(GetRootId());
	SetDefaultPath(path);
	ExpandRoot();
}

void DirectoryControl::SetupSections(void)
{
	// This may take a longish time. Go to busy cursor
	wxBusyCursor busy;

	wxDir dir;

	if (!openDir(dir)) {
		return;
	}

	const wxString path = GetDefaultPath();
	size_t index = path.find_last_of(wxFILE_SEP_PATH);

	if (index == SIZE_T_FAIL) {
		return;
	}

	const wxString name = path.substr(index + 1);
	wxDirItemData* const dir_item = new wxDirItemData(path, name, true);

	const wxTreeItemId id = AppendItem(GetRootId(), name, wxFileIconsTable::folder, -1, dir_item);
	ExpandDir(id);
}

void DirectoryControl::ExpandRoot(void)
{
	if (!HasFlag(DC_SINGLE_DEPTH)) {
		wxGenericDirCtrl::ExpandRoot();
		return;
	}

	// This may take a longish time. Go to busy cursor
	wxBusyCursor busy;

	wxDir dir;

	if (!openDir(dir)) {
		return;
	}

	// Add the first depth of folders.
	int style = wxDIR_DIRS;
	wxString file;
	
	if (GetShowHidden()) {
		style |= wxDIR_HIDDEN;
	}

	const wxTreeItemId root = GetRootId();
	const wxString path = GetDefaultPath();

	if (dir.GetFirst(&file, wxEmptyString, style)) {
		do {
			if (file == wxT(".") || file == wxT("..")) {
				continue;
			}

			wxDirItemData* const dir_item = new wxDirItemData(path + wxFILE_SEP_PATH + file, file, true);
			AppendItem(root, file, wxFileIconsTable::folder, -1, dir_item);
		} while (dir.GetNext(&file));
	}

	// Do the files now.
	if (HasFlag(wxDIRCTRL_DIR_ONLY)) {
		return;
	}

	style = wxDIR_FILES;

	if (GetShowHidden()) {
		style |= wxDIR_HIDDEN;
	}

	if (dir.GetFirst(&file, wxEmptyString, style)) {
		do {
			wxDirItemData* const dir_item = new wxDirItemData(path + wxFILE_SEP_PATH + file, file, true);
			AppendItem(root, file, wxFileIconsTable::file, -1, dir_item);
		} while (dir.GetNext(&file));
	}
}

bool DirectoryControl::openDir(wxDir& dir)
{
	wxString path = GetDefaultPath();

#if (defined(__WINDOWS__) && !defined(__WXWINCE__)) || defined(__DOS__) || defined(__OS2__)
	// Check if this is a root directory and if so,
	// whether the drive is available.
	if (!wxIsDriveAvailable(path))
	{
		static_cast<wxDirItemData*>(GetTreeCtrl()->GetItemData(GetRootId()))->m_isExpanded = false;
		//wxMessageBox(wxT("Sorry, this drive is not available."));
		return false;
	}
#endif

#if defined(__WINDOWS__) || defined(__DOS__) || defined(__OS2__)
	if (path.Last() == ':')
		path += wxString(wxFILE_SEP_PATH);
#endif

	return dir.Open(path);
}

NS_END
