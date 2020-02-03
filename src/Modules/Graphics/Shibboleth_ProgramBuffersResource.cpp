/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_ProgramBuffersResource.h"
#include "Shibboleth_RenderManagerBase.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(ProgramBuffersResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ProgramBuffersResource)
	.classAttrs(CreatableAttribute())

	.BASE(IResource)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ProgramBuffersResource)

Vector<Gleam::IRenderDevice*> ProgramBuffersResource::getDevices(void) const
{
	Vector<Gleam::IRenderDevice*> out{ ProxyAllocator("Graphics") };

	for (const auto& pair : _program_buffers) {
		out.emplace_back(const_cast<Gleam::IRenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool ProgramBuffersResource::createProgramBuffers(const Vector<Gleam::IRenderDevice*>& devices)
{
	bool success = true;

	for (Gleam::IRenderDevice* device : devices) {
		success = success && createProgramBuffers(*device);
	}

	return success;
}

bool ProgramBuffersResource::createProgramBuffers(Gleam::IRenderDevice& device)
{
	const RenderManagerBase& render_mgr = GetApp().GETMANAGERT(RenderManagerBase, RenderManager);
	Gleam::IProgramBuffers* const program_buffers = render_mgr.createProgramBuffers();

	if (!program_buffers) {
		LogErrorResource("Failed to create program buffers '%s'.", getFilePath().getBuffer());
		SHIB_FREET(program_buffers, GetAllocator());
		return false;
	}

	_program_buffers[&device].reset(program_buffers);
	return true;
}

const Gleam::IProgramBuffers* ProgramBuffersResource::getProgramBuffer(const Gleam::IRenderDevice& rd) const
{
	const auto it = _program_buffers.find(&rd);
	return (it != _program_buffers.end()) ? it->second.get() : nullptr;
}

Gleam::IProgramBuffers* ProgramBuffersResource::getProgramBuffer(const Gleam::IRenderDevice& rd)
{
	const auto it = _program_buffers.find(&rd);
	return (it != _program_buffers.end()) ? it->second.get() : nullptr;
}

NS_END
