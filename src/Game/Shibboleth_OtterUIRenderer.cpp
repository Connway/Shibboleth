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
#include "Shibboleth_RenderManager.h"
#include "Shibboleth_App.h"
#include <Gleam_IRenderDevice.h>
#include <Gleam_IRenderState.h>
#include <Gleam_ITexture.h>
#include <Gleam_IBuffer.h>
#include <Gleam_IMesh.h>

#define MAX_VERTICES 9000

NS_SHIBBOLETH

static Gleam::IMesh::TOPOLOGY_TYPE otter_topology_map[3] = {
	Gleam::IMesh::TRIANGLE_LIST,
	Gleam::IMesh::TRIANGLE_STRIP,
	Gleam::IMesh::TOPOLOGY_SIZE
};

OtterUIRenderer::OtterUIRenderer(App& app):
	_resource_manager(app.getManager<ResourceManager>("Resource Manager")), _app(app),
	_render_device(nullptr), _rd_spinlock(nullptr)
{
}

OtterUIRenderer::~OtterUIRenderer(void)
{
}

bool OtterUIRenderer::init(void)
{
	RenderManager& render_manager = _app.getManager<RenderManager>("Render Manager");
	Gleam::IRenderDevice& rd = render_manager.getRenderDevice();
	_rd_spinlock = &render_manager.getSpinLock();
	_render_device = &rd;

	_rd_spinlock->lock();

	_device_data.resize(rd.getNumDevices());

	for (unsigned int i = 0; i < rd.getNumDevices(); ++i) {
		_device_data[i].output_data.resize(rd.getNumOutputs(i));
		rd.setCurrentDevice(i);

		_device_data[i].render_state[0] = RenderStatePtr(render_manager.createRenderState());
		_device_data[i].render_state[1] = RenderStatePtr(render_manager.createRenderState());
		_device_data[i].render_state[2] = RenderStatePtr(render_manager.createRenderState());
		_device_data[i].vertex_buffer = BufferPtr(render_manager.createBuffer());
		_device_data[i].mesh = MeshPtr(render_manager.createMesh());

		if (!_device_data[i].render_state[0] || !_device_data[i].render_state[1] || !_device_data[i].render_state[2]) {
			// log error
			return false;
		}

		if (!_device_data[i].vertex_buffer) {
			// log error
			return false;
		}

		if (!_device_data[i].mesh) {
			// log error
			return false;
		}

		//DRAW_TO_STENCIL
		Gleam::IRenderState::StencilData front, back;
		front.comp_func = Gleam::IRenderState::COMPARE_ALWAYS;
		front.stencil_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		front.stencil_depth_pass = Gleam::IRenderState::STENCIL_REPLACE;
		front.stencil_pass_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		back = front;

		// add alpha blending
		Gleam::IRenderState::BlendData bd;
		bd.enable_alpha_blending = false;
		bd.blend_dst_alpha = Gleam::IRenderState::BLEND_FACTOR_DEST_ALPHA;
		bd.blend_dst_color = Gleam::IRenderState::BLEND_FACTOR_DEST_COLOR;
		bd.blend_op_alpha = Gleam::IRenderState::BLEND_OP_ADD;
		bd.blend_src_alpha = Gleam::IRenderState::BLEND_FACTOR_SRC_ALPHA;
		bd.blend_src_color = Gleam::IRenderState::BLEND_FACTOR_SRC_COLOR;
		bd.color_write_mask = 0;

		if (!_device_data[i].render_state[DRAW_TO_STENCIL]->init(
				*_render_device, false, false, true, Gleam::IRenderState::COMPARE_ALWAYS,
				front, back, 1, 0xFF, 0xFF, Gleam::IRenderState::CULL_BACK, false, bd
			)) {

			// log error
			return false;
		}

		//DRAW_USING_STENCIL
		front.comp_func = Gleam::IRenderState::COMPARE_NOT_EQUAL;
		front.stencil_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		front.stencil_depth_pass = Gleam::IRenderState::STENCIL_KEEP;
		front.stencil_pass_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		back = front;

		bd.enable_alpha_blending = false;
		bd.blend_dst_alpha = Gleam::IRenderState::BLEND_FACTOR_DEST_ALPHA;
		bd.blend_dst_color = Gleam::IRenderState::BLEND_FACTOR_DEST_COLOR;
		bd.blend_op_alpha = Gleam::IRenderState::BLEND_OP_ADD;
		bd.blend_src_alpha = Gleam::IRenderState::BLEND_FACTOR_SRC_ALPHA;
		bd.blend_src_color = Gleam::IRenderState::BLEND_FACTOR_SRC_COLOR;
		bd.color_write_mask = Gleam::IRenderState::COLOR_ALL;

		if (!_device_data[i].render_state[DRAW_USING_STENCIL]->init(
			*_render_device, false, false, true, Gleam::IRenderState::COMPARE_ALWAYS,
			front, back, 0, 0, 0, Gleam::IRenderState::CULL_BACK, false, bd
			)) {

			// log error
			return false;
		}

		//DISABLE_STENCIL
		front.comp_func = Gleam::IRenderState::COMPARE_NEVER;
		front.stencil_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		front.stencil_depth_pass = Gleam::IRenderState::STENCIL_KEEP;
		front.stencil_pass_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		back = front;

		// add alpha blending
		bd.enable_alpha_blending = false;
		bd.blend_dst_alpha = Gleam::IRenderState::BLEND_FACTOR_DEST_ALPHA;
		bd.blend_dst_color = Gleam::IRenderState::BLEND_FACTOR_DEST_COLOR;
		bd.blend_op_alpha = Gleam::IRenderState::BLEND_OP_ADD;
		bd.blend_src_alpha = Gleam::IRenderState::BLEND_FACTOR_SRC_ALPHA;
		bd.blend_src_color = Gleam::IRenderState::BLEND_FACTOR_SRC_COLOR;
		bd.color_write_mask = Gleam::IRenderState::COLOR_ALL;

		if (!_device_data[i].render_state[DISABLE_STENCIL]->init(
			*_render_device, false, false, false, Gleam::IRenderState::COMPARE_ALWAYS,
			front, back, 0, 0, 0, Gleam::IRenderState::CULL_BACK, false, bd
			)) {

			// log error
			return false;
		}

		if (!_device_data[i].vertex_buffer->init(rd, nullptr, sizeof(Otter::GUIVertex) * MAX_VERTICES, Gleam::IBuffer::VERTEX_DATA, 0, Gleam::IBuffer::WRITE)) {
			// log error
			return false;
		}

		_device_data[i].mesh->addBuffer(_device_data[i].vertex_buffer.get());

		for (unsigned int j = 0; j < rd.getNumOutputs(i); ++j) {
			rd.setCurrentOutput(j);
		}
	}

	_rd_spinlock->unlock();

	// request shader

	return true;
}

