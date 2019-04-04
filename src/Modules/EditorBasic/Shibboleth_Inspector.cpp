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

#include "Shibboleth_Inspector.h"
#include <Shibboleth_EditorItemSelectedMessage.h>
#include <Shibboleth_EditorInspectorAttribute.h>
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_EditorWindowAttribute.h>

#include <wx/collpane.h>
#include <wx/stattext.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/sizer.h>

#include "Shibboleth_Math.h"

//NS_SHIBBOLETH
//class Test final : public Gaff::IReflectionObject
//{
//private:
//	int32_t t1 = 123;
//	float t2 = 456.0f;
//	glm::vec3 t3 = glm::vec3(0.0f, 1.0f, 2.0f);

//	SHIB_REFLECTION_CLASS_DECLARE(Test);
//};
//
//class TestInner final : public Gaff::IReflectionObject
//{
//private:
//	Test t;

//	SHIB_REFLECTION_CLASS_DECLARE(TestInner);
//};

//class TestOuter final : public Gaff::IReflectionObject
//{
//private:
//	TestInner t;

//	SHIB_REFLECTION_CLASS_DECLARE(TestOuter);
//};
//NS_END

//SHIB_REFLECTION_DECLARE(Test)
//SHIB_REFLECTION_DEFINE(Test)

//SHIB_REFLECTION_DECLARE(TestOuter)
//SHIB_REFLECTION_DEFINE(TestOuter)

//SHIB_REFLECTION_DECLARE(TestInner)
//SHIB_REFLECTION_DEFINE(TestInner)

//NS_SHIBBOLETH
//SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Test)
//	.BASE(Gaff::IReflectionObject)
//	.var("IntProp", &Test::t1)
//	.var("FloatProp", &Test::t2)
//	.var("Vec3Prop", &Test::t3)
//SHIB_REFLECTION_CLASS_DEFINE_END(Test)

//SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TestOuter)
//	.BASE(Gaff::IReflectionObject)
//	.var("InnerClass", &TestOuter::t)
//SHIB_REFLECTION_CLASS_DEFINE_END(TestOuter)

//SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TestInner)
//	.BASE(Gaff::IReflectionObject)
//	.var("InnerClassAgain", &TestInner::t)
//SHIB_REFLECTION_CLASS_DEFINE_END(TestInner)
//NS_END


SHIB_REFLECTION_DEFINE(Inspector)

NS_SHIBBOLETH

static wxCollapsiblePane* CreateCollapsiblePane(wxWindow* parent, const wxString& label)
{
	return new wxCollapsiblePane(
		parent,
		wxID_ANY,
		label,
		wxDefaultPosition,
		wxDefaultSize,
		wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
	);
}

static wxWindow* CreateInspector(
	void* value,
	const Gaff::IReflectionDefinition& inspector_ref_def,
	const Gaff::IReflectionDefinition& ref_def,
	const Gaff::IReflectionDefinition* parent_ref_def,
	int32_t var_index,
	wxWindow* parent
)
{
	ProxyAllocator allocator("Editor");

	wxWindow* const inspector = inspector_ref_def.createT<wxWindow>(
		CLASS_HASH(wxWindow),
		ARG_HASH(void*, const Gaff::IReflectionDefinition&, const Gaff::IReflectionDefinition*, int32_t, wxWindow*),
		allocator,
		value,
		ref_def,
		parent_ref_def,
		var_index,
		parent
	);

	return inspector;
}


SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Inspector)
	.CTOR(const Gaff::IReflectionDefinition&, void*, bool, wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(const Gaff::IReflectionDefinition&, void*, bool, wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(const Gaff::IReflectionDefinition&, void*, bool, wxWindow*, wxWindowID)
	.CTOR(const Gaff::IReflectionDefinition&, void*, bool, wxWindow*)

	.CTOR(wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(wxWindow*, wxWindowID)
	.CTOR(wxWindow*)

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)

	.classAttrs(
		EditorWindowAttribute("&Inspector", "Window_Editors")
	)

	.func(
		"onItemSelected",
		&Inspector::onItemSelected,
		GlobalMessageAttribute<Inspector, EditorItemSelectedMessage>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(Inspector)

Inspector::Inspector(
	const Gaff::IReflectionDefinition& ref_def,
	void* data,
	bool create_top_level_pane,
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size),
	_create_top_level_pane(create_top_level_pane),
	_embedded(true)
{
	// $TODO: Think about using a flex grid sizer or something.
	wxBoxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
	sizer->SetSizeHints(this);
	SetSizer(sizer);

	onItemSelectedInternal(ref_def, data);
}

Inspector::Inspector(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size)
{
	// $TODO: Think about using a flex grid sizer or something.
	wxBoxSizer* const sizer = new wxBoxSizer(wxVERTICAL);
	sizer->SetSizeHints(this);
	SetSizer(sizer);

	//onItemSelected(EditorItemSelectedMessage(new TestInner));
}

Inspector::~Inspector(void)
{
}

void Inspector::SetCreateTopLevelPane(bool create)
{
	_create_top_level_pane = create;
}

bool Inspector::CreatesTopLevelPane(void) const
{
	return _create_top_level_pane;
}

void Inspector::onItemSelectedInternal(const Gaff::IReflectionDefinition& ref_def, void* data)
{
	wxSizer* sizer = GetSizer();
	sizer->Clear();

	if (!data) {
		return;
	}

	if (_create_top_level_pane) {
		wxCollapsiblePane* const coll_pane = CreateCollapsiblePane(this, ref_def.getReflectionInstance().getName());
		wxBoxSizer* const coll_sizer = new wxBoxSizer(wxVERTICAL);
		wxWindow* const window = coll_pane->GetPane();
		coll_sizer->SetSizeHints(window);
		window->SetSizer(coll_sizer);

		sizer->Add(coll_pane, 1, wxEXPAND);
		coll_sizer->AddSpacer(15);
		sizer = coll_sizer;

		Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &Inspector::paneChanged, this, coll_pane->GetId());
	}

	// Check if we have an property editor for this object.
	if (const Gaff::IReflectionDefinition* const inspector_ref_def = getInspectorReflection(ref_def)) {
		wxWindow* const inspector = CreateInspector(data, *inspector_ref_def, ref_def, nullptr, -1, this);

		if (!inspector) {
			// $TODO: Log error.
			return;
		}

		sizer->Add(inspector, 1, wxEXPAND | wxALL);

	// Iteratively create all the properties.
	} else {
		createEditors(data, ref_def, sizer, 0);
	}

	Layout();
}

void Inspector::onItemSelected(const EditorItemSelectedMessage& message)
{
	if (_embedded) {
		return;
	}

	onItemSelectedInternal(message.getRefDef(), message.getData());
}

const Gaff::IReflectionDefinition* Inspector::getInspectorReflection(const Gaff::IReflectionDefinition& ref_def) const
{
	const auto inspectors = GetApp().getReflectionManager().getReflectionWithAttribute(CLASS_HASH(IEditorInspectorAttribute));

	const auto it = eastl::find(inspectors.begin(), inspectors.end(), ref_def, [](const Gaff::IReflectionDefinition* inspector_def, const Gaff::IReflectionDefinition& item_def) -> bool
	{
		const auto attrs = inspector_def->getClassAttrs<IEditorInspectorAttribute, ProxyAllocator>(CLASS_HASH(IEditorInspectorAttribute));

		for (const IEditorInspectorAttribute* attr : attrs) {
			if (&attr->getType() == &item_def) {
				return true;
			}
		}

		return false;
	});

	return (it != inspectors.end()) ? *it : nullptr;
}

void Inspector::createEditors(void* object, const Gaff::IReflectionDefinition& ref_def, wxSizer* parent_sizer, int32_t spacer)
{
	const int32_t num_vars = ref_def.getNumVars();

	for (int32_t i = 0; i < num_vars; ++i) {
		Gaff::IReflectionVar* const var = ref_def.getVar(i);
		const Gaff::IReflectionDefinition& var_ref_def = var->getReflectionDefinition();
		void* const var_obj = var->getData(object);

		wxWindow* const parent_window = parent_sizer->GetContainingWindow();
		wxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

		if (spacer > 0) {
			sizer->AddSpacer(spacer);
		}

		if (const Gaff::IReflectionDefinition* const inspector_ref_def = getInspectorReflection(var_ref_def)) {
			wxWindow* const inspector = CreateInspector(var_obj, *inspector_ref_def, var_ref_def, &ref_def, i, parent_window);

			if (!inspector) {
				// $TODO: Log error.
				delete sizer;
				continue;
			}

			wxStaticText* const label = new wxStaticText(parent_window, wxID_ANY, ref_def.getVarName(i));

			sizer->Add(label, 0, /*wxEXPAND |*/ wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);
			sizer->Add(inspector, 5, /*wxEXPAND |*/ wxLEFT | wxRIGHT | wxALIGN_CENTER_VERTICAL, 5);

		} else {
			if (var_ref_def.getNumVars() <= 0) {
				// $TODO: Log error.
				continue;
			}

			wxCollapsiblePane* const coll_pane = CreateCollapsiblePane(parent_window, ref_def.getVarName(i));
			wxBoxSizer* const coll_sizer = new wxBoxSizer(wxVERTICAL);
			wxWindow* const window = coll_pane->GetPane();
			coll_sizer->SetSizeHints(window);
			window->SetSizer(coll_sizer);

			sizer->Add(coll_pane, 1, wxEXPAND);

			Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, &Inspector::paneChanged, this, coll_pane->GetId());

			createEditors(var_obj, var_ref_def, coll_sizer, 15);
		}

		parent_sizer->Add(sizer, 0, /*wxEXPAND |*/ wxALL, 5);
	}
}

void Inspector::paneChanged(const wxCollapsiblePaneEvent&)
{
	Layout();
}

NS_END
