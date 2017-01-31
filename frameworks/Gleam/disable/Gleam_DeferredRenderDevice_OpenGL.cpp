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

#include "Gleam_DeferredRenderDevice_OpenGL.h"
#include "Gleam_ShaderResourceView_OpenGL.h"
#include "Gleam_DepthStencilState_OpenGL.h"
#include "Gleam_SamplerState_OpenGL.h"
#include "Gleam_RasterState_OpenGL.h"
#include "Gleam_BlendState_OpenGL.h"
#include "Gleam_Program_OpenGL.h"
#include "Gleam_Layout_OpenGL.h"
#include "Gleam_Buffer_OpenGL.h"
#include "Gleam_Mesh_OpenGL.h"
#include <GL/glew.h>

NS_GLEAM

static Gaff::FunctionBinder<void, GLenum> gDisable_Enable_Funcs[2] = { Gaff::BindSTDCall(glDisable), Gaff::BindSTDCall(glEnable) };

DeferredRenderDeviceGL::DeferredRenderDeviceGL(void)
{
}

DeferredRenderDeviceGL::~DeferredRenderDeviceGL(void)
{
}

void DeferredRenderDeviceGL::destroy(void)
{
	_command_list.clear();
}

bool DeferredRenderDeviceGL::isDeferred(void) const
{
	return true;
}

RendererType DeferredRenderDeviceGL::getRendererType(void) const
{
	return RENDERER_OPENGL;
}

void DeferredRenderDeviceGL::executeCommandList(ICommandList* command_list)
{
	GAFF_ASSERT(command_list->getRendererType() == RENDERER_OPENGL);
	_command_list.append(reinterpret_cast<CommandListGL&>(*command_list));
}

bool DeferredRenderDeviceGL::finishCommandList(ICommandList* command_list)
{
	GAFF_ASSERT(command_list->getRendererType() == RENDERER_OPENGL);
	*command_list = std::move(_command_list);
	return true;
}

void DeferredRenderDeviceGL::renderNoVertexInput(unsigned int /*vert_count*/)
{
	// TODO: IMPLEMENT ME!
}

void DeferredRenderDeviceGL::setDepthStencilState(const DepthStencilStateGL* ds_state)
{
	if (ds_state) {
		static auto sfs_func = Gaff::BindSTDCall(glStencilFuncSeparate);
		static auto sos_func = Gaff::BindSTDCall(glStencilOpSeparate);
		static auto sm_func = Gaff::BindSTDCall(glStencilMask);
		static auto df_func = Gaff::BindSTDCall(glDepthFunc);

		const IDepthStencilState::DepthStencilStateSettings& ds_settings = ds_state->getDepthStencilSettings();

		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[ds_settings.depth_test], GL_DEPTH_TEST));
		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[ds_settings.stencil_test], GL_STENCIL_TEST));

		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(df_func, DepthStencilStateGL::Compare_Funcs[ds_settings.depth_func - 1]));

		Gaff::CachedFunction<void, GLenum, GLenum, GLint, GLuint> sfsf_cache(
			sfs_func, GL_FRONT,
			DepthStencilStateGL::Compare_Funcs[ds_settings.front_face.comp_func - 1],
			ds_settings.stencil_ref, ds_settings.stencil_write_mask
		);

		Gaff::CachedFunction<void, GLenum, GLenum, GLint, GLuint> sfsb_cache(
			sfs_func, GL_BACK,
			DepthStencilStateGL::Compare_Funcs[ds_settings.back_face.comp_func - 1],
			ds_settings.stencil_ref, ds_settings.stencil_write_mask
		);

		Gaff::CachedFunction<void, GLenum, GLenum, GLenum, GLenum> sosf_cache(
			sos_func, GL_FRONT,
			DepthStencilStateGL::Stencil_Ops[ds_settings.front_face.stencil_depth_fail - 1],
			DepthStencilStateGL::Stencil_Ops[ds_settings.front_face.stencil_pass_depth_fail - 1],
			DepthStencilStateGL::Stencil_Ops[ds_settings.front_face.stencil_depth_pass - 1]
		);

		Gaff::CachedFunction<void, GLenum, GLenum, GLenum, GLenum> sosb_cache(
			sos_func, GL_BACK,
			DepthStencilStateGL::Stencil_Ops[ds_settings.back_face.stencil_depth_fail - 1],
			DepthStencilStateGL::Stencil_Ops[ds_settings.back_face.stencil_pass_depth_fail - 1],
			DepthStencilStateGL::Stencil_Ops[ds_settings.back_face.stencil_depth_pass - 1]
		);

		Gaff::CachedFunction<void, GLenum> sm_cache(sm_func, ds_settings.stencil_write_mask);

		_command_list.addCommand(sosf_cache);
		_command_list.addCommand(sosb_cache);
		_command_list.addCommand(sfsf_cache);
		_command_list.addCommand(sfsb_cache);
		_command_list.addCommand(sm_cache);

	} else {
		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[0], GL_DEPTH_TEST));
		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[0], GL_STENCIL_TEST));
	}
}