void OtterUIRenderer::OnLoadTexture(sint32 textureID, const char* szPath)
{
	assert(!_resource_map[textureID]);

	ResourcePtr resource = _resource_manager.requestResource(szPath, TEX_LOADER_NORMALIZED);

	while (!resource->isLoaded()) {
		// Block until loaded
	}

	_resource_map[textureID] = resource;
}

void OtterUIRenderer::OnUnloadTexture(sint32 textureID)
{
	_resource_map[textureID] = nullptr;
}

void OtterUIRenderer::OnDrawBegin()
{
	// Calls prior to this will set up the device and output

	// set up shader, buffers, etc.

	// Make sure no one tries to use the render device while we're drawing
	_rd_spinlock->lock();
}

void OtterUIRenderer::OnDrawEnd()
{
	_rd_spinlock->unlock();
}

void OtterUIRenderer::OnCommitVertexBuffer(const Otter::GUIVertex* pVertices, uint32 numVertices)
{
	assert(_render_device && _rd_spinlock);

	// essentially just copy the vert buffer over to our buffer
	DeviceData& device_data = _device_data[_render_device->getCurrentDevice()];
	device_data.vertex_buffer->update(*_render_device, pVertices, numVertices);
}

void OtterUIRenderer::OnDrawBatch(const Otter::DrawBatch& batch)
{
	if (batch.mPrimitiveType == Otter::kPrim_TriangleFan) {
		// not supported, report error
		return;
	}

	// set texture
	// set model to world transform
	// set topology
	// render

	// Example D3D9 code
	//numBatches++;
	//mModel = *(D3DXMATRIX*)&batch.mTransform;

	//LPDIRECT3DTEXTURE9 texture = mTextures[batch.mTextureID];
	//mEffect->SetTechnique(texture != NULL ? "WithTexture" : "WithoutTexture");
	//mEffect->SetTexture("DIFFUSE_TEXTURE", texture);
	//mEffect->SetMatrix("MATRIX_MODEL", &mModel);

	//uint32 passes = 0;
	//mEffect->Begin(&passes, D3DXFX_DONOTSAVESAMPLERSTATE);
	//{
	//	for (uint32 pass = 0; pass < passes; pass++)
	//	{
	//		mEffect->BeginPass(pass);

	//		D3DPRIMITIVETYPE primType = D3DPT_TRIANGLELIST;
	//		switch (batch.mPrimitiveType)
	//		{
	//		case Otter::kPrim_TriangleFan:
	//		{
	//			primType = D3DPT_TRIANGLEFAN;
	//			break;
	//		}
	//		case Otter::kPrim_TriangleStrip:
	//		{
	//			primType = D3DPT_TRIANGLESTRIP;
	//			break;
	//		}
	//		}

	//		mD3DDevice->DrawPrimitive(primType, batch.mVertexStartIndex, batch.mPrimitiveCount);

	//		mEffect->EndPass();
	//	}
	//}
	//mEffect->End();
}

void OtterUIRenderer::SetStencilState(StencilState state)
{
	_stencil_state = state;

	// Example D3D9 code
	//switch (state) {
	//	case DRAW_TO_STENCIL:
	//		//clear the stencil of any other masks that have been previously rendered
	//		mD3DDevice->Clear(0, NULL, D3DCLEAR_STENCIL, 0, 0, 0);

	//		//enable stencil writing, disable color writing
	//		mD3DDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);
	//		mD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

	//		//set the stencil function to always write 1 to the stencil buffer
	//		mD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	//		mD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
	//		mD3DDevice->SetRenderState(D3DRS_STENCILREF, 1);
	//		break;
	//	case DRAW_USING_STENCIL:
	//		//re-enable color writing
	//		mD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

	//		//set the stencil function to only draw to pixels with 1 in the stencil buffer
	//		mD3DDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
	//		mD3DDevice->SetRenderState(D3DRS_STENCILREF, 0);
	//		mD3DDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
	//		break;
	//	case DISABLE_STENCIL:
	//		//disable stencil writing
	//		mD3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	//		mD3DDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	//		break;
	//}
}

NS_END
