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
#include "Shibboleth_Map.h"
#include <Interfaces/IRenderer.h>

NS_SHIBBOLETH

class IRenderManager;
class IApp;

class OtterUIRenderer : public Otter::IRenderer
{
public:
	OtterUIRenderer(IApp& app);
	~OtterUIRenderer(void);

	bool init(const char* default_shader);

	// resize
	// get output texture srv

	void setShaderProgram(const ResWrap<ProgramData>& programs);

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
		//RenderStatePtr render_state[3];
		SamplerStatePtr sampler;
		BufferPtr constant_buffer;
		BufferPtr vertex_buffer;
		LayoutPtr layout;
		MeshPtr mesh;
	};

	struct ResourceData
	{
		Array<ShaderResourceViewPtr> resource_views;
		ResWrap<TextureData> resource;
	};

	Map<int, ResourceData> _resource_map;
	Array<ProgramBuffersPtr> _program_buffers;
	Array<DeviceData> _device_data;

	IResourceManager& _resource_manager;
	IRenderManager& _render_manager;
	IApp& _app;

	Gleam::IRenderDevice& _render_device;

	ResWrap<ProgramData> _programs;

	Otter::IRenderer::StencilState _stencil_state;

	GAFF_NO_COPY(OtterUIRenderer);
};

NS_END