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

#include "Shibboleth_IResourceLoader.h"
#include <Shibboleth_ProxyAllocator.h>
#include <Shibboleth_Array.h>

NS_GAFF
	class SceneNode;
	class JSON;
	class Mesh;
NS_END

namespace Gleam {
	class IRenderDevice;
	class IShader;
	class IModel;
}

namespace Esprit {
	class Skeleton;
}

NS_SHIBBOLETH

struct GraphicsUserData;
class IResourceManager;
class IRenderManager;
class IFileSystem;
struct HoldingData;
struct ModelData;

class ModelLoader : public IResourceLoader
{
public:
	ModelLoader(IRenderManager& render_mgr, IResourceManager& res_mgr, IFileSystem& file_system);
	~ModelLoader(void);

	ResourceLoadData load(const IFile* file, ResourceContainer* res_cont) override;

private:
	struct VertSkeletonData
	{
		Array<unsigned int> bone_indices;
		Array<float> bone_weights;
	};

	IRenderManager& _render_mgr;
	IResourceManager& _res_mgr;
	IFileSystem& _file_system;

	bool loadMeshes(ModelData* data, const Gaff::JSON& lod_tags, const Gaff::JSON& model_prefs, const HoldingData& holding_data, unsigned short display_tags, bool any_display_tags);
	bool createMeshAndLayout(Gleam::IRenderDevice& rd, const Gaff::Mesh& scene_mesh, Gleam::IModel* model, const Gaff::JSON& model_prefs, unsigned int num_bone_weights, const Array<VertSkeletonData>& vert_skeleton_data);
	unsigned int generateLoadingFlags(const Gaff::JSON& model_prefs);
	Gleam::IShader* generateEmptyD3D11Shader(Gleam::IRenderDevice& rd, const Gaff::JSON& model_prefs, const Gaff::Mesh& scene_mesh, size_t num_bone_weights) const;
	bool loadSkeleton(ModelData* data, const Gaff::JSON& model_prefs, const HoldingData& holding_data, unsigned int& num_bone_weights, Array< Array<VertSkeletonData> >& vert_skeleton_data);

	bool finishLoadingResource(const char* file_name, ModelData* model_data, const Gaff::JSON& json, const HoldingData& holding_data);

	GAFF_NO_COPY(ModelLoader);
	GAFF_NO_MOVE(ModelLoader);

	friend class ModelLoaderFunctor;
};

NS_END