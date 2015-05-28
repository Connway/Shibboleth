/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_ResourceWrapper.h"
#include "Shibboleth_IComponent.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_RenderManager.h>
#include <Gleam_IBuffer.h>

NS_SHIBBOLETH

class LoadingMessage;
struct ModelData;

class ModelComponent : public IComponent
{
public:
	ModelComponent(void);
	~ModelComponent(void);

	bool validate(Gaff::JSON& json);
	bool load(const Gaff::JSON& json);
	bool save(Gaff::JSON& json);

	void allComponentsLoaded(void);

	static const char* getComponentName(void)
	{
		return "Model Component";
	}

	void* rawRequestInterface(unsigned int class_id) const;

	void render(double dt); // Temporary test function

	const ProgramBuffersData& getProgramBuffers(void) const;
	const ProgramData& getMaterials(void) const;
	const ModelData& getModels(void) const;

private:
	Gleam::IBuffer::BufferSettings _buffer_settings;

	ResourceWrapper<ProgramBuffersData> _program_buffers_res;
	ResourceWrapper<SamplerStateData> _sampler_res;
	ResourceWrapper<ProgramData> _material_res;
	ResourceWrapper<TextureData> _texture_res;
	ResourceWrapper<BufferData> _buffer_res;
	ResourceWrapper<ModelData> _model_res;

	RenderManager& _render_mgr;

	RenderModes _render_mode_override;

	unsigned int _current_lod;

	void ProgramBuffersCallback(const AHashString& /*resource*/, bool success);
	void TextureLoadedCallback(const AHashString& /*resource*/, bool success);
	void SamplerStateCallback(const AHashString& /*resource*/, bool success);
	void BufferCallback(const AHashString& /*resource*/, bool success);
	void LoadCallback(const AHashString& resource, bool success);
	void HandleLoadingMessage(const LoadingMessage& msg);

	SHIB_REF_DEF(ModelComponent);
};

NS_END
