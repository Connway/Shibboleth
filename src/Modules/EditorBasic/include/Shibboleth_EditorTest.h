#pragma once

#include <Shibboleth_Reflection.h>

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

#include <wx/propgrid/propgrid.h>

NS_SHIBBOLETH

class TestPropertyEditor final : public Gaff::IReflectionObject, public wxPGEditor
{
public:
	wxPGWindowList CreateControls(
		wxPropertyGrid* prop_grid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& size
	) const override;

	void UpdateControl(wxPGProperty* property, wxWindow* ctrl) const override;

	bool OnEvent(
		wxPropertyGrid* prop_grid,
		wxPGProperty* property,
		wxWindow* wnd_primary,
		wxEvent& event
	) const override;

private:

	SHIB_REFLECTION_CLASS_DECLARE(TestPropertyEditor);
};

NS_END

SHIB_REFLECTION_DECLARE(TestPropertyEditor);
