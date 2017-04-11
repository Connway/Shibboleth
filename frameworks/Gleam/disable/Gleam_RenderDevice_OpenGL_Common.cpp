/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gleam_RenderDevice_OpenGL.h"
#include "Gleam_ShaderResourceView_OpenGL.h"
#include "Gleam_DepthStencilState_OpenGL.h"
#include "Gleam_SamplerState_OpenGL.h"
#include "Gleam_RasterState_OpenGL.h"
#include "Gleam_CommandList_OpenGL.h"
#include "Gleam_BlendState_OpenGL.h"
#include "Gleam_Program_OpenGL.h"
#include "Gleam_Layout_OpenGL.h"
#include "Gleam_Buffer_OpenGL.h"
#include "Gleam_Mesh_OpenGL.h"
#include "Gleam_IMesh.h"

#include <GL/glew.h>

NS_GLEAM

typedef void(__stdcall *DisableEnable)(uint32_t);
//typedef void (__stdcall *DisableEnablei)(uint32_t, uint32_t);

static DisableEnable gDisable_Enable_Funcs[2] = { glDisable, glEnable };

bool RenderDeviceGL::CheckRequiredExtensions(void)
{
	return (GLEW_VERSION_4_3 && GLEW_EXT_polygon_offset_clamp) ||
		(
			GLEW_ARB_compute_shader && GLEW_ARB_geometry_shader4 &&
			GLEW_ARB_tessellation_shader && GLEW_ARB_explicit_uniform_location &&
			GLEW_ARB_sampler_objects && GLEW_ARB_separate_shader_objects &&
			GLEW_EXT_texture_filter_anisotropic && GLEW_ARB_shading_language_420pack &&
			GLEW_ARB_vertex_array_object
			);
}

RenderDeviceGL::RenderDeviceGL(void)
{
}

RenderDeviceGL::~RenderDeviceGL()
{
}

void RenderDeviceGL::setClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void RenderDeviceGL::resetRenderState(void)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glDisable(GL_BLEND);
	glClearDepth(1.0f);
	glClearStencil(0);
}

bool RenderDeviceGL::isDeferred(void) const
{
	return false;
}

RendererType RenderDeviceGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

IRenderDevice* RenderDeviceGL::createDeferredRenderDevice(void)
{
	return nullptr;
}

void RenderDeviceGL::executeCommandList(ICommandList* command_list)
{
	GAFF_ASSERT(command_list->getRendererType() == RENDERER_OPENGL);
	resetRenderState();
	reinterpret_cast<CommandListGL*>(command_list)->execute();
}

bool RenderDeviceGL::finishCommandList(ICommandList*)
{
	GAFF_ASSERT_MSG(false, "Calling a deferred render device function on an immediate render device");
	return false;
}

void RenderDeviceGL::setDepthStencilState(const DepthStencilStateGL* ds_state)
{
	if (ds_state) {
		const IDepthStencilState::DepthStencilStateSettings& ds_settings = ds_state->getDepthStencilSettings();

		gDisable_Enable_Funcs[ds_settings.depth_test](GL_DEPTH_TEST);
		gDisable_Enable_Funcs[ds_settings.stencil_test](GL_STENCIL_TEST);

		glDepthFunc(DepthStencilStateGL::Compare_Funcs[ds_settings.depth_func - 1]);

		glStencilOpSeparate(
			GL_FRONT, DepthStencilStateGL::Stencil_Ops[ds_settings.front_face.stencil_depth_fail - 1],
			DepthStencilStateGL::Stencil_Ops[ds_settings.front_face.stencil_pass_depth_fail - 1],
			DepthStencilStateGL::Stencil_Ops[ds_settings.front_face.stencil_depth_pass - 1]
		);

		glStencilOpSeparate(
			GL_BACK, DepthStencilStateGL::Stencil_Ops[ds_settings.back_face.stencil_depth_fail - 1],
			DepthStencilStateGL::Stencil_Ops[ds_settings.back_face.stencil_pass_depth_fail - 1],
			DepthStencilStateGL::Stencil_Ops[ds_settings.back_face.stencil_depth_pass - 1]
		);

		glStencilFuncSeparate(
			GL_FRONT, DepthStencilStateGL::Compare_Funcs[ds_settings.front_face.comp_func - 1],
			ds_settings.stencil_ref, ds_settings.stencil_write_mask
		);

		glStencilFuncSeparate(
			GL_BACK, DepthStencilStateGL::Compare_Funcs[ds_settings.back_face.comp_func - 1],
			ds_settings.stencil_ref, ds_settings.stencil_write_mask
		);

		glStencilMask(ds_settings.stencil_write_mask);

	} else {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
	}
}

