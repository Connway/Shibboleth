/************************************************************************************
Copyright (C) by Nicholas LaCroix
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

#define SHIB_REFL_IMPL
#include "StateMachine/Processes/Shibboleth_AngelScriptProcess.h"
#include "Shibboleth_StateMachineReflection.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::AngelScriptProcess)
	.template base<Shibboleth::IProcess>()
	.template ctor<>()

	.var("script", &Type::_script)
SHIB_REFLECTION_DEFINE_END(Shibboleth::AngelScriptProcess)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(AngelScriptProcess)

bool AngelScriptProcess::init(const StateMachine& owner)
{
	// $TODO: Create instance of script class.
	// $TODO: Call script class instance.
	GAFF_REF(owner);
	return true;
}

void AngelScriptProcess::update(const StateMachine& owner, InstancedArrayAny& instance_data)
{
	// $TODO: Call script class instance.
	GAFF_REF(owner, instance_data);
}

NS_END
