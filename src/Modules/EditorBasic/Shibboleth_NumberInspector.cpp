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
#include <Shibboleth_EngineAttributesCommon.h>
#include <wx/textctrl.h>
#include <wx/valnum.h>
#include <wx/sizer.h>

SHIB_REFLECTION_DEFINE(NumberInspector)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(NumberInspector)
	.CTOR(void*, const Gaff::IReflectionDefinition&, const Gaff::IReflectionDefinition*, int32_t, wxWindow*)

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

template <class T>
static void StepHelper(void* value, double step)
{
	if (step > 0.0) {
		*reinterpret_cast<T*>(value) += static_cast<T>(step);
	} else {
		*reinterpret_cast<T*>(value) -= static_cast<T>(-step);
	}
}

template <class T, class Validator>
static void SetMinMaxHelper(Validator& validator, double min, double max)
{
	min = Gaff::Max<double>(eastl::numeric_limits<T>::min(), min);
	max = Gaff::Min<double>(eastl::numeric_limits<T>::max(), max);
	validator.SetRange(static_cast<T>(min), static_cast<T>(max));
}

NumberInspector::NumberInspector(
	void* value,
	const Gaff::IReflectionDefinition& ref_def,
	const Gaff::IReflectionDefinition* parent_ref_def,
	int32_t var_index,
	wxWindow* parent
):
	wxPanel(parent),
	_value(value),
	_step(1.0)
{
	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

	const wxValidator* const validator = createValidator(value, ref_def, parent_ref_def, var_index);
	_text = new wxTextCtrl(this, wxID_ANY);
	_text->SetMinSize(wxSize(150, 40));
	_text->SetWindowStyle(wxVSCROLL);
	_text->SetValidator(*validator);

	delete validator;

	_text->GetValidator()->TransferToWindow();

	sizer->Add(_text, 1, wxALIGN_CENTER);
	sizer->SetSizeHints(this);
	SetSizer(sizer);

	_text->Bind(wxEVT_SCROLLWIN_LINEDOWN, &NumberInspector::onScrollDown, this);
	_text->Bind(wxEVT_SCROLLWIN_LINEUP, &NumberInspector::onScrollUp, this);

	Bind(wxEVT_TEXT, &NumberInspector::onTextChange, this, _text->GetId());
}

wxValidator* NumberInspector::createValidator(
	void* value,
	const Gaff::IReflectionDefinition& ref_def,
	const Gaff::IReflectionDefinition* parent_ref_def,
	int32_t var_index
)
{
	const Gaff::Hash64 hash = ref_def.getReflectionInstance().getHash();

	double min = eastl::numeric_limits<double>::min();
	double max = eastl::numeric_limits<double>::max();

	const RangeAttribute* range = nullptr;

	// Get value range.
	if (parent_ref_def) {
		range = parent_ref_def->getVarAttr<RangeAttribute>(parent_ref_def->getVarHash(var_index));
	} else {
		range = ref_def.getVarAttr<RangeAttribute>(parent_ref_def->getVarHash(var_index));
	}

	if (range) {
		_step = range->getStep();
		min = range->getMin();
		max = range->getMax();
	}

	if (hash == Reflection<int8_t>::GetHash()) {
		auto* const validator = new wxIntegerValidator<int8_t>(reinterpret_cast<int8_t*>(value));
		SetMinMaxHelper<int8_t>(*validator, min, max);
		_step_func = StepHelper<int8_t>;

		return validator;

	} else if (hash == Reflection<int16_t>::GetHash()) {
		auto* const validator = new wxIntegerValidator<int16_t>(reinterpret_cast<int16_t*>(value));
		SetMinMaxHelper<int16_t>(*validator, min, max);
		_step_func = StepHelper<int16_t>;

		return validator;

	} else if (hash == Reflection<int32_t>::GetHash()) {
		auto* const validator = new wxIntegerValidator<int32_t>(reinterpret_cast<int32_t*>(value));
		SetMinMaxHelper<int32_t>(*validator, min, max);
		_step_func = StepHelper<int32_t>;

		return validator;

	} else if (hash == Reflection<int64_t>::GetHash()) {
		auto* const validator = new wxIntegerValidator<int64_t>(reinterpret_cast<int64_t*>(value));
		SetMinMaxHelper<int64_t>(*validator, min, max);
		_step_func = StepHelper<int64_t>;

		return validator;

	} else if (hash == Reflection<uint8_t>::GetHash()) {
		auto* const validator = new wxIntegerValidator<uint8_t>(reinterpret_cast<uint8_t*>(value));
		SetMinMaxHelper<uint8_t>(*validator, min, max);
		_step_func = StepHelper<uint8_t>;

		return validator;

	} else if (hash == Reflection<uint16_t>::GetHash()) {
		auto* const validator = new wxIntegerValidator<uint16_t>(reinterpret_cast<uint16_t*>(value));
		SetMinMaxHelper<uint16_t>(*validator, min, max);
		_step_func = StepHelper<uint16_t>;

		return validator;

	} else if (hash == Reflection<uint32_t>::GetHash()) {
		auto* const validator = new wxIntegerValidator<uint32_t>(reinterpret_cast<uint32_t*>(value));
		SetMinMaxHelper<uint32_t>(*validator, min, max);
		_step_func = StepHelper<uint32_t>;

		return validator;

	} else if (hash == Reflection<uint64_t>::GetHash()) {
		auto* const validator = new wxIntegerValidator<uint64_t>(reinterpret_cast<uint64_t*>(value));
		SetMinMaxHelper<uint64_t>(*validator, min, max);
		_step_func = StepHelper<uint64_t>;

		return validator;

	} else if (hash == Reflection<float>::GetHash()) {
		auto* const validator = new wxFloatingPointValidator<float>(3, reinterpret_cast<float*>(value), wxNUM_VAL_NO_TRAILING_ZEROES);
		SetMinMaxHelper<float>(*validator, min, max);
		_step_func = StepHelper<float>;

		return validator;

	} else if (hash == Reflection<double>::GetHash()) {
		auto* const validator = new wxFloatingPointValidator<double>(6, reinterpret_cast<double*>(value), wxNUM_VAL_NO_TRAILING_ZEROES);
		_step_func = StepHelper<double>;
		validator->SetRange(min, max);

		return validator;
	}

	return nullptr;
}

void NumberInspector::onTextChange(const wxCommandEvent&)
{
	_text->GetValidator()->TransferFromWindow();
}

void NumberInspector::onScrollDown(const wxScrollWinEvent&)
{
	_step_func(_value, -_step);
	_text->GetValidator()->TransferToWindow();
}

void NumberInspector::onScrollUp(const wxScrollWinEvent&)
{
	_step_func(_value, _step);
	_text->GetValidator()->TransferToWindow();
}

NS_END
