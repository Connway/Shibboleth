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

#include <Shibboleth_EditorInspectorAttribute.h>
#include <Shibboleth_ECSArchetype.h>
#include <Shibboleth_Broadcaster.h>
#include <Shibboleth_Reflection.h>

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

#include <wx/panel.h>

class wxEditableListBox;
class wxTreeItemId;
class wxTreeEvent;
class wxListEvent;
class wxTreeCtrl;

NS_SHIBBOLETH

class EditorFileSelectedMessage;
class RefDefItem;

class ArchetypeEditor final : public Gaff::IReflectionObject, public wxPanel
{
public:
	ArchetypeEditor(
		wxWindow* parent,
		wxWindowID id = wxID_ANY,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize
	);

	~ArchetypeEditor(void);

private:
	wxTreeCtrl* _ecs_components = nullptr;
	wxEditableListBox* _archetype_shared_ui = nullptr;
	wxEditableListBox* _archetype_ui = nullptr;

	Broadcaster& _broadcaster;
	U8String _path;

	void onFileSelected(const EditorFileSelectedMessage& message);

	void onRemoveSharedComponents(wxListEvent& event);
	void onRemoveComponents(wxListEvent& event);

	void onAddComponents(wxTreeEvent& event);
	void onDragBegin(wxTreeEvent& event);

	void onRemoveComponentsHelper(wxListEvent& event, wxEditableListBox* ui);

	RefDefItem* getItem(const wxTreeItemId& id) const;
	void removeItem(RefDefItem* item, wxEditableListBox* ui);
	void addItem(RefDefItem* item, wxEditableListBox* ui);
	void initComponentList(void);

	void save(void);
	void load(void);

	friend class ArcheTypeEditorDropTarget;

	SHIB_REFLECTION_CLASS_DECLARE(ArchetypeEditor);
};

NS_END

SHIB_REFLECTION_DECLARE(ArchetypeEditor)