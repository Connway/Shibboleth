#include "Shibboleth_VectorInspector.h"

#include <Shibboleth_EditorInspectorAttribute.h>
#include <Shibboleth_Math.h>

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>
#include <wx/sizer.h>

SHIB_REFLECTION_DEFINE(VectorInspector)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(VectorInspector)
	.CTOR(void*, const Gaff::IReflectionDefinition&, const Gaff::IReflectionDefinition*, int32_t, wxWindow*)

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)

	.classAttrs(
		EditorInspectorAttribute<glm::vec3>(),
		EditorInspectorAttribute<glm::vec2>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(VectorInspector)

VectorInspector::VectorInspector(
	void* value,
	const Gaff::IReflectionDefinition& ref_def,
	const Gaff::IReflectionDefinition* /*parent_ref_def*/,
	int32_t /*var_index*/,
	wxWindow* parent
):
	wxPanel(parent)
{
	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

	wxTextCtrl* const x = new wxTextCtrl(this, wxID_ANY);
	wxTextCtrl* const y = new wxTextCtrl(this, wxID_ANY);
	wxTextCtrl* z = nullptr;

	if (&ref_def == &Reflection<glm::vec3>::GetReflectionDefinition()) {
		glm::vec3& vec3 = *reinterpret_cast<glm::vec3*>(value);

		wxFloatingPointValidator<float> x_validator(3, &vec3.x, wxNUM_VAL_NO_TRAILING_ZEROES);
		wxFloatingPointValidator<float> y_validator(3, &vec3.y, wxNUM_VAL_NO_TRAILING_ZEROES);
		wxFloatingPointValidator<float> z_validator(3, &vec3.z, wxNUM_VAL_NO_TRAILING_ZEROES);

		z = new wxTextCtrl(this, wxID_ANY);

		x->SetValidator(x_validator);
		y->SetValidator(y_validator);
		z->SetValidator(z_validator);

	} else {
		glm::vec2& vec2 = *reinterpret_cast<glm::vec2*>(value);

		wxFloatingPointValidator<float> x_validator(3, &vec2.x, wxNUM_VAL_NO_TRAILING_ZEROES);
		wxFloatingPointValidator<float> y_validator(3, &vec2.y, wxNUM_VAL_NO_TRAILING_ZEROES);

		x->SetValidator(x_validator);
		y->SetValidator(y_validator);
	}

	x->GetValidator()->TransferToWindow();
	y->GetValidator()->TransferToWindow();

	sizer->Add(new wxStaticText(this, wxID_ANY, "X: "), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	sizer->Add(x, 1, wxALIGN_CENTER);
	sizer->AddSpacer(10);
	sizer->Add(new wxStaticText(this, wxID_ANY, "Y: "), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
	sizer->Add(y, 1, wxALIGN_CENTER);

	Bind(wxEVT_TEXT, &VectorInspector::onTextChange, this, x->GetId());
	Bind(wxEVT_TEXT, &VectorInspector::onTextChange, this, y->GetId());

	if (z) {
		z->GetValidator()->TransferToWindow();

		sizer->AddSpacer(10);
		sizer->Add(new wxStaticText(this, wxID_ANY, "Z: "), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT);
		sizer->Add(z, 1, wxALIGN_CENTER);

		Bind(wxEVT_TEXT, &VectorInspector::onTextChange, this, z->GetId());
	}

	sizer->SetSizeHints(this);
	SetSizer(sizer);
}

void VectorInspector::onTextChange(const wxCommandEvent& event)
{
	static_cast<wxTextCtrl*>(event.GetEventObject())->GetValidator()->TransferFromWindow();
}


NS_END
