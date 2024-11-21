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
#include "Resources/Shibboleth_RenderTargetResource.h"
#include "Shibboleth_GraphicsDefines.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Gleam_RenderTarget.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::RenderTargetResource)
	.classAttrs(
		Shibboleth::CreatableAttribute()
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::RenderTargetResource)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(RenderTargetResource)

Gleam::RenderDevice* RenderTargetResource::getDevice(void) const
{
	return _render_device;
}

Error RenderTargetResource::createRenderTarget(Gleam::RenderDevice& device)
{
	_render_device = &device;
	_render_target.reset(SHIB_ALLOCT(Gleam::RenderTarget, GRAPHICS_ALLOCATOR));

	// This could arguably be a fatal error.
	return (_render_target != nullptr) ? Error::k_no_error : Error::k_simple_error;
}

const Gleam::RenderTarget* RenderTargetResource::getRenderTarget(void) const
{
	return _render_target.get();
}

Gleam::RenderTarget* RenderTargetResource::getRenderTarget(void)
{
	return _render_target.get();
}

void RenderTargetResource::setNoClear(bool no_clear)
{
	_flags.set(no_clear, Flag::NoClear);
}

bool RenderTargetResource::canClear(void) const
{
	return !_flags.testAll(Flag::NoClear);
}

void RenderTargetResource::setClearSettings(const Gleam::IRenderTarget::ClearSettings& settings)
{
	_clear_settings = settings;
}

const Gleam::IRenderTarget::ClearSettings& RenderTargetResource::getClearSettings(void) const
{
	return _clear_settings;
}

NS_END
