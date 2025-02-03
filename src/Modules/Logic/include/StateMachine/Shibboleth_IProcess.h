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

#pragma once

#include <Containers/Shibboleth_InstancedArray.h>

NS_SHIBBOLETH

class StateMachine;

class IProcess : public Refl::IReflectionObject
{
public:
	virtual ~IProcess(void) {}

	virtual bool initInstance(const StateMachine& /*owner*/, InstancedArrayAny& /*instance_data*/) { return true; }
	virtual bool init(const StateMachine& /*owner*/) { return true; }

	virtual void deinitInstance(const StateMachine& /*owner*/, InstancedArrayAny& /*instance_data*/) {}

	virtual void update(const StateMachine& owner, InstancedArrayAny& instance_data) = 0;

	SHIB_REFLECTION_CLASS_DECLARE(IProcess);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::IProcess)

#ifdef SHIB_REFL_IMPL
	#include "Attributes/Shibboleth_EngineAttributesCommon.h"

	SHIB_REFLECTION_BUILD_BEGIN(Shibboleth::IProcess)
		.classAttrs(
			Shibboleth::ClassBucketAttribute()
		)
	SHIB_REFLECTION_BUILD_END(Shibboleth::IProcess)
#endif
