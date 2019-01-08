#pragma once

#include <Shibboleth_Reflection.h>

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

#include <Shibboleth_Reflection.h>

#ifdef PLATFORM_WINDOWS
	#include <wx/msw/winundef.h>
#endif

#include <wx/panel.h>

class wxCommandEvent;

NS_SHIBBOLETH

class VectorInspector final : public Gaff::IReflectionObject, public wxPanel
{
public:
	VectorInspector(
		void* value,
		const Gaff::IReflectionDefinition& ref_def,
		const Gaff::IReflectionDefinition* parent_ref_def,
		int32_t var_index,
		wxWindow* parent
	);

private:
	void onTextChange(const wxCommandEvent& event);

	SHIB_REFLECTION_CLASS_DECLARE(VectorInspector);
};

NS_END

SHIB_REFLECTION_DECLARE(VectorInspector);
