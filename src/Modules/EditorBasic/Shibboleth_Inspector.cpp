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

#include <Shibboleth_NumberInspector.h>

#include <wx/stattext.h>
#include <wx/sizer.h>

SHIB_REFLECTION_DEFINE(Inspector)

NS_SHIBBOLETH

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
	//auto* const ni = new NumberInspector(new double(123.0f), Reflection<double>::GetReflectionDefinition(), &getReflectionDefinition(), this);

	//wxBoxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);
	//sizer->Add(new wxStaticText(this, wxID_ANY, "Test Number"), 2, wxEXPAND | wxALL | wxCENTER | wxALIGN_CENTER, 5);
	//sizer->Add(ni, 5, wxEXPAND | wxALL | wxCENTER | wxALIGN_LEFT, 5);
	//sizer->SetSizeHints(this);

	//SetSizer(sizer);
}

Inspector::~Inspector(void)
{
}

void Inspector::onItemSelected(const EditorItemSelectedMessage& message)
{
	clear();

	Gaff::IReflectionObject* const item = message.getItem();

	if (!item) {
		return;
	}

	// Check for an attribute with inspector logic.
	const Gaff::IReflectionDefinition& ref_def = item->getReflectionDefinition();
	const Gaff::IReflectionDefinition* const inspector_ref_def = getInspectorReflection(ref_def);

	wxPanel* inspector = nullptr;

	if (inspector_ref_def) {
		ProxyAllocator allocator("Editor");

		inspector = inspector_ref_def->createT<wxPanel>(
			CLASS_HASH(wxPanel),
			ARG_HASH(void*, const Gaff::IReflectionDefinition&, const Gaff::IReflectionDefinition*, wxWindow*),
			allocator,
			item->getBasePointer(),
			ref_def,
			nullptr,
			this
		);

	} else {
		inspector = reflectionInit(*item);
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

wxPanel* Inspector::reflectionInit(const Gaff::IReflectionObject& item)
{
	const Gaff::IReflectionDefinition& ref_def = item.getReflectionDefinition();
	GAFF_REF(ref_def);

	const int32_t num_vars = ref_def.getNumVars();

	for (int32_t i = 0; i < num_vars; ++i) {
		//Gaff::IReflectionVar* const var = ref_def.getVar(i);
		//var->getData
	}

	return nullptr;
}

void Inspector::clear(void)
{
	// Copy m_children.
	const wxWindowList children = m_children;

	for (wxWindow* child : children) {
		//RemoveChild(child);
		delete child;
	}
}

NS_END
