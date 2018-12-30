#include "Shibboleth_EditorTest.h"
#include <Shibboleth_EditorInspectorAttribute.h>

SHIB_REFLECTION_DEFINE(TestPropertyEditor)
SHIB_REFLECTION_DEFINE(TestType)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TestType)
	.BASE(Gaff::IReflectionObject)
	.var("IntProp", &TestType::t1)
	.var("FloatProp", &TestType::t2)
SHIB_REFLECTION_CLASS_DEFINE_END(TestType)

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TestPropertyEditor)
	.classAttrs(EditorInspectorAttribute<TestType>())

	.ctor<>()

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)
SHIB_REFLECTION_CLASS_DEFINE_END(TestPropertyEditor)

void TestPropertyEditor::Init(void)
{
	wxPropertyGrid::RegisterEditorClass(new TestPropertyEditor(), Reflection<TestPropertyEditor>::GetName());
}

wxPGWindowList TestPropertyEditor::CreateControls(
	wxPropertyGrid* prop_grid,
	wxPGProperty* property,
	const wxPoint& pos,
	const wxSize& size
) const
{
	GAFF_REF(prop_grid, property, pos, size);
	wxPGWindowList window_list;

	return window_list;
}

void TestPropertyEditor::UpdateControl(wxPGProperty* property, wxWindow* ctrl) const
{
	GAFF_REF(property, ctrl);
}

bool TestPropertyEditor::OnEvent(
	wxPropertyGrid* prop_grid,
	wxPGProperty* property,
	wxWindow* wnd_primary,
	wxEvent& event
) const
{
	GAFF_REF(prop_grid, property, wnd_primary, event);
	return false;
}

NS_END
