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

#include "Shibboleth_NumberInspector.h"
#include <Shibboleth_EditorInspectorAttribute.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/valnum.h>

SHIB_REFLECTION_DEFINE(NumberInspector)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(NumberInspector)
	.CTOR(const Gaff::IReflectionDefinition&, const char*, wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(const Gaff::IReflectionDefinition&, const char*, wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(const Gaff::IReflectionDefinition&, const char*, wxWindow*, wxWindowID)
	.CTOR(const Gaff::IReflectionDefinition&, const char*, wxWindow*)
	.CTOR(const Gaff::IReflectionDefinition&, const char*)

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)

	.classAttrs(
		EditorInspectorAttribute<int8_t>(),
		EditorInspectorAttribute<int16_t>(),
		EditorInspectorAttribute<int32_t>(),
		EditorInspectorAttribute<int64_t>(),
		EditorInspectorAttribute<uint8_t>(),
		EditorInspectorAttribute<uint16_t>(),
		EditorInspectorAttribute<uint32_t>(),
		EditorInspectorAttribute<uint64_t>(),
		EditorInspectorAttribute<float>(),
		EditorInspectorAttribute<double>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(NumberInspector)

NumberInspector::NumberInspector(const Gaff::IReflectionDefinition& type, const char* name, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size):
	wxPanel(parent, id, pos, size), _type(type)
{
	init(name);
}

NumberInspector::NumberInspector(const Gaff::IReflectionDefinition& type, const char* name, wxWindow* parent, wxWindowID id, const wxPoint& pos):
	wxPanel(parent, id, pos), _type(type)
{
	init(name);
}

NumberInspector::NumberInspector(const Gaff::IReflectionDefinition& type, const char* name, wxWindow* parent, wxWindowID id):
	wxPanel(parent, id), _type(type)
{
	init(name);
}

NumberInspector::NumberInspector(const Gaff::IReflectionDefinition& type, const char* name, wxWindow* parent):
	wxPanel(parent), _type(type)
{
	init(name);
}

NumberInspector::NumberInspector(const Gaff::IReflectionDefinition& type, const char* name):
	wxPanel(nullptr), _type(type)
{
	init(name);
}

wxValidator* NumberInspector::createValidator(void) const
{
	const Gaff::Hash64 hash = _type.getReflectionInstance().getHash();

	// Get min/max.

	if (hash == Reflection<int8_t>::GetHash()) {
		return new wxIntegerValidator<int8_t>();
	} else if (hash == Reflection<int16_t>::GetHash()) {
		return new wxIntegerValidator<int16_t>();
	} else if (hash == Reflection<int32_t>::GetHash()) {
		return new wxIntegerValidator<int32_t>();
	} else if (hash == Reflection<int64_t>::GetHash()) {
		return new wxIntegerValidator<int64_t>();
	} else if (hash == Reflection<uint8_t>::GetHash()) {
		return new wxIntegerValidator<uint8_t>();
	} else if (hash == Reflection<uint16_t>::GetHash()) {
		return new wxIntegerValidator<uint16_t>();
	} else if (hash == Reflection<uint32_t>::GetHash()) {
		return new wxIntegerValidator<uint32_t>();
	} else if (hash == Reflection<uint64_t>::GetHash()) {
		return new wxIntegerValidator<uint64_t>();
	} else if (hash == Reflection<float>::GetHash()) {
		return new wxFloatingPointValidator<float>(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
	} else if (hash == Reflection<double>::GetHash()) {
		return new wxFloatingPointValidator<double>(nullptr, wxNUM_VAL_NO_TRAILING_ZEROES);
	}

	return nullptr;
}

void NumberInspector::init(const char* name)
{
	wxStaticText* const label = new wxStaticText(this, wxID_ANY, name);
	wxTextCtrl* const text = new wxTextCtrl(this, wxID_ANY/*, default_value*/);
	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

	const wxValidator* const validator = createValidator();
	text->SetValidator(*validator);
	delete validator;

	sizer->Add(label, 1/*, wxEXPAND | wxALL*/);
	sizer->Add(text, 1, wxEXPAND | wxALL);
	sizer->SetSizeHints(this);

	SetSizer(sizer);
}

NS_END
