/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include "Shibboleth_IRenderManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IShader.h>
#include <Gaff_File.h>

NS_SHIBBOLETH

ShaderLoader::ShaderLoader(IRenderManager& render_mgr):
	_render_mgr(render_mgr)
{
}

ShaderLoader::~ShaderLoader(void)
{
}

ResourceLoadData ShaderLoader::load(const IFile* file, ResourceContainer* res_cont)
{
	GAFF_ASSERT(res_cont->getUserData() < Gleam::IShader::SHADER_TYPE_SIZE);

	ShaderData* shader_data = SHIB_ALLOCT(ShaderData, *GetAllocator());

	if (!shader_data) {
		return { nullptr };
	}

	shader_data->shader_type = static_cast<Gleam::IShader::SHADER_TYPE>(res_cont->getUserData());

	Gleam::IRenderDevice& rd = _render_mgr.getRenderDevice();

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		ShaderPtr shader(_render_mgr.createShader());
		rd.setCurrentDevice(i);

		if (!shader) {
			SHIB_FREET(shader_data, *GetAllocator());
			return { nullptr };
		}

		if (!shader->initSource(rd, file->getBuffer(), file->size(), shader_data->shader_type)) {
			SHIB_FREET(shader_data, *GetAllocator());
			return { nullptr };
		}

		shader_data->shaders.push(shader);
	}

	return { shader_data };
}

NS_END