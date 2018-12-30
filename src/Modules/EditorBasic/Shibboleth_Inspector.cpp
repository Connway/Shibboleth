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

#include "Shibboleth_Inspector.h"
#include <Shibboleth_EditorItemSelectedMessage.h>
#include <Shibboleth_EditorInspectorAttribute.h>
#include <Shibboleth_EngineAttributesCommon.h>
#include <Shibboleth_EditorWindowAttribute.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/sizer.h>

//NS_SHIBBOLETH
//class Test final : public Gaff::IReflectionObject
//{
//private:
//	int32_t t1 = 123;
//	float t2 = 456.0f;
//
//	SHIB_REFLECTION_CLASS_DECLARE(Test);
//};
//
//class TestInner final : public Gaff::IReflectionObject
//{
//private:
//	Test t;
//
//	SHIB_REFLECTION_CLASS_DECLARE(TestInner);
//};
//
//class TestOuter final : public Gaff::IReflectionObject
//{
//private:
//	TestInner t;
//
//	SHIB_REFLECTION_CLASS_DECLARE(TestOuter);
//};
//NS_END
//
//SHIB_REFLECTION_DECLARE(Test)
//SHIB_REFLECTION_DEFINE(Test)
//
//SHIB_REFLECTION_DECLARE(TestOuter)
//SHIB_REFLECTION_DEFINE(TestOuter)
//
//SHIB_REFLECTION_DECLARE(TestInner)
//SHIB_REFLECTION_DEFINE(TestInner)
//
//NS_SHIBBOLETH
//SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Test)
//	.BASE(Gaff::IReflectionObject)
//	.var("IntProp", &Test::t1)
//	.var("FloatProp", &Test::t2)
//SHIB_REFLECTION_CLASS_DEFINE_END(Test)
//
//SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TestOuter)
//	.BASE(Gaff::IReflectionObject)
//	.var("InnerClass", &TestOuter::t)
//SHIB_REFLECTION_CLASS_DEFINE_END(TestOuter)
//
//SHIB_REFLECTION_CLASS_DEFINE_BEGIN(TestInner)
//	.BASE(Gaff::IReflectionObject)
//	.var("InnerClassAgain", &TestInner::t)
//SHIB_REFLECTION_CLASS_DEFINE_END(TestInner)
//NS_END


SHIB_REFLECTION_DEFINE(Inspector)

NS_SHIBBOLETH

template <class T>
static wxPGProperty* CreateNumericProperty(const RangeAttribute* range, void* data, const char* name)
{
	double step = 1.0;
	double min = 0.0;
	double max = 0.0;

	if (range) {
		min = Gaff::Max<double, double>(range->getMin(), eastl::numeric_limits<T>::min());
		max = Gaff::Min<double, double>(range->getMax(), eastl::numeric_limits<T>::max());
		step = range->getStep();
	} else {
		min = eastl::numeric_limits<T>::min();
		max = eastl::numeric_limits<T>::max();
	}

	wxPGProperty* property = nullptr;

	// int**_t value
	if constexpr (eastl::is_integral_v<T> && eastl::is_signed<T>::value) {
		property = new wxIntProperty(name);
		property->SetValue(wxVariant(wxLongLong(*reinterpret_cast<T*>(data))));
		property->SetAttribute(wxPG_ATTR_MIN, wxVariant(wxLongLong(static_cast<T>(min))));
		property->SetAttribute(wxPG_ATTR_MAX, wxVariant(wxLongLong(static_cast<T>(max))));
		property->SetAttribute(wxPG_ATTR_SPINCTRL_STEP, wxVariant(wxLongLong(static_cast<T>(step))));
		property->SetAttribute(wxPG_ATTR_SPINCTRL_WRAP, wxVariant(false));
		property->SetEditor(wxPGEditor_SpinCtrl);

	// uint**_t value
	} else if (eastl::is_integral_v<T> && eastl::is_unsigned<T>::value) {
		property = new wxUIntProperty(name);
		property->SetValue(wxVariant(wxULongLong(*reinterpret_cast<T*>(data))));
		property->SetAttribute(wxPG_ATTR_MIN, wxVariant(wxULongLong(static_cast<T>(min))));
		property->SetAttribute(wxPG_ATTR_MAX, wxVariant(wxULongLong(static_cast<T>(max))));
		property->SetAttribute(wxPG_ATTR_SPINCTRL_STEP, wxVariant(wxULongLong(static_cast<T>(step))));
		property->SetAttribute(wxPG_ATTR_SPINCTRL_WRAP, wxVariant(false));
		property->SetEditor(wxPGEditor_SpinCtrl);

	// Floating point value
	} else if (eastl::is_floating_point_v<T>) {
		property = new wxFloatProperty(name);
		property->SetValue(wxVariant(static_cast<double>(*reinterpret_cast<T*>(data))));
		property->SetAttribute(wxPG_ATTR_MIN, wxVariant(min));
		property->SetAttribute(wxPG_ATTR_MAX, wxVariant(max));
		property->SetAttribute(wxPG_ATTR_SPINCTRL_STEP, wxVariant(step));
		property->SetAttribute(wxPG_ATTR_SPINCTRL_WRAP, wxVariant(false));
		property->SetEditor(wxPGEditor_SpinCtrl);

		if (Reflection<T>::GetHash() == Reflection<double>::GetHash()) {
			property->SetAttribute(wxPG_FLOAT_PRECISION, wxVariant(6));
		} else {
			property->SetAttribute(wxPG_FLOAT_PRECISION, wxVariant(3));
		}
	}

	return property;
}

