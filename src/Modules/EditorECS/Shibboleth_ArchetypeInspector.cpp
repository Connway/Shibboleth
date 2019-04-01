/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_ArchetypeInspector.h"
#include "Shibboleth_ArchetypeEditor.h"
#include <Shibboleth_EditorInspectorAttribute.h>
#include <Shibboleth_Inspector.h>

SHIB_REFLECTION_DEFINE(ArchetypeInspector)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ArchetypeInspector)
	.CTOR(void*, const Gaff::IReflectionDefinition&, const Gaff::IReflectionDefinition*, int32_t, wxWindow*)

	.BASE(Gaff::IReflectionObject)
	.BASE(wxWindow)

	.classAttrs(EditorInspectorAttribute<ArchetypeEditor>())
SHIB_REFLECTION_CLASS_DEFINE_END(ArchetypeInspector)

ArchetypeInspector::ArchetypeInspector(
	void* value,
	const Gaff::IReflectionDefinition& /*ref_def*/,
	const Gaff::IReflectionDefinition* /*parent_ref_def*/,
	int32_t /*var_index*/,
	wxWindow* parent
):
	wxPanel(parent),
	_editor(reinterpret_cast<ArchetypeEditor*>(value))
{
	ECSArchetype& archetype = _editor->getArchetype();

	int8_t* shared_data = reinterpret_cast<int8_t*>(archetype.getSharedData());
	const int32_t shared_count = archetype.getNumSharedComponents();

	for (int32_t i = 0; i < shared_count; ++i) {
		const Gaff::IReflectionDefinition& ref_def = archetype.getSharedComponentRefDef(i);

		shared_data += ref_def.size();
	}
}

NS_END
