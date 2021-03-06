/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Shibboleth_GraphicsResourceComponents.h"
#include <Shibboleth_ECSManager.h>

SHIB_REFLECTION_DEFINE_BEGIN(InstanceBufferCount)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")
	)

	.base< ECSComponentBaseShared<InstanceBufferCount> >()
	.var("value", &InstanceBufferCount::value)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(InstanceBufferCount)

SHIB_REFLECTION_DEFINE_BEGIN(RasterState)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")
	)

	.base< Resource<RasterStateResource> >()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(RasterState)

SHIB_REFLECTION_DEFINE_BEGIN(Model)
	.classAttrs(
		ECSClassAttribute(nullptr, "Graphics")
	)

	.base< Resource<ModelResource> >()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(Model)
