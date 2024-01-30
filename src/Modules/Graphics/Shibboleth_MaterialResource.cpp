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

#include "Shibboleth_MaterialResource.h"
#include "Shibboleth_RenderManager.h"
#include <Shibboleth_ResourceAttributesCommon.h>
#include <Shibboleth_SerializeReaderWrapper.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_ResourceLogging.h>
#include <Shibboleth_IFileSystem.h>
#include <Gleam_RendererDefines.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::MaterialResource)
	.classAttrs(
		Shibboleth::CreatableAttribute(),
		Shibboleth::ResourceExtensionAttribute(u8".material.bin"),
		Shibboleth::ResourceExtensionAttribute(u8".material")
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::MaterialResource)

namespace
{
	static Shibboleth::ProxyAllocator g_allocator("Graphics");
}

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(MaterialResource)

void MaterialResource::load(const ISerializeReader& reader, uintptr_t thread_id_int)
{
	const RenderManager& render_mgr = GetManagerTFast<RenderManager>();
	ResourceManager& res_mgr = GetManagerTFast<ResourceManager>();
	const Vector<Gleam::RenderDevice*>* devices = nullptr;
	U8String device_tag;

	{
		const auto guard = reader.enterElementGuard(u8"devices_tag");

		if (!reader.isNull() && !reader.isString()) {
			LogErrorResource("Malformed material '%s'. 'devices_tag' is not string.", getFilePath().getBuffer());
			failed();
			return;
		}

		const char8_t* const tag = reader.readString(u8"main");
		device_tag = tag;
		devices = render_mgr.getDevicesByTag(tag);
		reader.freeString(tag);
	}

	if (!devices || devices->empty()) {
		LogErrorResource("Failed to load material '%s'. Devices tag '%s' has no render devices associated with it.", getFilePath().getBuffer(), device_tag.data());
		failed();
		return;
	}

	ResourcePtr<ShaderResource> shaders[static_cast<size_t>(Gleam::IShader::Type::Count)];

	// Process the compute section first.
	{
		const auto guard = reader.enterElementGuard(u8"compute");

		// If a compute shader was specified, then don't process any other fields.
		if (!reader.isNull()) {
			if (!reader.isString()) {
				LogErrorResource("Malformed material '%s'. Element 'compute' is not a string.", getFilePath().getBuffer());
				failed();
				return;
			}

			const char8_t* const res_path = reader.readString();
			const U8String final_path = U8String(res_path) + Gleam::IShader::g_shader_extensions[static_cast<int32_t>(Gleam::k_Renderer)];
			reader.freeString(res_path);

			const HashStringView64<> path_hash(final_path);
			ResourcePtr<ShaderResource> compute = res_mgr.getResourceT<ShaderResource>(path_hash);

			if (!compute) {
				const IFile* const compute_file = res_mgr.loadFileAndWait(final_path.data(), thread_id_int);

				if (!compute_file) {
					LogErrorResource("Failed to load material '%s'. Failed to load compute shader '%s'.", getFilePath().getBuffer(), final_path.data());
					failed();
					return;
				}

				compute = res_mgr.createResourceT<ShaderResource>(path_hash);

				const bool success = compute->createShaderAndLayout(
					*devices,
					reinterpret_cast<const char*>(compute_file->getBuffer()),
					Gleam::IShader::Type::Compute);

				if (!success) {
					LogErrorResource("Failed to load material '%s'. Failed to create compute shader '%s'.", getFilePath().getBuffer(), final_path.data());
					failed();
					return;
				}
			}


			res_mgr.waitForResource(*compute);

			if (compute->hasFailed()) {
				LogErrorResource("Failed to load material '%s'. Failed to load compute shader '%s'.", getFilePath().getBuffer(), compute->getFilePath().getBuffer());
				failed();
				return;

			} else {
				if (createProgram(*devices, compute)) {
					succeeded();
				} else {
					failed();
				}
			}

			return;
		}
	}

	constexpr const char* shader_elements[static_cast<size_t>(Gleam::IShader::Type::Count)] = {
		"vertex",
		"pixel",
		"domain",
		"geometry",
		"hull",
		nullptr
	};

	int32_t index = 0;

	for (const char* element : shader_elements) {
		if (!element) {
			++index;
			continue;
		}

		const auto guard = reader.enterElementGuard(element);

		if (reader.isNull()) {
			++index;
			continue;

		} else if (!reader.isString()) {
			LogErrorResource("Malformed material '%s'. Element '%s' is not a string.", getFilePath().getBuffer(), element);

			if (!hasFailed()) {
				failed();
			}

			++index;
			continue;
		}

		const char8_t* const res_path = reader.readString();
		const U8String final_path = U8String(res_path) + Gleam::IShader::g_shader_extensions[static_cast<int32_t>(Gleam::k_Renderer)];
		reader.freeString(res_path);

		const HashStringView64<> path_hash(final_path);
		shaders[index] = res_mgr.getResourceT<ShaderResource>(path_hash);

		if (!shaders[index]) {
			const IFile* const shader_file = res_mgr.loadFileAndWait(final_path.data(), thread_id_int);

			if (!shader_file) {
				LogErrorResource("Failed to load material '%s'. Failed to load compute shader '%s'.", getFilePath().getBuffer(), final_path.data());
				failed();
				return;
			}

			shaders[index] = res_mgr.createResourceT<ShaderResource>(path_hash);

			const bool success = shaders[index]->createShaderAndLayout(
				*devices,
				reinterpret_cast<const char*>(shader_file->getBuffer()),
				static_cast<Gleam::IShader::Type>(index));

			if (!success) {
				LogErrorResource("Failed to load material '%s'. Failed to create compute shader '%s'.", getFilePath().getBuffer(), final_path.data());
				failed();
				return;
			}
		}

		++index;
	}

	const bool success = createProgram(
		*devices,
		shaders[static_cast<int32_t>(Gleam::IShader::Type::Vertex)],
		shaders[static_cast<int32_t>(Gleam::IShader::Type::Pixel)],
		shaders[static_cast<int32_t>(Gleam::IShader::Type::Domain)],
		shaders[static_cast<int32_t>(Gleam::IShader::Type::Geometry)],
		shaders[static_cast<int32_t>(Gleam::IShader::Type::Hull)]
	);

	if (success) {
		succeeded();
	} else {
		failed();
	}
}

