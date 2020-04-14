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

#include "Shibboleth_BufferResource.h"
#include "Shibboleth_IRenderManager.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE_BEGIN(BufferResource)
	.classAttrs(CreatableAttribute())

	.base<IResource>()
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(BufferResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(BufferResource)

Vector<Gleam::IRenderDevice*> BufferResource::getDevices(void) const
{
	Vector<Gleam::IRenderDevice*> out{ ProxyAllocator("Graphics") };

	for (const auto& pair : _buffers) {
		out.emplace_back(const_cast<Gleam::IRenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool BufferResource::createBuffer(const Vector<Gleam::IRenderDevice*>& devices, const Gleam::IBuffer::BufferSettings& buffer_settings)
{
	bool success = true;

	for (Gleam::IRenderDevice* device : devices) {
		success = success && createBuffer(*device, buffer_settings);
	}

	return success;
}

bool BufferResource::createBuffer(Gleam::IRenderDevice& device, const Gleam::IBuffer::BufferSettings& buffer_settings)
{
	const IRenderManager& render_mgr = GetApp().GETMANAGERT(IRenderManager, RenderManager);
	Gleam::IBuffer* const buffer = render_mgr.createBuffer();

	if (!buffer->init(device, buffer_settings)) {
		LogErrorResource("Failed to create buffer '%s'.", getFilePath().getBuffer());
		SHIB_FREET(buffer, GetAllocator());
		return false;
	}

	_buffers[&device].reset(buffer);
	return true;
}

const Gleam::IBuffer* BufferResource::getBuffer(const Gleam::IRenderDevice& rd) const
{
	const auto it = _buffers.find(&rd);
	return (it != _buffers.end()) ? it->second.get() : nullptr;
}

Gleam::IBuffer* BufferResource::getBuffer(const Gleam::IRenderDevice& rd)
{
	const auto it = _buffers.find(&rd);
	return (it != _buffers.end()) ? it->second.get() : nullptr;
}

NS_END
