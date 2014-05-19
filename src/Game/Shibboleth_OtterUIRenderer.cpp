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

#include "Shibboleth_OtterUIRenderer.h"
#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_TextureLoader.h"
#include "Shibboleth_App.h"
#include <Gleam_ITexture.h>

NS_SHIBBOLETH

OtterUIRenderer::OtterUIRenderer(App& app):
	_resource_manager(app.getManager<ResourceManager>("Resource Manager")), _app(app)
{
}

OtterUIRenderer::~OtterUIRenderer(void)
{
}

void OtterUIRenderer::OnLoadTexture(sint32 textureID, const char* szPath)
{
	assert(!_texture_map[textureID]);

	TextureLoader::TextureData texture_data;
	texture_data.device_id = 0;
	texture_data.output_id = 0;
	texture_data.normalized = true;
	texture_data.cubemap = false;

	ResourcePtr resource = _resource_manager.requestResource(szPath, *((unsigned long long*)&texture_data));

	while (!resource->isLoaded()) {
		// Block until loaded
	}
}

void OtterUIRenderer::OnUnloadTexture(sint32 textureID)
{
}

void OtterUIRenderer::OnDrawBegin()
{
}

void OtterUIRenderer::OnDrawEnd()
{
}

void OtterUIRenderer::OnCommitVertexBuffer(const Otter::GUIVertex* pVertices, uint32 numVertices)
{
}

void OtterUIRenderer::OnDrawBatch(const Otter::DrawBatch& batch)
{
}

void OtterUIRenderer::SetStencilState(StencilState /*state*/)
{
}

NS_END
