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

#include "Shibboleth_ArchetypeEditor.h"
#include "Shibboleth_ECSAttributes.h"
#include <Shibboleth_EditorFileHandlerAttribute.h>
#include <Shibboleth_EditorFileSelectedMessage.h>
#include <Shibboleth_EditorItemSelectedMessage.h>
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_EditorWindowAttribute.h>
#include <Shibboleth_UniqueAttribute.h>
#include <Shibboleth_SerializeReader.h>
#include <Shibboleth_LogManager.h>
#include <Gaff_JSON.h>

#include <wx/listctrl.h>
#include <wx/editlbox.h>
#include <wx/splitter.h>
#include <wx/treectrl.h>
#include <wx/sizer.h>
#include <wx/dnd.h>

SHIB_REFLECTION_DEFINE(ArchetypeEditor)

NS_SHIBBOLETH

constexpr static const char* const s_ref_def_format = "RefDefItemFormat";
static wxColour s_grey(127, 127, 127);
static wxColour s_black(0, 0, 0);

class RefDefItem final : public wxTreeItemData
{
public:
	RefDefItem(const Gaff::IReflectionDefinition& ref_def): _ref_def(ref_def) {}

	const Gaff::IReflectionDefinition& getRefDef(void) const { return _ref_def; }

	bool isDisabled(void) const { return _count == 2; }

	int32_t increment() { return ++_count; }
	int32_t decrement() { return --_count; }

private:
	const Gaff::IReflectionDefinition& _ref_def;
	int32_t _count = 0;
};

class ArcheTypeEditorDropTarget final : public wxDropTarget
{
public:
	ArcheTypeEditorDropTarget(ArchetypeEditor& editor, wxEditableListBox* ui):
		_editor(editor), _ui(ui)
	{
		m_dataObject = new wxCustomDataObject(s_ref_def_format);
	}

private:
	ArchetypeEditor& _editor;
	wxEditableListBox* const _ui;

	wxDragResult OnData(wxCoord /*x*/, wxCoord /*y*/, wxDragResult result) override
	{
		wxCustomDataObject* const data = static_cast<wxCustomDataObject*>(m_dataObject);
		RefDefItem** const items = static_cast<RefDefItem** const>(data->GetData());
		const int32_t num_items = static_cast<int32_t>(data->GetDataSize()) / sizeof(RefDefItem*);
		wxListCtrl* const list = _ui->GetListCtrl();

		int32_t add_count = 0;

		for (int32_t i = 0; i < num_items && items[i]; ++i) {
			const char* const name = items[i]->getRefDef().getReflectionInstance().getName();
			const int32_t existing_index = list->FindItem(-1, name);

			if (Gaff::InRange(existing_index, 0, list->GetItemCount())) {
				// Already have this component.
				continue;
			}

			if (_editor.addItem(*items[i], *_ui)) {
				++add_count;
			}
		}

		data->Free();

		if (add_count > 0) {
			_editor._broadcaster.broadcastSync(EditorItemSelectedMessage(_editor));
			return result;
		}

		return wxDragNone;
	}
};


SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ArchetypeEditor)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(wxWindow*, wxWindowID)
	.CTOR(wxWindow*)

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)

	.classAttrs(
		EditorWindowAttribute("&Archetype Editor", "Window_Editors"),
		EditorFileHandlerAttribute(".archetype")
	)

	.func(
		"onFileSelected",
		&ArchetypeEditor::onFileSelected,
		GlobalMessageAttribute<ArchetypeEditor, EditorFileSelectedMessage>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(ArchetypeEditor)

ArchetypeEditor::ArchetypeEditor(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size),
	_broadcaster(GetApp().getBroadcaster())
{
	wxSplitterWindow* const splitter = new wxSplitterWindow(this);
	splitter->SetWindowStyleFlag(wxSP_3D | wxSP_LIVE_UPDATE);
	splitter->SetMinimumPaneSize(10);

	_ecs_components = new wxTreeCtrl(splitter, wxID_ANY);
	_ecs_components->SetWindowStyleFlag(wxTR_HIDE_ROOT | wxTR_MULTIPLE);
	_ecs_components->AddRoot(wxT(""));

	wxSplitterWindow* const splitter_archetype = new wxSplitterWindow(splitter);
	splitter_archetype->SetWindowStyleFlag(wxSP_3D | wxSP_LIVE_UPDATE);
	splitter_archetype->SetMinimumPaneSize(10);

	_archetype_shared_ui = new wxEditableListBox(splitter_archetype, wxID_ANY, "Shared Components", wxDefaultPosition, wxDefaultSize, wxEL_ALLOW_DELETE);
	_archetype_shared_ui->SetDropTarget(new ArcheTypeEditorDropTarget(*this, _archetype_shared_ui));
	_archetype_shared_ui->GetListCtrl()->DeleteAllItems();

	_archetype_ui = new wxEditableListBox(splitter_archetype, wxID_ANY, "Components", wxDefaultPosition, wxDefaultSize, wxEL_ALLOW_DELETE);
	_archetype_ui->SetDropTarget(new ArcheTypeEditorDropTarget(*this, _archetype_ui));
	_archetype_ui->GetListCtrl()->DeleteAllItems();

	_ecs_components->Disable();
	_archetype_shared_ui->Disable();
	_archetype_ui->Disable();

	splitter_archetype->SplitHorizontally(_archetype_shared_ui, _archetype_ui, 200);
	splitter->SplitVertically(splitter_archetype, _ecs_components, 400);

	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(splitter, 1, wxEXPAND | wxALL);
	sizer->SetSizeHints(this);

	SetSizer(sizer);

	initComponentList();

	Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &ArchetypeEditor::onAddComponents, this, _ecs_components->GetId());
	Bind(wxEVT_TREE_ITEM_ACTIVATED, &ArchetypeEditor::onAddComponents, this, _ecs_components->GetId());
	Bind(wxEVT_TREE_BEGIN_DRAG, &ArchetypeEditor::onDragBegin, this, _ecs_components->GetId());

	Bind(wxEVT_LIST_DELETE_ITEM, &ArchetypeEditor::onRemoveSharedComponents, this, _archetype_shared_ui->GetListCtrl()->GetId());
	Bind(wxEVT_LIST_DELETE_ITEM, &ArchetypeEditor::onRemoveComponents, this, _archetype_ui->GetListCtrl()->GetId());
}

ArchetypeEditor::~ArchetypeEditor(void)
{
}

int32_t ArchetypeEditor::getNumSharedComponents(void) const
{
	return static_cast<int32_t>(_shared_components.size());
}

int32_t ArchetypeEditor::getNumComponents(void) const
{
	return static_cast<int32_t>(_components.size());
}

const Gaff::IReflectionDefinition& ArchetypeEditor::getSharedComponentRefDef(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_shared_components.size()));
	return *_shared_components[index].ref_def;
}

const Gaff::IReflectionDefinition& ArchetypeEditor::getComponentRefDef(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_components.size()));
	return *_components[index].ref_def;
}

void* ArchetypeEditor::getSharedComponentInstance(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_shared_components.size()));
	return _shared_components[index].instance;
}

void* ArchetypeEditor::getComponentInstance(int32_t index) const
{
	GAFF_ASSERT(index < static_cast<int32_t>(_components.size()));
	return _components[index].instance;
}

void ArchetypeEditor::onFileSelected(const EditorFileSelectedMessage& message)
{
	const U8String& path = message.getPath();

	if (!Gaff::EndsWith(path.c_str(), path.size(), ".archetype")) {
		_ecs_components->Disable();
		_archetype_shared_ui->Disable();
		_archetype_ui->Disable();
		return;
	}

	_ecs_components->Enable();
	_archetype_shared_ui->Enable();
	_archetype_ui->Enable();

	save();
	_path = path;
	load();

	_broadcaster.broadcastSync(EditorItemSelectedMessage(*this));
}

void ArchetypeEditor::onRemoveSharedComponents(wxListEvent& event)
{
	onRemoveComponentsHelper(event, *_archetype_shared_ui);
}