void DeferredRenderDeviceGL::setRasterState(const RasterStateGL* raster_state)
{
	static auto poc_func = Gaff::BindSTDCall(glPolygonOffsetClampEXT);
	static auto pm_func = Gaff::BindSTDCall(glPolygonMode);
	static auto ff_func = Gaff::BindSTDCall(glFrontFace);
	static auto cf_func = Gaff::BindSTDCall(glCullFace);

	if (raster_state) {
		const IRasterState::RasterStateSettings& raster_settings = raster_state->getRasterSettings();

		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[1], GL_POLYGON_OFFSET_FILL));

		Gaff::CachedFunction<void, GLfloat, GLfloat, GLfloat> poc_cache(
			poc_func,
			static_cast<GLfloat>(raster_settings.depth_bias),
			raster_settings.slope_scale_depth_bias,
			raster_settings.depth_bias_clamp
		);

		_command_list.addCommand(poc_cache);

		Gaff::CachedFunction<void, GLenum, GLenum> pm_cache(pm_func, GL_FRONT_AND_BACK, (raster_settings.wireframe) ? GL_LINE : GL_FILL);
		_command_list.addCommand(pm_cache);

		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[!raster_settings.two_sided], GL_CULL_FACE));
		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(cf_func, GL_BACK));
		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(ff_func, (raster_settings.front_face_counter_clockwise) ? GL_CCW : GL_CW));

		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[raster_settings.scissor_enabled], GL_SCISSOR_TEST));

	} else {
		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[0], GL_POLYGON_OFFSET_FILL));

		Gaff::CachedFunction<void, GLenum, GLenum> pm_cache(pm_func, GL_FRONT_AND_BACK, GL_FILL);
		_command_list.addCommand(pm_cache);

		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[1], GL_CULL_FACE));
		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(cf_func, GL_BACK));
		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(ff_func, GL_CCW));

		_command_list.addCommand(Gaff::CachedFunction<void, GLenum>(gDisable_Enable_Funcs[0], GL_SCISSOR_TEST));
	}
}

