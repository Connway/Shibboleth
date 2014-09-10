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

#pragma once

#include "Shibboleth_IResourceLoader.h"
#include <Gaff_Defines.h>

NS_GAFF
	class JSON;
	class Mesh;
NS_END

namespace Gleam {
	class IRenderDevice;
	class IShader;
	class IModel;
}

NS_SHIBBOLETH

struct ModelData;
class RenderManager;

class ModelLoader : public IResourceLoader
{
public:
	ModelLoader(RenderManager& render_mgr);
	~ModelLoader(void);

	Gaff::IVirtualDestructor* load(const char* file_name, unsigned long long user_data);

private:
	RenderManager& _render_mgr;

	bool loadMeshes(ModelData* data, const Gaff::JSON& lod_tags, const Gaff::JSON& model_prefs);
	bool createMeshAndLayout(Gleam::IRenderDevice& rd, const Gaff::Mesh& scene_mesh, Gleam::IModel* model, const Gaff::JSON& model_prefs);
	unsigned int generateLoadingFlags(const Gaff::JSON& model_prefs);
	Gleam::IShader* generateEmptyD3D11Shader(Gleam::IRenderDevice& rd, const Gaff::JSON& model_prefs, const Gaff::Mesh& scene_mesh) const;

	GAFF_NO_COPY(ModelLoader);
	GAFF_NO_MOVE(ModelLoader);
};

NS_END
