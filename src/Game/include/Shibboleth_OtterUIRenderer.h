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

#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_Map.h"
#include <Interfaces/IRenderer.h>

NS_SHIBBOLETH

class RenderManager;
class App;

class OtterUIRenderer : public Otter::IRenderer
{
public:
	OtterUIRenderer(App& app);
	~OtterUIRenderer(void);

	bool init(void);

	// resize
	// get output texture srv

	void setShaderProgram(ProgramPtr& program);

	void OnLoadTexture(sint32 textureID, const char* szPath);
	void OnUnloadTexture(sint32 textureID);

	void OnDrawBegin();
	void OnDrawEnd();

	void OnCommitVertexBuffer(const Otter::GUIVertex* pVertices, uint32 numVertices);
	void OnDrawBatch(const Otter::DrawBatch& batch);

	void SetStencilState(StencilState state);

private:
	struct OutputData
	{
		RenderTargetPtr render_target;
		TexturePtr output_texture;
		TexturePtr stencil_buffer;
	};

	struct DeviceData
	{
		Array<OutputData> output_data;
		RenderStatePtr render_state[3];
		SamplerStatePtr sampler;
		BufferPtr constant_buffer;
		BufferPtr vertex_buffer;
		MeshPtr mesh;
	};

	struct ResourceData
	{
		Array<ShaderResourceViewPtr> resource_views;
		ResourcePtr resource;
	};

	Map<int, ResourceData> _resource_map;
	Array<DeviceData> _device_data;

	ResourceManager& _resource_manager;
	RenderManager& _render_manager;
	App& _app;

	Gleam::IRenderDevice* _render_device;
	Gaff::SpinLock* _rd_spinlock;

	ProgramPtr _program;

	Otter::IRenderer::StencilState _stencil_state;

	GAFF_NO_COPY(OtterUIRenderer);
};

NS_END
