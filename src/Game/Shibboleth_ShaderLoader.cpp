/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Shibboleth_ShaderLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_RenderManager.h"
#include <Gleam_IRenderDevice.h>
#include <Gleam_IShader.h>
#include <Gaff_File.h>

NS_SHIBBOLETH

ShaderLoader::ShaderLoader(RenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

ShaderLoader::~ShaderLoader(void)
{
}

Gaff::IVirtualDestructor* ShaderLoader::load(const char* file_name, unsigned long long user_data)
{
	assert(user_data < Gleam::IShader::SHADER_TYPE_SIZE);
	assert(
		(_render_mgr.getRenderDevice().isD3D() && Gaff::File::checkExtension(file_name, ".hlsl")) ||
		(!_render_mgr.getRenderDevice().isD3D() && Gaff::File::checkExtension(file_name, ".glsl"))
	);

	ShaderData* shader_data = GetAllocator().template allocT<ShaderData>();

	if (!shader_data) {
		return nullptr;
	}

	shader_data->shader_type = (Gleam::IShader::SHADER_TYPE)user_data;

	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_render_mgr.getSpinLock());
	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		ShaderPtr shader(_render_mgr.createShader());
		rd.setCurrentDevice(i);

		if (!shader) {
			GetAllocator().freeT(shader_data);
			return nullptr;
		}

		if (!shader->init(rd, file_name, shader_data->shader_type)) {
			GetAllocator().freeT(shader_data);
			return nullptr;
		}

		shader_data->shaders.push(shader);
	}

	return shader_data;
}

NS_END