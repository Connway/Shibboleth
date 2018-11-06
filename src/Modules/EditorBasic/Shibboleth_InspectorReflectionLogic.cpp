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

#include "Shibboleth_InspectorReflectionLogic.h"

NS_SHIBBOLETH

wxWindow* CreateInspectorWidgetWithReflection(Gaff::IReflectionObject& object)
{
	const Gaff::IReflectionDefinition& ref_def = object.getReflectionDefinition();
	const int32_t num_vars = ref_def.getNumVars();

	for (int32_t i = 0; i < num_vars; ++i) {

	}

	return nullptr;
}

void InspectorReflectionLogic::populate(Gaff::IReflectionObject& inspector, Gaff::IReflectionObject& object)
{
	GAFF_REF(inspector);
	GAFF_REF(object);
}

NS_END
