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

#include "Shibboleth_ArchetypeEditor.h"
#include <Shibboleth_EditorWindowAttribute.h>

SHIB_REFLECTION_EXTERNAL_DEFINE(Shibboleth::ArchetypeEditor)

SHIB_REFLECTION_BUILDER_BEGIN(Shibboleth::ArchetypeEditor)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&, const wxSize&)
	.CTOR(wxWindow*, wxWindowID, const wxPoint&)
	.CTOR(wxWindow*, wxWindowID)
	.CTOR(wxWindow*)

	.BASE(wxWindow)

	.classAttrs(
		EditorWindowAttribute("Editors/Archetype Editor", "Archetype Editor")
	)

SHIB_REFLECTION_BUILDER_END(Shibboleth::ArchetypeEditor)

NS_SHIBBOLETH

ArchetypeEditor::ArchetypeEditor(
	wxWindow* parent,
	wxWindowID id,
	const wxPoint& pos,
	const wxSize& size
):
	wxPanel(parent, id, pos, size)
{
	const auto* const comp_ref_defs = GetApp().getReflectionManager().getTypeBucket(Gaff::FNV1aHash64Const("IECSComponent"));

	for (const Gaff::IReflectionDefinition* ref_def : *comp_ref_defs) {
		GAFF_REF(ref_def);
	}
}

ArchetypeEditor::~ArchetypeEditor(void)
{
}

NS_END