Vector<Gleam::RenderDevice*> MaterialResource::getDevices(void) const
{
	for (const auto& shader : _shaders) {
		if (shader) {
			return shader->getDevices();
		}
	}

	return Vector<Gleam::RenderDevice*>(g_allocator);
}

bool MaterialResource::createProgram(
	const Vector<Gleam::RenderDevice*>& devices,
	ResourcePtr<ShaderResource>& vertex,
	ResourcePtr<ShaderResource>& pixel,
	ResourcePtr<ShaderResource>& domain,
	ResourcePtr<ShaderResource>& geometry,
	ResourcePtr<ShaderResource>& hull)
{
	bool success = true;

	for (Gleam::RenderDevice* device : devices) {
		success = success && createProgram(*device, vertex, pixel, domain, geometry, hull);
	}

	return success;
}

bool MaterialResource::createProgram(
	Gleam::RenderDevice& device,
	ResourcePtr<ShaderResource>& vertex,
	ResourcePtr<ShaderResource>& pixel,
	ResourcePtr<ShaderResource>& domain,
	ResourcePtr<ShaderResource>& geometry,
	ResourcePtr<ShaderResource>& hull)
{
	Gleam::Shader* const vert = (vertex) ? vertex->getShader(device) : nullptr;
	Gleam::Shader* const pix = (pixel) ? pixel->getShader(device) : nullptr;
	Gleam::Shader* const dom = (domain) ? domain->getShader(device) : nullptr;
	Gleam::Shader* const geo = (geometry) ? geometry->getShader(device) : nullptr;
	Gleam::Shader* const hul = (hull) ? hull->getShader(device) : nullptr;

	if (vertex && !vert) {
		LogErrorResource("Failed to create material '%s'. Vertex shader '%s' was not initialized with the given device.", getFilePath().getBuffer(), vertex->getFilePath().getBuffer());
		return false;
	}

	if (pixel && !pix) {
		LogErrorResource("Failed to create material '%s'. Pixel shader '%s' was not initialized with the given device.", getFilePath().getBuffer(), pixel->getFilePath().getBuffer());
		return false;
	}

	if (domain && !dom) {
		LogErrorResource("Failed to create material '%s'. Domain shader '%s' was not initialized with the given device.", getFilePath().getBuffer(), domain->getFilePath().getBuffer());
		return false;
	}

	if (geometry && !geo) {
		LogErrorResource("Failed to create material '%s'. Geometry shader '%s' was not initialized with the given device.", getFilePath().getBuffer(), geometry->getFilePath().getBuffer());
		return false;
	}

	if (hull && !hul) {
		LogErrorResource("Failed to create material '%s'. Hull shader '%s' was not initialized with the given device.", getFilePath().getBuffer(), hull->getFilePath().getBuffer());
		return false;
	}

	Gleam::Program* const program = SHIB_ALLOCT(Gleam::Program, g_allocator);

	if (vert) {
		_shaders[static_cast<int32_t>(Gleam::IShader::Type::Vertex)] = vertex;
		program->attach(vert);
	}

	if (pix) {
		_shaders[static_cast<int32_t>(Gleam::IShader::Type::Pixel)] = pixel;
		program->attach(pix);
	}

	if (dom) {
		_shaders[static_cast<int32_t>(Gleam::IShader::Type::Domain)] = domain;
		program->attach(dom);
	}

	if (geo) {
		_shaders[static_cast<int32_t>(Gleam::IShader::Type::Geometry)] = geometry;
		program->attach(geo);
	}

	if (hul) {
		_shaders[static_cast<int32_t>(Gleam::IShader::Type::Hull)] = hull;
		program->attach(hul);
	}

	_programs[&device].reset(program);

	return true;
}

