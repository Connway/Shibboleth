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

#include <Shibboleth_Reflection.h>

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

//#include <wx/treebase.h>
#include <wx/panel.h>
#include <wx/dnd.h>

class wxTreeItemId;
class wxTreeEvent;
class wxTreeCtrl;
class wxListBox;

NS_SHIBBOLETH

class RefDefItem;

class ArchetypeEditor final : public wxPanel, public wxDropTarget, public Gaff::IReflectionObject
{
public:
	ArchetypeEditor(
		wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize
	);

	~ArchetypeEditor(void);

	wxDragResult OnData(wxCoord x, wxCoord y, wxDragResult result) override;

private:
	wxTreeCtrl* _ecs_components = nullptr;
	wxListBox* _archetype = nullptr;

	void onAddComponents(wxTreeEvent& event);
	void onDragBegin(wxTreeEvent& event);

	RefDefItem* getItem(const wxTreeItemId& id) const;
	void addItem(RefDefItem* item);
	void initComponentList(void);

	SHIB_REFLECTION_CLASS_DECLARE(ArchetypeEditor);
};

NS_END

SHIB_REFLECTION_DECLARE(ArchetypeEditor)
