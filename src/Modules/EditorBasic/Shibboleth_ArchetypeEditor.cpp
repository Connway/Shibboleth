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

#include "Shibboleth_ArchetypeEditor.h"
#include "Shibboleth_ECSAttributes.h"
#include <Shibboleth_EditorWindowAttribute.h>
#include <Shibboleth_EditorFileTypeHandler.h>
#include <Shibboleth_UniqueAttribute.h>

#include <wx/stattext.h>
#include <wx/treectrl.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/event.h>

SHIB_REFLECTION_DEFINE(ArchetypeEditor)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ArchetypeEditor)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(wxWindow*, wxWindowID)
	.CTOR(wxWindow*)

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)

	.classAttrs(
		EditorWindowAttribute("&Editors/&Archetype Editor", "Archetype Editor"),
		EditorFileTypeHandler(".archetype")
	)
SHIB_REFLECTION_CLASS_DEFINE_END(ArchetypeEditor)

constexpr static const char* const s_ref_def_format = "RefDefItemFormat";
static wxColour g_grey(127, 127, 127);

class RefDefItem final : public wxTreeItemData {
public:
	RefDefItem(const Gaff::IReflectionDefinition* ref_def): _ref_def(ref_def) {}

	const Gaff::IReflectionDefinition* getRefDef(void) const { return _ref_def; }

	bool isDisabled(void) const { return _disabled; }
	void setDisabled(bool disabled) { _disabled = disabled; }

private:
	const Gaff::IReflectionDefinition* const _ref_def = nullptr;
	bool _disabled = false;
};


ArchetypeEditor::ArchetypeEditor(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size)
{
	_ecs_components = new wxTreeCtrl(this, wxID_ANY);
	_ecs_components->SetWindowStyleFlag(wxTR_HIDE_ROOT | wxTR_MULTIPLE);
	_ecs_components->AddRoot(wxT(""));

	_archetype = new wxListBox(this, wxID_ANY);
	_archetype->SetWindowStyleFlag(wxLB_MULTIPLE);
	_archetype->SetDropTarget(this);

	wxStaticText* const archetype_text = new wxStaticText(this, wxID_ANY, "Archetype");
	wxStaticText* const component_text = new wxStaticText(this, wxID_ANY, "Components List");
	archetype_text->SetWindowStyleFlag(wxALIGN_CENTRE_HORIZONTAL | wxBORDER_THEME);
	component_text->SetWindowStyleFlag(wxALIGN_CENTRE_HORIZONTAL | wxBORDER_THEME);

	wxFlexGridSizer* const sizer = new wxFlexGridSizer(2, 2, wxSize(0, 0));
	sizer->Add(archetype_text, 1, wxEXPAND | wxALL, 1);
	sizer->Add(component_text, 1, wxEXPAND | wxALL, 1);

	sizer->Add(_archetype, 15, wxEXPAND | wxALL, 1);
	sizer->Add(_ecs_components, 15, wxEXPAND | wxALL, 1);

	sizer->AddGrowableRow(1, 15);
	sizer->AddGrowableCol(0, 1);
	sizer->AddGrowableCol(1, 1);

	sizer->SetSizeHints(this);
	SetSizer(sizer);

	initComponentList();

	Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &ArchetypeEditor::onAddComponents, this, _ecs_components->GetId());
	Bind(wxEVT_TREE_ITEM_ACTIVATED, &ArchetypeEditor::onAddComponents, this, _ecs_components->GetId());
	Bind(wxEVT_TREE_BEGIN_DRAG, &ArchetypeEditor::onDragBegin, this, _ecs_components->GetId());

	m_dataObject = new wxCustomDataObject(s_ref_def_format);
}

ArchetypeEditor::~ArchetypeEditor(void)
{
}

wxDragResult ArchetypeEditor::OnData(wxCoord /*x*/, wxCoord /*y*/, wxDragResult result)
{
	const wxCustomDataObject* const data = reinterpret_cast<wxCustomDataObject*>(m_dataObject);
	RefDefItem** const items = reinterpret_cast<RefDefItem** const>(data->GetData());
	const int32_t num_items = static_cast<int32_t>(data->GetDataSize()) / sizeof(RefDefItem*);

	for (int32_t i = 0; i < num_items && items[i]; ++i) {
		addItem(items[i]);
	}

	return result;
}

