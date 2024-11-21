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
#include "Resources/Shibboleth_ProgramBuffersResource.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_AppUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ProgramBuffersResource)
	.classAttrs(Shibboleth::CreatableAttribute())

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::ProgramBuffersResource)


namespace
{
	static Shibboleth::ProxyAllocator g_allocator("Graphics");
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(ProgramBuffersResource)

Vector<Gleam::RenderDevice*> ProgramBuffersResource::getDevices(void) const
{
	Vector<Gleam::RenderDevice*> out{ GRAPHICS_ALLOCATOR };

	for (const auto& pair : _program_buffers) {
		out.emplace_back(const_cast<Gleam::RenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool ProgramBuffersResource::createProgramBuffers(const Vector<Gleam::RenderDevice*>& devices)
{
	bool success = true;

	for (Gleam::RenderDevice* device : devices) {
		success = success && createProgramBuffers(*device);
	}

	return success;
}

bool ProgramBuffersResource::createProgramBuffers(Gleam::RenderDevice& device)
{
	Gleam::ProgramBuffers* const program_buffers = SHIB_ALLOCT(Gleam::ProgramBuffers, g_allocator);

	if (!program_buffers) {
		LogErrorResource("Failed to create program buffers '%s'.", getFilePath().getBuffer());
		SHIB_FREET(program_buffers, GetAllocator());
		return false;
	}

	_program_buffers[&device].reset(program_buffers);
	return true;
}

const Gleam::ProgramBuffers* ProgramBuffersResource::getProgramBuffer(const Gleam::RenderDevice& rd) const
{
	const auto it = _program_buffers.find(&rd);
	return (it != _program_buffers.end()) ? it->second.get() : nullptr;
}

Gleam::ProgramBuffers* ProgramBuffersResource::getProgramBuffer(const Gleam::RenderDevice& rd)
{
	const auto it = _program_buffers.find(&rd);
	return (it != _program_buffers.end()) ? it->second.get() : nullptr;
}

NS_END