void ArchetypeEditor::onRemoveComponents(wxListEvent& event)
{
	onRemoveComponentsHelper(event, *_archetype_ui);
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

		addItem(*item, *_archetype_ui);

	} else {
		for (size_t i = 0; i < size; ++i) {
			RefDefItem* const item = getItem(ids[i]);

			if (!item) {
				return;
			}

			addItem(*item, *_archetype_ui);
		}
	}

	_broadcaster.broadcastSync(EditorItemSelectedMessage(*this));
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

	wxDataObject* const shared_data = _archetype_shared_ui->GetDropTarget()->GetDataObject();
	static_cast<wxCustomDataObject*>(shared_data)->SetData(sizeof(RefDefItem*) * size, ref_def_items);

	wxDataObject* const data = _archetype_ui->GetDropTarget()->GetDataObject();
	static_cast<wxCustomDataObject*>(data)->SetData(sizeof(RefDefItem*) * size, ref_def_items);

	delete [] ref_def_items;

	wxDropSource source(this);
	source.SetData(*data);
	source.DoDragDrop();
}

void ArchetypeEditor::onRemoveComponentsHelper(wxListEvent& event, wxEditableListBox& ui)
{
	const wxString& name = event.GetText();

	wxTreeItemIdValue unused;
	wxTreeItemId id = _ecs_components->GetFirstChild(_ecs_components->GetRootItem(), unused);

	while (id.IsOk()) {
		RefDefItem* item = static_cast<RefDefItem*>(_ecs_components->GetItemData(id));
		wxTreeItemId item_id = id;

		// This is a category, iterate all the items in the category.
		if (!item) {
			item_id = _ecs_components->GetFirstChild(id, unused);
		}

		bool found = false;

		while (item_id.IsOk()) {
			item = static_cast<RefDefItem*>(_ecs_components->GetItemData(item_id));

			if (_ecs_components->GetItemText(item_id) == name) {
				if (&ui == _archetype_shared_ui) {
					const auto it = findComponent(_shared_components, item->getRefDef());

					if (it != _shared_components.end()) {
						_shared_components.erase_unsorted(it);
					}
				} else {
					const auto it = findComponent(_components, item->getRefDef());

					if (it != _components.end()) {
						_components.erase_unsorted(it);
					}
				}

				item->decrement();

				if (!item->isDisabled()) {
					_ecs_components->SetItemTextColour(item->GetId(), s_black);
				}

				found = true;
				break;
			}

			if (item_id == id) {
				break;
			}

			item_id = _ecs_components->GetNextSibling(item_id);
		}

		if (found) {
			break;
		}

		id = _ecs_components->GetNextSibling(id);
	}

	_broadcaster.broadcastSync(EditorItemSelectedMessage(*this));
}

bool ArchetypeEditor::hasItem(const RefDefItem& item, wxEditableListBox& ui) const
{
	wxListCtrl* const list = ui.GetListCtrl();

	const char* const name = item.getRefDef().getReflectionInstance().getName();
	const int32_t existing_index = list->FindItem(-1, name);

	return Gaff::InRange(existing_index, 0, list->GetItemCount());
}

Vector<ArchetypeEditor::ComponentData>::iterator ArchetypeEditor::findComponent(Vector<ComponentData>& components, const Gaff::IReflectionDefinition& ref_def)
{
	return eastl::find(
		components.begin(),
		components.end(),
		ref_def,
		[](const ComponentData& lhs, const Gaff::IReflectionDefinition& rhs) -> bool { return lhs.ref_def == &rhs; }
	);
}

RefDefItem* ArchetypeEditor::getItem(const wxTreeItemId& id) const
{
	if (_ecs_components->HasChildren(id)) {
		return nullptr;
	}

	RefDefItem* const item = static_cast<RefDefItem*>(_ecs_components->GetItemData(id));

	if (item->isDisabled()) {
		return nullptr;
	}

	return item;
}