bool MaterialResource::createProgram(const Vector<Gleam::RenderDevice*>& devices, ResourcePtr<ShaderResource>& vertex, ResourcePtr<ShaderResource>& pixel)
{
	ResourcePtr<ShaderResource> empty;
	return createProgram(devices, vertex, pixel, empty, empty, empty);
}

bool MaterialResource::createProgram(Gleam::RenderDevice& device, ResourcePtr<ShaderResource>& vertex, ResourcePtr<ShaderResource>& pixel)
{
	ResourcePtr<ShaderResource> empty;
	return createProgram(device, vertex, pixel, empty, empty, empty);
}

bool MaterialResource::createProgram(const Vector<Gleam::RenderDevice*>& devices, ResourcePtr<ShaderResource>& compute)
{
	bool success = true;

	for (Gleam::RenderDevice* device : devices) {
		success = success && createProgram(*device, compute);
	}

	return success;
}

bool MaterialResource::createProgram(Gleam::RenderDevice& device, ResourcePtr<ShaderResource>& compute)
{
	Gleam::Shader* const shader = compute->getShader(device);

	if (!shader) {
		LogErrorResource("Failed to create material '%s'. Compute shader '%s' was not initialized with the given device.", getFilePath().getBuffer(), compute->getFilePath().getBuffer());
		return false;
	}

	Gleam::Program* const program = SHIB_ALLOCT(Gleam::Program, g_allocator);

	_shaders[static_cast<int32_t>(Gleam::IShader::Type::Compute)] = compute;
	program->attach(shader);

	_programs[&device].reset(program);

	return true;
}

const Gleam::Program* MaterialResource::getProgram(const Gleam::RenderDevice& device) const
{
	const auto it = _programs.find(&device);
	return (it != _programs.end()) ? it->second.get() : nullptr;
}

Gleam::Program* MaterialResource::getProgram(const Gleam::RenderDevice& device)
{
	const auto it = _programs.find(&device);
	return (it != _programs.end()) ? it->second.get() : nullptr;
}

const Gleam::Layout* MaterialResource::getLayout(const Gleam::RenderDevice& device) const
{
	const ResourcePtr<ShaderResource>& shader = _shaders[static_cast<int32_t>(Gleam::IShader::Type::Vertex)];
	return (shader) ? shader->getLayout(device) : nullptr;
}

Gleam::Layout* MaterialResource::getLayout(const Gleam::RenderDevice& device)
{
	ResourcePtr<ShaderResource>& shader = _shaders[static_cast<int32_t>(Gleam::IShader::Type::Vertex)];
	return (shader) ? shader->getLayout(device) : nullptr;
}

NS_END
