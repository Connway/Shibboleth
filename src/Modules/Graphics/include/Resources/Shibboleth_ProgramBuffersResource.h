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

#pragma once

#include "Shibboleth_GraphicsDefines.h"
#include <Shibboleth_IResource.h>
#include <Gleam_ProgramBuffers.h>

NS_GLEAM
	class RenderDevice;
NS_END

NS_SHIBBOLETH

class ProgramBuffersResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	Vector<Gleam::RenderDevice*> getDevices(void) const;

	bool createProgramBuffers(const Vector<Gleam::RenderDevice*>& devices);
	bool createProgramBuffers(Gleam::RenderDevice& device);

	const Gleam::ProgramBuffers* getProgramBuffer(const Gleam::RenderDevice& rd) const;
	Gleam::ProgramBuffers* getProgramBuffer(const Gleam::RenderDevice& rd);

private:
	VectorMap< const Gleam::RenderDevice*, UniquePtr<Gleam::ProgramBuffers> > _program_buffers{ GRAPHICS_ALLOCATOR };

	SHIB_REFLECTION_CLASS_DECLARE(ProgramBuffersResource);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ProgramBuffersResource)