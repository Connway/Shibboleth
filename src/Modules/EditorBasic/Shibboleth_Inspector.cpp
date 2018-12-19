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
	//_default_logic = new InspectorReflectionLogic;
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
	const auto inspectors = GetApp().getReflectionManager().getReflectionWithAttribute(CLASS_HASH(IEditorInspectorAttribute));
	const Gaff::IReflectionDefinition& ref_def = item->getReflectionDefinition();

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

	wxPanel* inspector = nullptr;

	if (it != inspectors.end()) {
		ProxyAllocator allocator("Editor");

		inspector = (*it)->createT<wxPanel>(
			CLASS_HASH(wxPanel),
			ARG_HASH(const Gaff::IReflectionDefinition&, const char*, wxWindow*),
			allocator,
			ref_def,
			ref_def.getReflectionInstance().getName(),
			this
		);

	} else {
		inspector = reflectionInit(*item);
	}

	AddChild(inspector);

	//_logic = const_cast<IInspectorLogic*>(ref_def.GET_CLASS_ATTR(IInspectorLogic));

	//// If no attribute, check if the item itself is the inspector logic.
	//if (!_logic) {
	//	_logic = const_cast<IInspectorLogic*>(INTERFACE_CAST(IInspectorLogic, *item));

	//	// Default to reflection inspector.
	//	if (!_logic) {
	//		_logic = _default_logic;
	//	}
	//}

	//_logic->populate(*this, *item);
}

wxPanel* Inspector::reflectionInit(const Gaff::IReflectionObject& item)
{
	const Gaff::IReflectionDefinition& ref_def = item.getReflectionDefinition();

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

	//_logic = nullptr;
}

NS_END
