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

#include "Shibboleth_BuiltinTypeEditors.h"
#include <Shibboleth_EditorInspectorAttribute.h>

SHIB_REFLECTION_DEFINE(IntegerEditor)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(IntegerEditor)
	.CTOR(const Gaff::IReflectionDefinition&, wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(const Gaff::IReflectionDefinition&, wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(const Gaff::IReflectionDefinition&, wxWindow*, wxWindowID)
	.CTOR(const Gaff::IReflectionDefinition&, wxWindow*)

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
		EditorInspectorAttribute<uint64_t>()
	)
SHIB_REFLECTION_CLASS_DEFINE_END(IntegerEditor)

IntegerEditor::IntegerEditor(const Gaff::IReflectionDefinition& type, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size):
	wxPanel(parent, id, pos, size), _type(type)
{
	init();
}

IntegerEditor::IntegerEditor(const Gaff::IReflectionDefinition& type, wxWindow* parent, wxWindowID id, const wxPoint& pos):
	wxPanel(parent, id, pos), _type(type)
{
	init();
}

IntegerEditor::IntegerEditor(const Gaff::IReflectionDefinition& type, wxWindow* parent, wxWindowID id):
	wxPanel(parent, id), _type(type)
{
	init();
}

IntegerEditor::IntegerEditor(const Gaff::IReflectionDefinition& type, wxWindow* parent):
	wxPanel(parent), _type(type)
{
	init();
}

void IntegerEditor::init(void)
{
}

NS_END
