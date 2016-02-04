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

#include "Shibboleth_OtterUIRenderer.h"
#include "Shibboleth_RenderManager.h"
#include "Shibboleth_IApp.h"
#include <Gleam_IShaderResourceView.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_IRenderDevice.h>
//#include <Gleam_IRenderState.h>
#include <Gleam_IProgram.h>
//#include <Gleam_ITexture.h>
#include <Gleam_ILayout.h>
#include <Gleam_IBuffer.h>
#include <Gleam_IMesh.h>

#define MAX_VERTICES 9000

NS_SHIBBOLETH

static Gleam::IMesh::TOPOLOGY_TYPE otter_topology_map[3] = {
	Gleam::IMesh::TRIANGLE_LIST,
	Gleam::IMesh::TRIANGLE_STRIP,
	Gleam::IMesh::TOPOLOGY_SIZE
};

OtterUIRenderer::OtterUIRenderer(IApp& app):
	_resource_manager(app.getManagerT<ResourceManager>("Resource Manager")),
	_render_manager(app.getManagerT<RenderManager>("Render Manager")),
	_app(app), _render_device(app.getManagerT<RenderManager>("Render Manager").getRenderDevice())
{
}

OtterUIRenderer::~OtterUIRenderer(void)
{
}

bool OtterUIRenderer::init(const char* default_shader)
{
	assert(default_shader && strlen(default_shader));

	// request default shader program
	_programs = _resource_manager.requestResource(default_shader);

	while (!_programs.getResourcePtr()->isLoaded()) {
		// Block until loaded
	}

	_program_buffers.resize(_render_device.getNumDevices());
	_device_data.resize(_render_device.getNumDevices());

	for (unsigned int i = 0; i < _render_device.getNumDevices(); ++i) {
		_device_data[i].output_data.resize(_render_device.getNumOutputs(i));
		_render_device.setCurrentDevice(i);

		_program_buffers[i] = ProgramBuffersPtr(_render_manager.createProgramBuffers());

		//_device_data[i].render_state[0] = RenderStatePtr(_render_manager.createRenderState());
		//_device_data[i].render_state[1] = RenderStatePtr(_render_manager.createRenderState());
		//_device_data[i].render_state[2] = RenderStatePtr(_render_manager.createRenderState());
		_device_data[i].sampler = SamplerStatePtr(_render_manager.createSamplerState());
		_device_data[i].constant_buffer = BufferPtr(_render_manager.createBuffer());
		_device_data[i].vertex_buffer = BufferPtr(_render_manager.createBuffer());
		_device_data[i].layout = LayoutPtr(_render_manager.createLayout());
		_device_data[i].mesh = MeshPtr(_render_manager.createMesh());

		if (!_program_buffers[i]) {
			// log error
			return false;
		}

		//if (!_device_data[i].render_state[0] || !_device_data[i].render_state[1] || !_device_data[i].render_state[2]) {
		//	// log error
		//	return false;
		//}

		if (!_device_data[i].sampler) {
			// log error
			return false;
		}

		if (!_device_data[i].constant_buffer) {
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

		////DRAW_TO_STENCIL
		//Gleam::IRenderState::StencilData front, back;
		//front.comp_func = Gleam::IRenderState::COMPARE_ALWAYS;
		//front.stencil_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		//front.stencil_depth_pass = Gleam::IRenderState::STENCIL_REPLACE;
		//front.stencil_pass_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		//back = front;

		//// add alpha blending
		//Gleam::IRenderState::BlendData bd;
		//bd.enable_alpha_blending = false;
		//bd.blend_dst_alpha = Gleam::IRenderState::BLEND_FACTOR_DEST_ALPHA;
		//bd.blend_dst_color = Gleam::IRenderState::BLEND_FACTOR_DEST_COLOR;
		//bd.blend_op_alpha = Gleam::IRenderState::BLEND_OP_ADD;
		//bd.blend_src_alpha = Gleam::IRenderState::BLEND_FACTOR_SRC_ALPHA;
		//bd.blend_src_color = Gleam::IRenderState::BLEND_FACTOR_SRC_COLOR;
		//bd.color_write_mask = 0;

		//if (!_device_data[i].render_state[DRAW_TO_STENCIL]->init(
		//		_render_device, false, false, true, Gleam::IRenderState::COMPARE_ALWAYS,
		//		front, back, 1, 0xFF, 0xFF, Gleam::IRenderState::CULL_BACK, false, bd
		//	)) {

		//	// log error
		//	return false;
		//}

		////DRAW_USING_STENCIL
		//front.comp_func = Gleam::IRenderState::COMPARE_NOT_EQUAL;
		//front.stencil_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		//front.stencil_depth_pass = Gleam::IRenderState::STENCIL_KEEP;
		//front.stencil_pass_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		//back = front;

		//bd.enable_alpha_blending = false;
		//bd.blend_dst_alpha = Gleam::IRenderState::BLEND_FACTOR_DEST_ALPHA;
		//bd.blend_dst_color = Gleam::IRenderState::BLEND_FACTOR_DEST_COLOR;
		//bd.blend_op_alpha = Gleam::IRenderState::BLEND_OP_ADD;
		//bd.blend_src_alpha = Gleam::IRenderState::BLEND_FACTOR_SRC_ALPHA;
		//bd.blend_src_color = Gleam::IRenderState::BLEND_FACTOR_SRC_COLOR;
		//bd.color_write_mask = Gleam::IRenderState::COLOR_ALL;

		//if (!_device_data[i].render_state[DRAW_USING_STENCIL]->init(
		//	_render_device, false, false, true, Gleam::IRenderState::COMPARE_ALWAYS,
		//	front, back, 0, 0, 0, Gleam::IRenderState::CULL_BACK, false, bd
		//	)) {

		//	// log error
		//	return false;
		//}

		////DISABLE_STENCIL
		//front.comp_func = Gleam::IRenderState::COMPARE_NEVER;
		//front.stencil_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		//front.stencil_depth_pass = Gleam::IRenderState::STENCIL_KEEP;
		//front.stencil_pass_depth_fail = Gleam::IRenderState::STENCIL_KEEP;
		//back = front;

		//// add alpha blending
		//bd.enable_alpha_blending = false;
		//bd.blend_dst_alpha = Gleam::IRenderState::BLEND_FACTOR_DEST_ALPHA;
		//bd.blend_dst_color = Gleam::IRenderState::BLEND_FACTOR_DEST_COLOR;
		//bd.blend_op_alpha = Gleam::IRenderState::BLEND_OP_ADD;
		//bd.blend_src_alpha = Gleam::IRenderState::BLEND_FACTOR_SRC_ALPHA;
		//bd.blend_src_color = Gleam::IRenderState::BLEND_FACTOR_SRC_COLOR;
		//bd.color_write_mask = Gleam::IRenderState::COLOR_ALL;

		//if (!_device_data[i].render_state[DISABLE_STENCIL]->init(
		//	_render_device, false, false, false, Gleam::IRenderState::COMPARE_ALWAYS,
		//	front, back, 0, 0, 0, Gleam::IRenderState::CULL_BACK, false, bd
		//	)) {

		//	// log error
		//	return false;
		//}

		if (!_device_data[i].vertex_buffer->init(_render_device, nullptr, sizeof(Otter::GUIVertex) * MAX_VERTICES, Gleam::IBuffer::VERTEX_DATA, 0, Gleam::IBuffer::WRITE)) {
			// log error
			return false;
		}

		_device_data[i].mesh->addBuffer(_device_data[i].vertex_buffer.get());

		if (!_device_data[i].sampler->init(_render_device, Gleam::ISamplerState::FILTER_ANISOTROPIC,
			Gleam::ISamplerState::WRAP_REPEAT, Gleam::ISamplerState::WRAP_REPEAT,
			Gleam::ISamplerState::WRAP_REPEAT, 0, 0, 0, 16, 0, 0, 0, 0)) {

			// log error
			return false;
		}

		if (!_device_data[i].constant_buffer->init(_render_device, nullptr, sizeof(float) * 16, Gleam::IBuffer::SHADER_DATA, 0, Gleam::IBuffer::WRITE)) {
			// log error
			return false;
		}

		_program_buffers[i]->addConstantBuffer(Gleam::IShader::SHADER_VERTEX, _device_data[i].constant_buffer.get());
		_program_buffers[i]->addSamplerState(Gleam::IShader::SHADER_PIXEL, _device_data[i].sampler.get());

		// use default shader program to create layout
		//_device_data[i].layout->init(_render_device, layout_desc, sizeof(layout_desc), _programs->programs[i]->getAttachedShader(Gleam::IShader::SHADER_VERTEX));

		for (unsigned int j = 0; j < _render_device.getNumOutputs(i); ++j) {
			_render_device.setCurrentOutput(j);

			// make output texture
			// make output stencil buffer
			// make output render target
		}
	}

	return true;
}

void OtterUIRenderer::setShaderProgram(const ResWrap<ProgramData>& programs)
{
	_programs = programs;
}

void OtterUIRenderer::OnLoadTexture(sint32 textureID, const char* szPath)
{
	assert(!_resource_map.hasElementWithKey(textureID));

	// Apply some fixup to the file extension. Make it read *.texture instead.
	ResWrap<TextureData> tex_res = _resource_manager.requestResource(szPath);

	while (!tex_res.getResourcePtr()->isLoaded()) {
		// Block until loaded
	}

	//ResourceData res_data = { Array<ShaderResourceViewPtr>(), resource };
	ResourceData res_data = { Array<ShaderResourceViewPtr>(), tex_res };

	res_data.resource_views.resize(_render_device.getNumDevices());

	for (unsigned int i = 0; i < _render_device.getNumDevices(); ++i) {
		Gleam::IShaderResourceView* rsv = _render_manager.createShaderResourceView();

		if (!rsv) {
			// log error
			return;
		}

		if (!rsv->init(_render_device, tex_res->textures[i].get())) {
			// log error
			return;
		}

		res_data.resource_views[i] = rsv;
	}

	_resource_map[textureID] = res_data;
}

void OtterUIRenderer::OnUnloadTexture(sint32 textureID)
{
	_resource_map.erase(textureID);
}

void OtterUIRenderer::OnDrawBegin()
{
	// Calls prior to this will set up the device and output

	// set up shader, buffers, etc.
	//DeviceData& device_data = _device_data[_render_device.getCurrentDevice()];
}

void OtterUIRenderer::OnDrawEnd()
{
}

void OtterUIRenderer::OnCommitVertexBuffer(const Otter::GUIVertex* pVertices, uint32 numVertices)
{
	// copy the vert buffer over to our buffer
	DeviceData& device_data = _device_data[_render_device.getCurrentDevice()];
	device_data.vertex_buffer->update(_render_device, pVertices, numVertices);
}

void OtterUIRenderer::OnDrawBatch(const Otter::DrawBatch& batch)
{
	assert(_programs);

	if (batch.mPrimitiveType == Otter::kPrim_TriangleFan) {
		// not supported, report error
		return;
	}

	unsigned int curr_device = _render_device.getCurrentDevice();

	Gleam::IMesh::TOPOLOGY_TYPE topology = otter_topology_map[batch.mPrimitiveType];
	ProgramBuffersPtr& program_buffers = _program_buffers[curr_device];
	ResourceData& res_data = _resource_map[batch.mTextureID];
	ProgramPtr& program = _programs->programs[curr_device];
	assert(res_data.resource && !res_data.resource_views.empty());

	DeviceData& device_data = _device_data[curr_device];
	OutputData& output_data = device_data.output_data[_render_device.getCurrentOutput()];

	// update model-to-world transform
	device_data.constant_buffer->update(_render_device, batch.mTransform.mEntry, sizeof(batch.mTransform.mEntry));

	// add appropriate textures
	program_buffers->addResourceView(Gleam::IShader::SHADER_PIXEL, res_data.resource_views[curr_device].get());
	program->bind(_render_device);
	program_buffers->bind(_render_device);

	// bind layout
	device_data.layout->setLayout(_render_device, device_data.mesh.get());

	output_data.render_target->bind(_render_device);

	device_data.mesh->setTopologyType(topology);
	device_data.mesh->renderNonIndexed(_render_device, batch.mVertexCount);

	output_data.render_target->unbind(_render_device);
	device_data.layout->unsetLayout(_render_device);
	program->unbind(_render_device);

	program_buffers->popResourceView(Gleam::IShader::SHADER_PIXEL);

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
