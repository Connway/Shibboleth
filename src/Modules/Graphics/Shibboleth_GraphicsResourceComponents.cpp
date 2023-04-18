/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::InstanceBufferCount)
	.classAttrs(
		Shibboleth::ECSClassAttribute(nullptr, u8"Graphics")
	)

	.template base< Shibboleth::ECSComponentBaseShared<Shibboleth::InstanceBufferCount> >()
	.var("value", &Shibboleth::InstanceBufferCount::value)
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::InstanceBufferCount)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RasterState)
	.classAttrs(
		Shibboleth::ECSClassAttribute(nullptr, u8"Graphics")
	)

	.template base< Shibboleth::Resource<Shibboleth::RasterStateResource> >()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::RasterState)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::Model)
	.classAttrs(
		Shibboleth::ECSClassAttribute(nullptr, u8"Graphics")
	)

	.template base< Shibboleth::Resource<Shibboleth::ModelResource> >()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::Model)
