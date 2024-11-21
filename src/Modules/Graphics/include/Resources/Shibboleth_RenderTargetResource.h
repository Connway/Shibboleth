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

#include <Shibboleth_IResource.h>
#include <Shibboleth_Error.h>
#include <Gleam_IRenderTarget.h>

NS_GLEAM
	class RenderDevice;
	class RenderTarget;
NS_END

NS_SHIBBOLETH

class RenderTargetResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	Gleam::RenderDevice* getDevice(void) const;

	Error createRenderTarget(Gleam::RenderDevice& device);

	const Gleam::RenderTarget* getRenderTarget(void) const;
	Gleam::RenderTarget* getRenderTarget(void);

	void setNoClear(bool no_clear);
	bool canClear(void) const;

	void setClearSettings(const Gleam::IRenderTarget::ClearSettings& settings);
	const Gleam::IRenderTarget::ClearSettings& getClearSettings(void) const;

private:
	enum class Flag
	{
		NoClear,

		Count
	};

	Gleam::IRenderTarget::ClearSettings _clear_settings;

	UniquePtr<Gleam::RenderTarget> _render_target;
	Gleam::RenderDevice* _render_device = nullptr;

	Gaff::Flags<Flag> _flags;

	SHIB_REFLECTION_CLASS_DECLARE(RenderTargetResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::RenderTargetResource)
