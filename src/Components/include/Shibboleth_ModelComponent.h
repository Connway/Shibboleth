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

#include "Shibboleth_IModelComponent.h"
#include <Shibboleth_IOcclusionManager.h>
#include <Shibboleth_Component.h>

NS_SHIBBOLETH

class ModelComponent : public Component, public IModelComponent
{
public:
	ModelComponent(void);
	~ModelComponent(void);

	const Gaff::JSON& getSchema(void) const override;
	bool load(const Gaff::JSON& json) override;
	bool save(Gaff::JSON& json) override;

	void allComponentsLoaded(void) override;
	void addToWorld(void) override;
	void removeFromWorld(void) override;

	void setActive(bool active) override;

	void setStatic(bool is_static) override;
	bool isStatic(void) const override;

	size_t determineLOD(const Gleam::Vector4CPU& pos) override;

	//unsigned int getMeshInstanceHash(size_t mesh) const override;

	const Array<MeshData>& getMeshData(void) const override;
	Array<MeshData>& getMeshData(void) override;
	
	const ModelData& getModel(void) const override;
	ModelData& getModel(void) override;

private:
	ResourceWrapper<ModelData> _model;
	Array<MeshData> _mesh_data;

	IOcclusionManager::OcclusionID _occlusion_id;

	volatile size_t _requests_finished;
	volatile size_t _total_requests;

	IOcclusionManager& _occlusion_mgr;

	char _flags;

	void resourceLoadedCallback(ResourceContainer* resource);

	void requestResources(const Gaff::JSON& materials, IResourceManager& res_mgr);
	void setupResources(void);

	void addToOcclusionManager(void);
	void removeFromOcclusionManager(void);

	SHIB_REF_DEF(ModelComponent);
	REF_DEF_REQ;
};

NS_END
