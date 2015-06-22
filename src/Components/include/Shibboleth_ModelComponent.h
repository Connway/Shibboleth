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

namespace Gleam {
	class Vector4CPU;
}

NS_SHIBBOLETH

class ResourceManager;
class RenderManager;
//class LoadingMessage;
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

	INLINE static const char* getComponentName(void)
	{
		return "Model Component";
	}

	void* rawRequestInterface(unsigned int class_id) const;

	void render(double dt); // Temporary test function

	size_t determineLOD(const Gleam::Vector4CPU& pos);

	INLINE const Array< ResourceWrapper<ProgramBuffersData> >& getProgramBuffers(void) const;
	INLINE Array< ResourceWrapper<ProgramBuffersData> >& getProgramBuffers(void);

	INLINE const Array< ResourceWrapper<ProgramData> >& getMaterials(void) const;
	INLINE Array< ResourceWrapper<ProgramData> >& getMaterials(void);

	INLINE const ModelData& getModel(void) const;
	INLINE ModelData& getModel(void);

private:
	Array< ResourceWrapper<ProgramBuffersData> > _program_buffers;
	Array< ResourceWrapper<SamplerStateData> > _samplers;
	Array< ResourceWrapper<ProgramData> > _materials;
	Array< ResourceWrapper<TextureData> > _textures;
	Array< ResourceWrapper<BufferData> >  _buffers;
	ResourceWrapper<ModelData> _model;

	Array<Gleam::IBuffer::BufferSettings> _buffer_settings;
	Array< Array<MaterialMapping> > _buffer_mappings;
	Array< Array<MaterialMapping> > _sampler_mappings;
	Array< Array<MaterialMapping> > _texture_mappings;

	RenderModes _render_mode_override;

	volatile size_t _requests_finished;
	volatile size_t _total_requests;

	RenderManager& _render_mgr;

	void ResourceLoadedCallback(ResourceContainer* resource);
	//void HandleLoadingMessage(const LoadingMessage& msg);

	void requestMaterials(const Gaff::JSON& json, ResourceManager& res_mgr);
	void requestTextures(const Gaff::JSON& json, ResourceManager& res_mgr);
	void requestSamplers(const Gaff::JSON& json, ResourceManager& res_mgr);
	void requestBuffers(const Gaff::JSON& json, ResourceManager& res_mgr);
	void setupResources(void);

	bool _init;

	SHIB_REF_DEF(ModelComponent);
};

NS_END