bool ArchetypeEditor::addItem(RefDefItem& item, wxEditableListBox& ui)
{
	wxListCtrl* const list = ui.GetListCtrl();
	const int32_t index = static_cast<int32_t>(list->GetItemCount());

	const Gaff::IReflectionDefinition& ref_def = item.getRefDef();
	const char* const name = ref_def.getReflectionInstance().getName();

	if (hasItem(item, ui)) {
		return false;
	}

	list->InsertItem(index, name);

	ProxyAllocator allocator("Editor");

	if (&ui == _archetype_shared_ui) {
		if (!ref_def.getStaticFunc<void, const void*, void*>(Gaff::FNV1aHash32Const("CopyShared"))) {
			LogErrorDefault("Failed to add shared component '%s'.", name);
			return false;
		}

		_shared_components.emplace_back(ComponentData{ &ref_def, ref_def.create(allocator) });

		//if (!_archetype.addShared(ref_def)) {
		//	LogErrorDefault("Failed to add shared component '%s'.", name);
		//	return false;
		//}

	} else {
		if (!ref_def.getStaticFunc<void, const void*, int32_t, void*, int32_t>(Gaff::FNV1aHash32Const("Copy"))) {
			LogErrorDefault("Failed to add component '%s'.", name);
			return false;
		}

		_components.emplace_back(ComponentData{ &ref_def, ref_def.create(allocator) });

		//if (!_archetype.add(ref_def)) {
		//	LogErrorDefault("Failed to add component '%s'.", name);
		//	return false;
		//}
	}

	item.increment();

	if (item.isDisabled()) {
		_ecs_components->SetItemTextColour(item.GetId(), s_grey);
	}

	//save();

	return true;
}

void ArchetypeEditor::initComponentList(void)
{
	const auto comp_ref_defs = GetApp().getReflectionManager().getReflectionWithAttribute<ECSClassAttribute>();
	const wxTreeItemId root = _ecs_components->GetRootItem();

	for (const Gaff::IReflectionDefinition* ref_def : comp_ref_defs) {
		const ECSClassAttribute* const ecs = ref_def->getClassAttr<ECSClassAttribute>();
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
		const wxTreeItemId id = _ecs_components->AppendItem(category_id, ref_def->getReflectionInstance().getName(), -1, -1, new RefDefItem(*ref_def));
		RefDefItem* const item = static_cast<RefDefItem*>(_ecs_components->GetItemData(id));
		GAFF_REF(item);
	}

	_ecs_components->ExpandAll();
}

void ArchetypeEditor::save(void)
{
}

void ArchetypeEditor::load(void)
{
	Gaff::JSON json;
	
	if (!json.parseFile(_path.c_str())) {
		return;
	}

	const ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	const Gaff::JSON shared_components = json["shared_components"];
	const Gaff::JSON components = json["components"];
	const ProxyAllocator allocator("Editor");

	shared_components.forEachInObject([&](const char* component, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			// $TODO: Log error
			return false;
		}

		const Gaff::IReflectionDefinition* const ref_def = refl_mgr.getReflection(Gaff::FNV1aHash64String(component));

		if (!ref_def) {
			// $TODO: Log error
			return false;
		}

		const ECSClassAttribute* const ecs = ref_def->getClassAttr<ECSClassAttribute>();

		if (!ecs) {
			// $TODO: Log error
			return false;
		}

		// Add to shared components list.
		RefDefItem ref_def_item(*ref_def);
		
		if (!addItem(ref_def_item, *_archetype_shared_ui)) {
			// $TODO: Log error
			return false;
		}

		SerializeReader<Gaff::JSON> reader(value, allocator);
		ref_def->load(reader, _shared_components.back().instance);

		return false;
	});

	components.forEachInObject([&](const char* component, const Gaff::JSON& value) -> bool
	{
		if (!value.isObject()) {
			// $TODO: Log error.
			return false;
		}

		const Gaff::IReflectionDefinition* const ref_def = refl_mgr.getReflection(Gaff::FNV1aHash64String(component));

		if (!ref_def) {
			// $TODO: Log error
			return false;
		}

		const ECSClassAttribute* const ecs = ref_def->getClassAttr<ECSClassAttribute>();

		if (!ecs) {
			// $TODO: Log error
			return false;
		}

		// Add to components list.
		RefDefItem ref_def_item(*ref_def);

		if (!addItem(ref_def_item, *_archetype_ui)) {
			// $TODO: Log error
			return false;
		}

		SerializeReader<Gaff::JSON> reader(value, allocator);
		ref_def->load(reader, _components.back().instance);

		return false;
	});
}

NS_END
