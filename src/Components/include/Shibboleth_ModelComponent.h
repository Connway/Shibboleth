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

#pragma once

#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_ResourceWrapper.h"
#include "Shibboleth_Component.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_OcclusionManager.h>
#include <Gleam_IBuffer.h>

NS_GLEAM
	class Vector4CPU;
NS_END

NS_SHIBBOLETH

class ResourceManager;
class RenderManager;
//class LoadingMessage;
struct ModelData;

class ModelComponent : public Component
{
public:
	struct MeshData
	{
		ResourceWrapper<ProgramBuffersData> program_buffers;
		ResourceWrapper<ProgramData> material;

		Array< ResourceWrapper<TextureData> > textures;
		Array< ResourceWrapper<SamplerStateData> > samplers;
		//Array< ResourceWrapper<BufferData> > _buffers;
	};

	static const char* getComponentName(void)
	{
		return "Model Component";
	}

	ModelComponent(void);
	~ModelComponent(void);

	const Gaff::JSON& getSchema(void) const override;
	bool load(const Gaff::JSON& json) override;
	bool save(Gaff::JSON& json) override;

	void allComponentsLoaded(void) override;
	void addToWorld(void) override;
	void removeFromWorld(void) override;

	void setActive(bool active) override;

	void* rawRequestInterface(Gaff::ReflectionHash class_id) const override;

	void setStatic(bool is_static);
	bool isStatic(void) const;

	size_t determineLOD(const Gleam::Vector4CPU& pos);

	//unsigned int getMeshInstanceHash(size_t mesh) const;

	INLINE const Array<MeshData>& getMeshData(void) const;
	INLINE Array<MeshData>& getMeshData(void);
	
	INLINE const ModelData& getModel(void) const;
	INLINE ModelData& getModel(void);

private:
	ResourceWrapper<ModelData> _model;
	Array<MeshData> _mesh_data;

	OcclusionManager::OcclusionID _occlusion_id;

	volatile size_t _requests_finished;
	volatile size_t _total_requests;

	OcclusionManager& _occlusion_mgr;

	char _flags;

	void ResourceLoadedCallback(ResourceContainer* resource);

	void requestResources(const Gaff::JSON& materials, ResourceManager& res_mgr);
	void setupResources(void);

	void addToOcclusionManager(void);
	void removeFromOcclusionManager(void);

	SHIB_REF_DEF(ModelComponent);
};

NS_END
