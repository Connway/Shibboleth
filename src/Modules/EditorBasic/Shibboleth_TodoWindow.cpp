/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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
#include <Gaff_File.h>

#include <wx/listctrl.h>
#include <wx/sizer.h>

#include <filesystem>

SHIB_REFLECTION_DEFINE(TodoWindow)

NS_SHIBBOLETH

static int wxCALLBACK TodoSort(wxIntPtr item1, wxIntPtr item2, wxIntPtr data)
{
	const wxListView* const list_view = reinterpret_cast<wxListView*>(data);
	const wxString text1 = list_view->GetItemText(item1, 1);
	const wxString text2 = list_view->GetItemText(item2, 1);

	if (text1 < text2) {
		return -1;
	} else if (text1 > text2) {
		return 1;
	}

	return 0;
}

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
	_list_view->AppendColumn("Line", wxLIST_FORMAT_LEFT, 60);
	_list_view->AppendColumn("File", wxLIST_FORMAT_LEFT, 340);
	_list_view->AppendColumn("Comment", wxLIST_FORMAT_LEFT, 400);

	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(_list_view, 1, wxEXPAND | wxALL);
	sizer->SetSizeHints(this);

	SetSizer(sizer);

	const Gaff::JSON dirs = GetApp().getConfigs()["todo_watch_dirs"];

	if (!dirs.isArray()) {
		return;
	}

	dirs.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool
	{
		if (!value.isString()) {
			// $TODO: Log error
			return false;
		}

		const char* const dir = value.getString();

		if (!std::filesystem::is_directory(dir)) {
			// $TODO: Log error
			return false;
		}

		// Makes sure the path ends with a '/'.
		const wxString str = (Gaff::EndsWith(dir, "/", 1)) ? dir : wxString(dir) + "/";

		wxFileName path(str);
		path.MakeAbsolute();

		const wxString path_string = path.GetFullPath();

		_fs_watcher.AddTree(path, wxFSW_EVENT_ALL);
		_fs_watcher.SetOwner(this);

		Bind(wxEVT_FSWATCHER, &TodoWindow::fileChanged, this);

		// $TODO: Maybe think about threading the initial populate so this window doesn't stall when opening on large codebases.
		initialPopulate(path_string.c_str());
		return false;
	});

	_list_view->SortItems(TodoSort, reinterpret_cast<wxIntPtr>(_list_view));
}

TodoWindow::~TodoWindow(void)
{
}

void TodoWindow::fileChanged(const wxFileSystemWatcherEvent& event)
{
	const wxFileName& path = event.GetPath();
	wxString full_path = path.GetFullPath();
	full_path.Replace("\\", "/");

	const char* const file_name = full_path.c_str();

	// Filter out files we can't inspect.
	if (!canParseFile(file_name)) {
		return;
	}

	switch (event.GetChangeType()) {
		case wxFSW_EVENT_CREATE:
			parseFile(file_name);
			_list_view->SortItems(TodoSort, reinterpret_cast<wxIntPtr>(_list_view));
			break;

		case wxFSW_EVENT_DELETE:
			removeFile(file_name);
			break;

		case wxFSW_EVENT_RENAME: {
			wxString new_full_path = event.GetNewPath().GetFullPath();
			new_full_path.Replace("\\", "/");

			if (!canParseFile(new_full_path.c_str())) {
				removeFile(file_name);
				return;
			}

			long item_index = _list_view->FindItem(-1, file_name);

			while (item_index > -1) {
				_list_view->SetItem(item_index, 1, new_full_path);
				item_index = _list_view->FindItem(-1, file_name);
			}

			_list_view->SortItems(TodoSort, reinterpret_cast<wxIntPtr>(_list_view));
		} break;

		case wxFSW_EVENT_MODIFY:
			parseFile(file_name);
			_list_view->SortItems(TodoSort, reinterpret_cast<wxIntPtr>(_list_view));
			break;
	}
}

bool TodoWindow::canParseFile(const char* file_name) const
{
	return Gaff::EndsWith(file_name, ".h") || Gaff::EndsWith(file_name, ".cpp") || Gaff::EndsWith(file_name, ".cfg");
}

void TodoWindow::initialPopulate(const char* path)
{
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		const wchar_t* name = entry.path().c_str();
		CONVERT_STRING(char, file_name, name);

		for (int32_t i = 0; file_name[i]; ++i) {
			if (file_name[i] == '\\') {
				file_name[i] = '/';
			}
		}

		if (entry.is_directory() && !Gaff::EndsWith(file_name, "Dependencies")) {
			initialPopulate(file_name);
			continue;
		}

		if (!entry.is_regular_file() || !canParseFile(file_name)) {
			continue;
		}

		parseFile(file_name);
	}
}

long TodoWindow::removeFile(const char* file_name)
{
	const int32_t num_items = _list_view->GetItemCount();
	int32_t start_index = -1;

	for (int32_t i = 0; i < num_items; ++i) {
		if (_list_view->GetItemText(i, 1) == file_name) {
			start_index = i;
			break;
		}
	}

	if (start_index != -1) {
		do {
			_list_view->DeleteItem(start_index);
		} while (start_index < _list_view->GetItemCount() && _list_view->GetItemText(start_index, 1) == file_name);
	}

	return start_index;
}

void TodoWindow::parseFile(const char* file_name)
{
	long insert_index = removeFile(file_name);
	insert_index = (insert_index> -1) ? insert_index : _list_view->GetItemCount();

	Gaff::File file(file_name);

	if (!file.isOpen()) {
		// $TODO: Log error
		return;
	}

	int32_t line_number = 0;
	char line[1024];

	while (file.readString(line, ARRAY_SIZE(line))) {
		size_t todo_index = Gaff::FindFirstOf(line, "$TODO");
		size_t prev_index = 0;

		const int32_t start_line = ++line_number;

		while (todo_index != SIZE_T_FAIL) {
			size_t walk_start = todo_index - 2;
			wxString todo_text;

			while (walk_start > prev_index) {
				// Single line comment.
				if (line[walk_start] == '/' && line[walk_start + 1] == '/') {
					todo_text = line + walk_start + 2;
					break;
				}

				--walk_start;
			}

			if (todo_text.IsEmpty()) {
				// $TODO: Log error

			} else {
				todo_text.Trim(false);
				todo_text.Trim();

				_list_view->InsertItem(insert_index, wxString::Format(wxT("%i"), start_line));
				_list_view->SetItem(insert_index, 1, file_name);
				_list_view->SetItem(insert_index, 2, todo_text);
				_list_view->SetItemData(insert_index, insert_index);
				++insert_index;
			}

			todo_index = Gaff::FindFirstOf(line + todo_index + 5, "$TODO");
		}
	}
}

NS_END