void ArchetypeEditor::onAddComponents(wxTreeEvent& event)
{
	wxArrayTreeItemIds ids;
	size_t size = _ecs_components->GetSelections(ids);

	if (ids.IsEmpty()) {
		RefDefItem* const item = getItem(event.GetItem());

		if (!item) {
			return;
		}

		addItem(item);

	} else {
		for (size_t i = 0; i < size; ++i) {
			RefDefItem* const item = getItem(ids[i]);

			if (!item) {
				return;
			}

			addItem(item);
		}
	}
}

void ArchetypeEditor::onDragBegin(wxTreeEvent& event)
{
	wxArrayTreeItemIds ids;
	size_t size = _ecs_components->GetSelections(ids);
	RefDefItem** ref_def_items = nullptr;

	if (ids.IsEmpty()) {
		RefDefItem* const item = getItem(event.GetItem());

		if (!item) {
			return;
		}

		ref_def_items = new RefDefItem*[1];
		ref_def_items[0] = item;
		size = 1;

	} else {
		ref_def_items = new RefDefItem*[size];
		int32_t index = 0;

		memset(ref_def_items, 0, sizeof(RefDefItem*) * size);

		for (size_t i = 0; i < size; ++i) {
			RefDefItem* const item = getItem(ids[i]);

			if (!item) {
				continue;
			}

			ref_def_items[index] = item;
			++index;
		}

		if (!index) {
			delete [] ref_def_items;
			return;
		}
	}

	reinterpret_cast<wxCustomDataObject*>(m_dataObject)->TakeData(sizeof(RefDefItem*) * size, ref_def_items);

	wxDropSource source(this);
	source.SetData(*m_dataObject);
	source.DoDragDrop();
}

RefDefItem* ArchetypeEditor::getItem(const wxTreeItemId& id) const
{
	if (_ecs_components->HasChildren(id)) {
		return nullptr;
	}

	RefDefItem* const item = reinterpret_cast<RefDefItem*>(_ecs_components->GetItemData(id));

	if (item->isDisabled()) {
		return nullptr;
	}

	return item;
}

void ArchetypeEditor::addItem(RefDefItem* item)
{
	const Gaff::IReflectionDefinition* const ref_def = item->getRefDef();

	if (ref_def->getClassAttribute<UniqueAttribute>()) {
		_ecs_components->SetItemTextColour(item->GetId(), g_grey);
		item->setDisabled(true);
	}

	_archetype->Append(ref_def->getReflectionInstance().getName(), reinterpret_cast<wxClientData*>(item));
}

void ArchetypeEditor::initComponentList(void)
{
	const auto comp_ref_defs = GetApp().getReflectionManager().getReflectionWithAttribute(Reflection<ECSClassAttribute>::GetHash());
	const wxTreeItemId root = _ecs_components->GetRootItem();

	for (const Gaff::IReflectionDefinition* ref_def : comp_ref_defs) {
		const ECSClassAttribute* const ecs = ref_def->getClassAttribute<ECSClassAttribute>();
		wxTreeItemId category_id = root;

		if (const char* const category = ecs->getCategory()) {
			wxTreeItemIdValue unused;
			wxTreeItemId id = _ecs_components->GetFirstChild(root, unused);

			while (id.IsOk()) {
				if (_ecs_components->GetItemText(id) == category) {
					category_id = id;
					break;
				}

				id = _ecs_components->GetNextSibling(id);
			}

			if (category_id == root) {
				category_id = _ecs_components->AppendItem(root, category);
			}
		}

		// Referencing otherwise GetItemData() returns null in on onDragBegin(). Might be some DLL weirdness.
		const wxTreeItemId id = _ecs_components->AppendItem(category_id, ref_def->getReflectionInstance().getName(), -1, -1, new RefDefItem(ref_def));
		RefDefItem* const item = reinterpret_cast<RefDefItem*>(_ecs_components->GetItemData(id));
		GAFF_REF(item);
	}

	_ecs_components->ExpandAll();
}

NS_END
