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

#include "Shibboleth_BufferResource.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_ResourceLogging.h>
#include <Gleam_RenderDevice.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::BufferResource)
.classAttrs(Shibboleth::CreatableAttribute())

.template base<Shibboleth::IResource>()
.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::BufferResource)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(BufferResource)

static ProxyAllocator g_allocator("Graphics");

Vector<Gleam::RenderDevice*> BufferResource::getDevices(void) const
{
	Vector<Gleam::RenderDevice*> out(g_allocator);

	for (const auto& pair : _buffers) {
		out.emplace_back(const_cast<Gleam::RenderDevice*>(pair.first));
	}

	out.shrink_to_fit();
	return out;
}

bool BufferResource::createBuffer(const Vector<Gleam::RenderDevice*>& devices, const Gleam::IBuffer::Settings& buffer_settings)
{
	bool success = true;

	for (Gleam::RenderDevice* device : devices) {
		success = success && createBuffer(*device, buffer_settings);
	}

	return success;
}

bool BufferResource::createBuffer(Gleam::RenderDevice& device, const Gleam::IBuffer::Settings& buffer_settings)
{
	Gleam::Buffer* const buffer = SHIB_ALLOCT(Gleam::Buffer, g_allocator);

	if (!buffer->init(device, buffer_settings)) {
		LogErrorResource("Failed to create buffer '%s'.", getFilePath().getBuffer());
		SHIB_FREET(buffer, GetAllocator());
		return false;
	}

	_buffers[&device].reset(buffer);
	return true;
}

const Gleam::Buffer* BufferResource::getBuffer(const Gleam::RenderDevice& rd) const
{
	const auto it = _buffers.find(&rd);
	return (it != _buffers.end()) ? it->second.get() : nullptr;
}

Gleam::Buffer* BufferResource::getBuffer(const Gleam::RenderDevice& rd)
{
	const auto it = _buffers.find(&rd);
	return (it != _buffers.end()) ? it->second.get() : nullptr;
}

NS_END