void DeferredRenderDeviceGL::setBlendState(const BlendStateGL* blend_state)
{
	static auto bes_func = Gaff::BindSTDCall(glBlendEquationSeparatei);
	static auto bfs_func = Gaff::BindSTDCall(glBlendFuncSeparatei);
	static auto di_func = Gaff::BindSTDCall(glDisablei);
	static auto ei_func = Gaff::BindSTDCall(glEnablei);
	static auto cmi_func = Gaff::BindSTDCall(glColorMaski);

	const IBlendState::BlendStateSettings* blend_settings = (blend_state) ? blend_state->getBlendSettings() : nullptr;

	for (unsigned int i = 0; i < 8; ++i) {
		if (blend_settings && blend_settings[i].enable_alpha_blending) {
			_command_list.addCommand(Gaff::CachedFunction<void, GLenum, GLuint>(ei_func, GL_BLEND, i));

			Gaff::CachedFunction<void, GLenum, GLuint> ei_cache(ei_func, GL_BLEND, i);

			Gaff::CachedFunction<void, GLuint, GLenum, GLenum, GLenum, GLenum> bfs_cache(
				bfs_func, i, BlendStateGL::Blend_Factors[blend_settings[i].blend_src_color - 1],
				BlendStateGL::Blend_Factors[blend_settings[i].blend_dst_color - 1],
				BlendStateGL::Blend_Factors[blend_settings[i].blend_src_alpha - 1],
				BlendStateGL::Blend_Factors[blend_settings[i].blend_dst_alpha - 1]
			);

			Gaff::CachedFunction<void, GLuint, GLenum, GLenum> bes_cache(
				bes_func, i, BlendStateGL::Blend_Ops[blend_settings[i].blend_op_color - 1],
				BlendStateGL::Blend_Ops[blend_settings[i].blend_op_alpha - 1]
			);

			Gaff::CachedFunction<void, GLuint, GLboolean, GLboolean, GLboolean, GLboolean> cmi_cache(
				cmi_func, i,
				(blend_settings[i].color_write_mask & IBlendState::COLOR_RED) > 0,
				(blend_settings[i].color_write_mask & IBlendState::COLOR_GREEN) > 0,
				(blend_settings[i].color_write_mask & IBlendState::COLOR_BLUE) > 0,
				(blend_settings[i].color_write_mask & IBlendState::COLOR_ALPHA) > 0
			);

			_command_list.addCommand(ei_cache);
			_command_list.addCommand(bfs_cache);
			_command_list.addCommand(bes_cache);
			_command_list.addCommand(cmi_cache);

		} else {
			_command_list.addCommand(Gaff::CachedFunction<void, GLuint, GLboolean, GLboolean, GLboolean, GLboolean>(cmi_func, i, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
			_command_list.addCommand(Gaff::CachedFunction<void, GLenum, GLuint>(di_func, GL_BLEND, i));
		}
	}
}

void DeferredRenderDeviceGL::setLayout(LayoutGL* layout, const IMesh* mesh)
{
	static auto evaa_func = Gaff::BindSTDCall(glEnableVertexAttribArray);
	static auto vap_func = Gaff::BindSTDCall(glVertexAttribPointer);
	static auto bb_func = Gaff::BindSTDCall(glBindBuffer);

	const GleamArray< GleamArray<LayoutGL::LayoutData> >& layout_descs = layout->GetLayoutDescriptors();
	const LayoutGL::LayoutData* layout_data = nullptr;
	const BufferGL* buffer = nullptr;
	unsigned int stride = 0;
	unsigned int count = 0;

	for (unsigned int i = 0; i < layout_descs.size(); ++i) {
		const GleamArray<LayoutGL::LayoutData>& ld = layout_descs[i];
		buffer = reinterpret_cast<const BufferGL*>(mesh->getBuffer(i));
		stride = buffer->getStride();

		Gaff::CachedFunction<void, GLenum, GLuint> bb_cache(bb_func, GL_ARRAY_BUFFER, buffer->getBuffer());
		_command_list.addCommand(bb_cache);

		for (unsigned int j = 0; j < ld.size(); ++j) {
			layout_data = &ld[j];

			Gaff::CachedFunction<void, GLuint> evaa_cache(evaa_func, count);
			_command_list.addCommand(evaa_cache);

			Gaff::CachedFunction<void, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*> vap_cache(
				vap_func, count, layout_data->size, layout_data->type, layout_data->normalized,
				stride, reinterpret_cast<void*>(static_cast<uintptr_t>(layout_data->aligned_byte_offset))
			);

			_command_list.addCommand(vap_cache);
			++count;
		}
	}

	Gaff::CachedFunction<void, GLenum, GLuint> bb_cache(bb_func, GL_ARRAY_BUFFER, 0);
	_command_list.addCommand(bb_cache);
}

void DeferredRenderDeviceGL::unsetLayout(LayoutGL* layout)
{
	static auto gl_func = Gaff::BindSTDCall(glDisableVertexAttribArray);

	const GleamArray< GleamArray<LayoutGL::LayoutData> >& layout_descs = layout->GetLayoutDescriptors();
	unsigned int count = 0;

	for (unsigned int i = 0; i < layout_descs.size(); ++i) {
		for (unsigned int j = 0; j < layout_descs[i].size(); ++j) {
			Gaff::CachedFunction<void, GLuint> cached_func(gl_func, count);
			_command_list.addCommand(cached_func);
			++count;
		}
	}
}

void DeferredRenderDeviceGL::bindShader(ProgramGL* shader)
{
	static auto bpp_func = Gaff::BindSTDCall(glBindProgramPipeline);

	Gaff::CachedFunction<void, GLuint> bpp_cache(bpp_func, shader->getProgram());
	_command_list.addCommand(bpp_cache);
}

void DeferredRenderDeviceGL::bindProgramBuffers(ProgramBuffersGL* program_buffers)
{
	static auto bbb_func = Gaff::BindSTDCall(glBindBufferBase);
	static auto at_func = Gaff::BindSTDCall(glActiveTexture);
	static auto bt_func = Gaff::BindSTDCall(glBindTexture);
	static auto bs_func = Gaff::BindSTDCall(glBindSampler);

	unsigned int texture_count = 0;
	unsigned int count = 0;

	for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
		const GleamArray<IShaderResourceView*>& resource_views = program_buffers->getResourceViews(static_cast<Gleam::IShader::SHADER_TYPE>(i));
		const GleamArray<ISamplerState*>& sampler_states = program_buffers->getSamplerStates(static_cast<Gleam::IShader::SHADER_TYPE>(i));
		const GleamArray<IBuffer*>& const_bufs = program_buffers->getConstantBuffers(static_cast<Gleam::IShader::SHADER_TYPE>(i));

		GAFF_ASSERT(sampler_states.size() <= resource_views.size());
		unsigned int sampler_count = 0;

		for (unsigned int j = 0; j < const_bufs.size(); ++j) {
			Gaff::CachedFunction<void, GLuint, GLenum, GLenum> bbb_cache(bbb_func, GL_UNIFORM_BUFFER, count, reinterpret_cast<const BufferGL*>(const_bufs[j])->getBuffer());
			_command_list.addCommand(bbb_cache);

			++count;
		}

		for (unsigned int j = 0; j < resource_views.size(); ++j) {
			ShaderResourceViewGL* rv = reinterpret_cast<ShaderResourceViewGL*>(resource_views[j]);

			if (resource_views[j]->getViewType() == IShaderResourceView::VIEW_TEXTURE) {
				// should probably assert that texture_count isn't higher than the supported number of textures

				SamplerStateGL* st = reinterpret_cast<SamplerStateGL*>(sampler_states[sampler_count]);

				Gaff::CachedFunction<void, GLenum> at_cache(at_func, GL_TEXTURE0 + texture_count);
				Gaff::CachedFunction<void, GLenum, GLuint> bt_cache(bt_func, rv->getTarget(), rv->getResourceView());
				Gaff::CachedFunction<void, GLenum, GLenum> bs_cache(bs_func, texture_count, st->getSamplerState());
				_command_list.addCommand(at_cache);
				_command_list.addCommand(bt_cache);
				_command_list.addCommand(bs_cache);

				++texture_count;
				++sampler_count;

			} else {
				GAFF_ASSERT_MSG(false, "How is your ShaderResourceView not a texture? That's the only type we have implemented ...");
			}
		}
	}
}

void DeferredRenderDeviceGL::unbindShader(void)
{
	static auto gl_func = Gaff::BindSTDCall(glBindProgramPipeline);
	Gaff::CachedFunction<void, GLuint> cached_func(gl_func, 0);
	_command_list.addCommand(cached_func);
}

void DeferredRenderDeviceGL::renderMeshNonIndexed(unsigned int topology, unsigned int vert_count, unsigned int start_location)
{
	static auto gl_func = Gaff::BindSTDCall(glDrawArrays);
	Gaff::CachedFunction<void, GLenum, GLint, GLsizei> cached_func(gl_func, topology, start_location, vert_count);
	_command_list.addCommand(cached_func);
}

void DeferredRenderDeviceGL::renderMeshInstanced(MeshGL* mesh, unsigned int count)
{
	static auto bb_func = Gaff::BindSTDCall(glBindBuffer);
	Gaff::CachedFunction<void, GLenum, GLuint> bb_cache(bb_func, GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<BufferGL*>(mesh->getIndiceBuffer())->getBuffer());

	static auto dei_func = Gaff::BindSTDCall(glDrawElementsInstanced);
	Gaff::CachedFunction<void, GLenum, GLsizei, GLenum, const GLvoid*, GLsizei> dei_cache(dei_func, mesh->getGLTopology(), mesh->getIndexCount(), GL_UNSIGNED_INT, 0, count);

	_command_list.addCommand(bb_cache);
	_command_list.addCommand(dei_cache);
}

void DeferredRenderDeviceGL::renderMesh(MeshGL* mesh)
{
	static auto bb_func = Gaff::BindSTDCall(glBindBuffer);
	Gaff::CachedFunction<void, GLenum, GLuint> bb_cache(bb_func, GL_ELEMENT_ARRAY_BUFFER, reinterpret_cast<BufferGL*>(mesh->getIndiceBuffer())->getBuffer());

	static auto de_func = Gaff::BindSTDCall(glDrawElements);
	Gaff::CachedFunction<void, GLenum, GLsizei, GLenum, const GLvoid*> de_cache(de_func, mesh->getGLTopology(), mesh->getIndexCount(), GL_UNSIGNED_INT, 0);

	_command_list.addCommand(bb_cache);
	_command_list.addCommand(de_cache);
}

NS_END