static wxPGProperty* GetBuiltInProperty(
	void* object,
	const Gaff::IReflectionDefinition& ref_def,
	int32_t var_index,
	const Gaff::IReflectionDefinition& var_ref_def
)
{
	const Gaff::Hash32 var_name_hash = ref_def.getVarHash(var_index);
	const char* const var_name = ref_def.getVarName(var_index);

	const RangeAttribute* const range = ref_def.getVarAttr<RangeAttribute>(var_name_hash);
	const Gaff::Hash64 hash = var_ref_def.getReflectionInstance().getHash();

	if (hash == Reflection<int8_t>::GetHash()) {
		return CreateNumericProperty<int8_t>(range, object, var_name);
	} else if (hash == Reflection<int16_t>::GetHash()) {
		return CreateNumericProperty<int16_t>(range, object, var_name);
	} else if (hash == Reflection<int32_t>::GetHash()) {
		return CreateNumericProperty<int32_t>(range, object, var_name);
	} else if (hash == Reflection<int64_t>::GetHash()) {
		return CreateNumericProperty<int64_t>(range, object, var_name);

	} else if (hash == Reflection<uint8_t>::GetHash()) {
		return CreateNumericProperty<uint8_t>(range, object, var_name);
	} else if (hash == Reflection<uint16_t>::GetHash()) {
		return CreateNumericProperty<uint16_t>(range, object, var_name);
	} else if (hash == Reflection<uint32_t>::GetHash()) {
		return CreateNumericProperty<uint32_t>(range, object, var_name);
	} else if (hash == Reflection<uint64_t>::GetHash()) {
		return CreateNumericProperty<uint64_t>(range, object, var_name);

	} else if (hash == Reflection<float>::GetHash()) {
		return CreateNumericProperty<float>(range, object, var_name);
	} else if (hash == Reflection<double>::GetHash()) {
		return CreateNumericProperty<double>(range, object, var_name);

	} else if (hash == Reflection<U8String>::GetHash()) {
		wxStringProperty* const property = new wxStringProperty(var_name);
		property->SetValue(wxVariant(reinterpret_cast<U8String*>(object)->data()));
	}

	return nullptr;
}


SHIB_REFLECTION_CLASS_DEFINE_BEGIN(Inspector)
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
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size)
{
	wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->SetSizeHints(this);
	SetSizer(sizer);

	_properties = new wxPropertyGrid(this);
	_properties->SetWindowStyle(wxPG_SPLITTER_AUTO_CENTER);
	_properties->SetColumnProportion(0, 2);
	_properties->SetColumnProportion(1, 5);
	_properties->CenterSplitter(true);

	sizer->Add(_properties, 1, wxEXPAND | wxALL | wxCENTER | wxALIGN_CENTER);

	//onItemSelected(EditorItemSelectedMessage(new TestOuter));
}

Inspector::~Inspector(void)
{
}

void Inspector::onItemSelected(const EditorItemSelectedMessage& message)
{
	_properties->Clear();

	Gaff::IReflectionObject* const item = message.getItem();

	if (!item) {
		return;
	}

	const Gaff::IReflectionDefinition& ref_def = item->getReflectionDefinition();
	const Gaff::IReflectionDefinition* const inspector_ref_def = getInspectorReflection(ref_def);

	// Check if we have an property editor for this object.
	if (inspector_ref_def) {
		//ProxyAllocator allocator("Editor");

		//wxPGProperty* const property = inspector_ref_def->createT<wxPGProperty>(
		//	CLASS_HASH(wxPGProperty),
		//	ARG_HASH(const Gaff::IReflectionDefinition&),
		//	allocator,
		//	ref_def
		//);

		//_properties->Append(property);

	// Iteratively create all the properties.
	} else {
		createEditors(item->getBasePointer(), item->getReflectionDefinition());
	}
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

void Inspector::createEditors(void* object, const Gaff::IReflectionDefinition& ref_def, wxPGProperty* root_category)
{
	const int32_t num_vars = ref_def.getNumVars();

	if (num_vars <= 0) {
		return;
	}

	for (int32_t i = 0; i < num_vars; ++i) {
		Gaff::IReflectionVar* const var = ref_def.getVar(i);
		const Gaff::IReflectionDefinition& var_ref_def = var->getReflectionDefinition();

		void* const var_obj = var->getData(object);

		if (wxPGProperty* const built_in_prop = GetBuiltInProperty(var_obj, ref_def, i, var_ref_def)) {
			if (root_category) {
				_properties->AppendIn(root_category, built_in_prop);
			} else {
				_properties->Append(built_in_prop);
			}

		} else if (const Gaff::IReflectionDefinition* const inspector_ref_def = getInspectorReflection(ref_def)) {
			//ProxyAllocator allocator("Editor");

			//wxPGProperty* const property = inspector_ref_def->createT<wxPGProperty>(
			//	CLASS_HASH(wxPGProperty),
			//	ARG_HASH(const Gaff::IReflectionDefinition&, const Gaff::IReflectionDefinition*, int32_t),
			//	allocator,
			//	var_ref_def,
			//	&ref_def,
			//	i
			//);

			//if (root_category) {
			//	_properties->AppendIn(root_category, property);
			//} else {
			//	_properties->Append(property);
			//}

		} else {
			wxPropertyCategory* const category = new wxPropertyCategory(ref_def.getVarName(i));

			if (root_category) {
				_properties->AppendIn(root_category, category);
			} else {
				_properties->Append(category);
			}

			createEditors(var_obj, var_ref_def, category);
		}
	}
}

NS_END
