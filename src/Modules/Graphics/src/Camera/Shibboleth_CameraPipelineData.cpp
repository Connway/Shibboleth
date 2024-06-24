/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Camera/Shibboleth_CameraPipelineData.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::CameraPipelineData)
	.template ctor<>()

	.func("getView", static_cast<const Shibboleth::CameraView& (Shibboleth::CameraPipelineData::*)(int32_t) const>(&Shibboleth::CameraPipelineData::getView))
	.func("getView", static_cast<Shibboleth::CameraView& (Shibboleth::CameraPipelineData::*)(int32_t)>(&Shibboleth::CameraPipelineData::getView))
	.func("createView", &Shibboleth::CameraPipelineData::createView)
	.func("removeView", &Shibboleth::CameraPipelineData::removeView)
SHIB_REFLECTION_DEFINE_END(Shibboleth::CameraPipelineData)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(CameraPipelineData)


const CameraView& CameraPipelineData::getView(int32_t id) const
{
	return const_cast<CameraPipelineData*>(this)->getView(id);
}

CameraView& CameraPipelineData::getView(int32_t id)
{
	return _views[id];
}

int32_t CameraPipelineData::createView(void)
{
	return _views.emplace();
}

void CameraPipelineData::removeView(int32_t id)
{
	_views.remove(id);
}

NS_END