void RenderDeviceGL::setRasterState(const RasterStateGL* raster_state)
{
	if (raster_state) {
		const IRasterState::RasterStateSettings& raster_settings = raster_state->getRasterSettings();

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffsetClampEXT(static_cast<GLfloat>(raster_settings.depth_bias), raster_settings.slope_scale_depth_bias, raster_settings.depth_bias_clamp);

		glPolygonMode(GL_FRONT_AND_BACK, (raster_settings.wireframe) ? GL_LINE : GL_FILL);

		gDisable_Enable_Funcs[!raster_settings.two_sided](GL_CULL_FACE);
		glCullFace(GL_BACK);

		glFrontFace((raster_settings.front_face_counter_clockwise) ? GL_CCW : GL_CW);

		gDisable_Enable_Funcs[raster_settings.scissor_enabled](GL_SCISSOR_TEST);

	} else {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		glDisable(GL_SCISSOR_TEST);
	}
}

void RenderDeviceGL::setBlendState(const BlendStateGL* blend_state)
{
	const IBlendState::BlendStateSettings* blend_settings = (blend_state) ? blend_state->getBlendSettings() : nullptr;

	for (int32_t i = 0; i < 8; ++i) {
		if (blend_settings && blend_settings[i].enable_alpha_blending) {
			glEnablei(GL_BLEND, i);

			glBlendFuncSeparatei(
				i,
				BlendStateGL::Blend_Factors[blend_settings[i].blend_src_color - 1], BlendStateGL::Blend_Factors[blend_settings[i].blend_dst_color - 1],
				BlendStateGL::Blend_Factors[blend_settings[i].blend_src_alpha - 1], BlendStateGL::Blend_Factors[blend_settings[i].blend_dst_alpha - 1]
			);

			glBlendEquationSeparatei(i, BlendStateGL::Blend_Ops[blend_settings[i].blend_op_color - 1], BlendStateGL::Blend_Ops[blend_settings[i].blend_op_alpha - 1]);

			glColorMaski(
				i,
				(blend_settings[i].color_write_mask & IBlendState::COLOR_RED) > 0,
				(blend_settings[i].color_write_mask & IBlendState::COLOR_GREEN) > 0,
				(blend_settings[i].color_write_mask & IBlendState::COLOR_BLUE) > 0,
				(blend_settings[i].color_write_mask & IBlendState::COLOR_ALPHA) > 0
			);

		} else {
			glColorMaski(i, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glDisablei(GL_BLEND, i);
		}
	}
}

void RenderDeviceGL::setLayout(LayoutGL* layout, const IMesh* mesh)
{
	const Vector< Vector<LayoutGL::LayoutData> >& layout_descs = layout->GetLayoutDescriptors();
	const LayoutGL::LayoutData* layout_data = nullptr;
	const BufferGL* buffer = nullptr;
	int32_t stride = 0;
	uint32_t count = 0;

	for (int32_t i = 0; i < static_cast<int32_t>(layout_descs.size()); ++i) {
		const Vector<LayoutGL::LayoutData>& ld = layout_descs[i];
		buffer = reinterpret_cast<const BufferGL*>(mesh->getBuffer(i));
		stride = buffer->getStride();

		glBindBuffer(GL_ARRAY_BUFFER, buffer->getBuffer());

		for (int32_t j = 0; j < static_cast<int32_t>(ld.size()); ++j) {
			layout_data = &ld[j];

			glEnableVertexAttribArray(count);

			glVertexAttribPointer(count, layout_data->size, layout_data->type, layout_data->normalized,
				stride, reinterpret_cast<void*>(static_cast<uintptr_t>(layout_data->aligned_byte_offset)));

			++count;
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void RenderDeviceGL::unsetLayout(LayoutGL* layout)
{
	const Vector< Vector<LayoutGL::LayoutData> >& layout_descs = layout->GetLayoutDescriptors();
	uint32_t count = 0;

	for (int32_t i = 0; i < static_cast<int32_t>(layout_descs.size()); ++i) {
		for (int32_t j = 0; j < static_cast<int32_t>(layout_descs[i].size()); ++j) {
			glDisableVertexAttribArray(count);
			++count;
		}
	}
}

void RenderDeviceGL::bindShader(ProgramGL* shader)
{
	glBindProgramPipeline(shader->getProgram());
}

void RenderDeviceGL::unbindShader(void)
{
	glBindProgramPipeline(0);
}

void RenderDeviceGL::bindProgramBuffers(ProgramBuffersGL* program_buffers)
{
	uint32_t texture_count = 0;
	uint32_t count = 0;

	for (int32_t i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		const Vector<IShaderResourceView*>& resource_views = program_buffers->getResourceViews(static_cast<Gleam::IShader::SHADER_TYPE>(i));
		const Vector<ISamplerState*>& sampler_states = program_buffers->getSamplerStates(static_cast<Gleam::IShader::SHADER_TYPE>(i));
		const Vector<IBuffer*>& const_bufs = program_buffers->getConstantBuffers(static_cast<Gleam::IShader::SHADER_TYPE>(i));

		GAFF_ASSERT(sampler_states.size() <= resource_views.size());
		int32_t sampler_count = 0;

		for (int32_t j = 0; j < static_cast<int32_t>(const_bufs.size()); ++j) {
			glBindBufferBase(GL_UNIFORM_BUFFER, count, reinterpret_cast<const BufferGL*>(const_bufs[j])->getBuffer());
			++count;
		}

		for (int32_t j = 0; j < static_cast<int32_t>(resource_views.size()); ++j) {
			ShaderResourceViewGL* rv = reinterpret_cast<ShaderResourceViewGL*>(resource_views[j]);

			if (resource_views[j]->getViewType() == IShaderResourceView::VIEW_TEXTURE) {
				// should probably assert that texture_count isn't higher than the supported number of textures
				GAFF_ASSERT(texture_count < 32);

				SamplerStateGL* st = reinterpret_cast<SamplerStateGL*>(sampler_states[sampler_count]);

				glActiveTexture(GL_TEXTURE0 + texture_count);
				glBindTexture(rv->getTarget(), rv->getResourceView());
				glBindSampler(texture_count, st->getSamplerState());

				++texture_count;
				++sampler_count;

			} else {
				GAFF_ASSERT_MSG(false, "How is your ShaderResourceView not a texture? That's the only type we have implemented ...");
			}
		}
	}
}

void RenderDeviceGL::renderMeshNonIndexed(uint32_t topology, int32_t vert_count, int32_t start_location)
{
	glDrawArrays(topology, start_location, vert_count);
}

void RenderDeviceGL::renderMeshInstanced(MeshGL* mesh, int32_t count)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<BufferGL*>(mesh->getIndiceBuffer())->getBuffer());
	glDrawElementsInstanced(mesh->getGLTopology(), mesh->getIndexCount(), GL_UNSIGNED_INT, 0, count);
}

void RenderDeviceGL::renderMesh(MeshGL* mesh)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<BufferGL*>(mesh->getIndiceBuffer())->getBuffer());
	glDrawElements(mesh->getGLTopology(), mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
}

void RenderDeviceGL::renderNoVertexInput(int32_t /*vert_count*/)
{
	// TODO: IMPLEMENT ME!
}

NS_END
