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

#pragma once

#include <Shibboleth_Reflection.h>

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

#include <wx/treebase.h>
#include <wx/panel.h>

class wxTreeCtrl;
class wxListBox;

NS_SHIBBOLETH

class LoadedModulesWindow : public Gaff::IReflectionObject, public wxPanel
{
public:
	LoadedModulesWindow(
		wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize
	);

private:
	wxTreeCtrl* _reflection_tree = nullptr;
	wxListBox* _modules_list = nullptr;

	wxArrayString _reflection_types;
	wxArrayTreeItemIds _tree_ids;

	void initTree(void);
	void onModuleSelected(wxCommandEvent& event);

	SHIB_REFLECTION_CLASS_DECLARE(LoadedModulesWindow);
};

NS_END

SHIB_REFLECTION_DECLARE(LoadedModulesWindow)
