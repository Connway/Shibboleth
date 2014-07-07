/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Shibboleth_TestComponent.h"
#include "Shibboleth_App.h"

NS_SHIBBOLETH

EnumReflectionDefinition<TestComponent::TestEnum> TestComponent::_enum_ref_def;
ReflectionDefinition<TestComponent> TestComponent::_ref_def;

COMP_REF_DEF_LOAD(TestComponent, _ref_def);
COMP_REF_DEF_SAVE(TestComponent, _ref_def);

TestComponent::TestComponent(App& app):
	_app(app)
{
	if (!_enum_ref_def.isDefined()) {
		_enum_ref_def.addValue("A", TE_A);
		_enum_ref_def.addValue("B", TE_B);

		_enum_ref_def.markDefined();
	}

	if (!_ref_def.isDefined()) {
		_ref_def.setAllocator(ProxyAllocator());

		_ref_def.addEnum("e", &TestComponent::_e, _enum_ref_def);
		_ref_def.addFloat("a", &TestComponent::_a);
		_ref_def.addShort("b", &TestComponent::_b);

		// do addX() calls here
		_ref_def.markDefined();
	}
}

void TestComponent::allComponentsLoaded(void)
{
}

NS_END
