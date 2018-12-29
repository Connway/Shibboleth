#include "Shibboleth_EditorTest.h"

SHIB_REFLECTION_DEFINE(TestPropertyEditor)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TestPropertyEditor)
	.ctor<>()

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)
SHIB_REFLECTION_CLASS_DEFINE_END(TestPropertyEditor)

wxPGWindowList TestPropertyEditor::CreateControls(
	wxPropertyGrid* prop_grid,
	wxPGProperty* property,
	const wxPoint& pos,
	const wxSize& size
) const
{
	GAFF_REF(prop_grid, property, pos, size);
	return wxPGWindowList();
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
