/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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
#include "Gleam_SamplerState_OpenGL.h"
#include "Gleam_RenderState_OpenGL.h"
#include "Gleam_Program_OpenGL.h"
#include "Gleam_Layout_OpenGL.h"
#include "Gleam_Buffer_OpenGL.h"
#include "Gleam_Mesh_OpenGL.h"
#include <GL/glew.h>

NS_GLEAM

// For internal Gleam use only.
template <class ReturnType, class... Args>
class GLFuncWrapper : public Gaff::IFunction<void>
{
public:
	GLFuncWrapper(const Gaff::CachedFunction<ReturnType, Args...>& function): _function(function) {}
	GLFuncWrapper(Gaff::CachedFunction<ReturnType, Args...>&& function): _function(std::move(function)) {}
	~GLFuncWrapper(void);

	void call(void) const { _function(); }
	void call(void) { _function(); }
	bool valid(void) const { return _function.valid(); }

private:
	Gaff::CachedFunction<ReturnType, Args...> _function;
};

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

bool DeferredRenderDeviceGL::isD3D(void) const
{
	return false;
}

void DeferredRenderDeviceGL::executeCommandList(ICommandList* command_list)
{
	assert(!command_list->isD3D());
	_command_list.append(reinterpret_cast<CommandListGL&>(*command_list));
}

bool DeferredRenderDeviceGL::finishCommandList(ICommandList* command_list)
{
	assert(!command_list->isD3D());
	*command_list = std::move(_command_list);
	return true;
}

void DeferredRenderDeviceGL::renderNoVertexInput(unsigned int vert_count)
{
	// TODO: IMPLEMENT ME!
}

void DeferredRenderDeviceGL::setRenderState(const RenderStateGL* render_state)
{
	static Gaff::FunctionBinder<void, GLenum> disable_enable_funcs[2] = { Gaff::BindSTDCall(glDisable), Gaff::BindSTDCall(glEnable) };
	static auto bes_func = Gaff::BindSTDCall(glBlendEquationSeparatei);
	static auto sfs_func = Gaff::BindSTDCall(glStencilFuncSeparate);
	static auto bfs_func = Gaff::BindSTDCall(glBlendFuncSeparatei);
	static auto sos_func = Gaff::BindSTDCall(glStencilOpSeparate);
	static auto pm_func = Gaff::BindSTDCall(glPolygonMode);
	static auto sm_func = Gaff::BindSTDCall(glStencilMask);
	static auto df_func = Gaff::BindSTDCall(glDepthFunc);
	static auto ff_func = Gaff::BindSTDCall(glFrontFace);
	static auto cf_func = Gaff::BindSTDCall(glCullFace);
	static auto di_func = Gaff::BindSTDCall(glDisablei);
	static auto ei_func = Gaff::BindSTDCall(glEnablei);

	Gaff::CachedFunction<void, GLenum, GLenum> pm_cache(pm_func, GL_FRONT_AND_BACK, (render_state->isWireframe()) ? GL_LINE : GL_FILL);
	_command_list.addCommand(pm_cache);

	Gaff::CachedFunction<void, GLenum> dte_cache(disable_enable_funcs[render_state->isDepthTestEnabled()], GL_DEPTH_TEST);
	Gaff::CachedFunction<void, GLenum> ste_cache(disable_enable_funcs[render_state->isStencilTestEnabled()], GL_STENCIL_TEST);
	_command_list.addCommand(dte_cache);
	_command_list.addCommand(ste_cache);

	Gaff::CachedFunction<void, GLenum> df_cache(df_func, RenderStateGL::Compare_Funcs[render_state->getDepthFunc() - 1]);
	_command_list.addCommand(df_cache);


	const IRenderState::StencilData& front_face = render_state->getFrontFaceStencilData();
	const IRenderState::StencilData& back_face = render_state->getBackFaceStencilData();

	Gaff::CachedFunction<void, GLenum, GLenum, GLenum, GLenum> sosf_cache(
		sos_func, GL_FRONT, RenderStateGL::Stencil_Ops[front_face.stencil_depth_fail - 1],
		RenderStateGL::Stencil_Ops[front_face.stencil_pass_depth_fail - 1],
		RenderStateGL::Stencil_Ops[front_face.stencil_depth_pass - 1]
	);

	Gaff::CachedFunction<void, GLenum, GLenum, GLenum, GLenum> sosb_cache(
		sos_func, GL_BACK, RenderStateGL::Stencil_Ops[back_face.stencil_depth_fail - 1],
		RenderStateGL::Stencil_Ops[back_face.stencil_pass_depth_fail - 1],
		RenderStateGL::Stencil_Ops[back_face.stencil_depth_pass - 1]
	);

	Gaff::CachedFunction<void, GLenum, GLenum, GLint, GLuint> sfsf_cache(
		sfs_func, GL_FRONT,
		RenderStateGL::Compare_Funcs[front_face.comp_func - 1], render_state->getStencilRef(),
		render_state->getStencilReadMask()
	);

	Gaff::CachedFunction<void, GLenum, GLenum, GLint, GLuint> sfsb_cache(
		sfs_func, GL_FRONT,
		RenderStateGL::Compare_Funcs[back_face.comp_func - 1], render_state->getStencilRef(),
		render_state->getStencilReadMask()
	);

	Gaff::CachedFunction<void, GLenum> sm_cache(sm_func, render_state->getStencilWriteMask());

	_command_list.addCommand(sosf_cache);
	_command_list.addCommand(sosb_cache);
	_command_list.addCommand(sfsf_cache);
	_command_list.addCommand(sfsb_cache);
	_command_list.addCommand(sm_cache);


	IRenderState::CULL_MODE cull_face_mode = render_state->getCullFaceMode();

	Gaff::CachedFunction<void, GLenum> cfed_cache(disable_enable_funcs[cull_face_mode != IRenderState::CULL_NONE], GL_CULL_FACE);
	Gaff::CachedFunction<void, GLenum> cf_cache(cf_func, GL_FRONT - 2 + cull_face_mode);
	Gaff::CachedFunction<void, GLenum> ff_cache(ff_func, (render_state->isFrontFaceCounterClockwise()) ? GL_CCW : GL_CW);
	_command_list.addCommand(cfed_cache);
	_command_list.addCommand(cf_cache);
	_command_list.addCommand(ff_cache);

	const IRenderState::BlendData* blend_data = render_state->getBlendData();

	for (unsigned int i = 0; i < 8; ++i) {
		if (blend_data[i].enable_alpha_blending) {
			Gaff::CachedFunction<void, GLenum, GLuint> ei_cache(ei_func, GL_BLEND, i);

			Gaff::CachedFunction<void, GLuint, GLenum, GLenum, GLenum, GLenum> bfs_cache(
				bfs_func, i, RenderStateGL::Blend_Factors[blend_data[i].blend_src_color - 1],
				RenderStateGL::Blend_Factors[blend_data[i].blend_dst_color - 1],
				RenderStateGL::Blend_Factors[blend_data[i].blend_src_alpha - 1],
				RenderStateGL::Blend_Factors[blend_data[i].blend_dst_alpha - 1]
			);

			Gaff::CachedFunction<void, GLuint, GLenum, GLenum> bes_cache(
				bes_func, i, RenderStateGL::Blend_Ops[blend_data[i].blend_op_color - 1],
				RenderStateGL::Blend_Ops[blend_data[i].blend_op_alpha - 1]
			);

			_command_list.addCommand(ei_cache);
			_command_list.addCommand(bfs_cache);
			_command_list.addCommand(bes_cache);

		} else {
			Gaff::CachedFunction<void, GLenum, GLuint> di_cache(di_func, GL_BLEND, i);
			_command_list.addCommand(di_cache);
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
		buffer = (const BufferGL*)mesh->getBuffer(i);
		stride = buffer->getStride();

		Gaff::CachedFunction<void, GLenum, GLuint> bb_cache(bb_func, GL_ARRAY_BUFFER, buffer->getBuffer());
		_command_list.addCommand(bb_cache);

		for (unsigned int j = 0; j < ld.size(); ++j) {
			layout_data = &ld[j];

			Gaff::CachedFunction<void, GLuint> evaa_cache(evaa_func, count);
			_command_list.addCommand(evaa_cache);

#if defined(_WIN64) || defined(__LP64__)
			Gaff::CachedFunction<void, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*> vap_cache(
				vap_func, count, layout_data->size, layout_data->type, layout_data->normalized,
				stride, (void*)(unsigned long long)layout_data->aligned_byte_offset
			);
#else
			Gaff::CachedFunction<void, GLuint, GLint, GLenum, GLboolean, GLsizei, const GLvoid*> vap_cache(
				vap_func, count, layout_data->size, layout_data->type, layout_data->normalized,
				stride, (void*)layout_data->aligned_byte_offset
			);
#endif

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

void DeferredRenderDeviceGL::bindShader(ProgramGL* shader, ProgramBuffersGL* program_buffers)
{
	static auto bpp_func = Gaff::BindSTDCall(glBindProgramPipeline);
	static auto bbb_func = Gaff::BindSTDCall(glBindBufferBase);
	static auto at_func = Gaff::BindSTDCall(glActiveTexture);
	static auto bt_func = Gaff::BindSTDCall(glBindTexture);
	static auto bs_func = Gaff::BindSTDCall(glBindSampler);

	Gaff::CachedFunction<void, GLuint> bpp_cache(bpp_func, shader->getProgram());
	_command_list.addCommand(bpp_cache);

	if (program_buffers) {
		unsigned int texture_count = 0;
		unsigned int count = 0;

		for (unsigned int i = 0; i < IShader::SHADER_TYPE_SIZE - 1; ++i) {
			const GleamArray<IShaderResourceView*>& resource_views = program_buffers->getResourceViews((Gleam::IShader::SHADER_TYPE)i);
			const GleamArray<ISamplerState*>& sampler_states = program_buffers->getSamplerStates((Gleam::IShader::SHADER_TYPE)i);
			const GleamArray<IBuffer*>& const_bufs = program_buffers->getConstantBuffers((Gleam::IShader::SHADER_TYPE)i);

			assert(sampler_states.size() <= resource_views.size());
			unsigned int sampler_count = 0;

			for (unsigned int j = 0; j < const_bufs.size(); ++j) {
				Gaff::CachedFunction<void, GLuint, GLenum, GLenum> bbb_cache(bbb_func, GL_UNIFORM_BUFFER, count, ((const BufferGL*)const_bufs[j])->getBuffer());
				_command_list.addCommand(bbb_cache);

				++count;
			}

			for (unsigned int j = 0; j < resource_views.size(); ++j) {
				ShaderResourceViewGL* rv = (ShaderResourceViewGL*)resource_views[j];

				if (resource_views[j]->getViewType() == IShaderResourceView::VIEW_TEXTURE) {
					// should probably assert that texture_count isn't higher than the supported number of textures

					SamplerStateGL* st = (SamplerStateGL*)sampler_states[sampler_count];

					Gaff::CachedFunction<void, GLenum> at_cache(at_func, GL_TEXTURE0 + texture_count);
					Gaff::CachedFunction<void, GLenum, GLuint> bt_cache(bt_func, rv->getTarget(), rv->getResourceView());
					Gaff::CachedFunction<void, GLenum, GLenum> bs_cache(bs_func, texture_count, st->getSamplerState());
					_command_list.addCommand(at_cache);
					_command_list.addCommand(bt_cache);
					_command_list.addCommand(bs_cache);

					++texture_count;
					++sampler_count;

				} else {
					assert(0 && "How is your ShaderResourceView not a texture? That's the only type we have implemented ...");
				}
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
	Gaff::CachedFunction<void, GLenum, GLuint> bb_cache(bb_func, GL_ELEMENT_ARRAY_BUFFER, ((BufferGL*)mesh->getIndiceBuffer())->getBuffer());

	static auto dei_func = Gaff::BindSTDCall(glDrawElementsInstanced);
	Gaff::CachedFunction<void, GLenum, GLsizei, GLenum, const GLvoid*, GLsizei> dei_cache(dei_func, mesh->getGLTopology(), mesh->getIndexCount(), GL_UNSIGNED_INT, 0, count);

	_command_list.addCommand(bb_cache);
	_command_list.addCommand(dei_cache);
}

void DeferredRenderDeviceGL::renderMesh(MeshGL* mesh)
{
	static auto bb_func = Gaff::BindSTDCall(glBindBuffer);
	Gaff::CachedFunction<void, GLenum, GLuint> bb_cache(bb_func, GL_ELEMENT_ARRAY_BUFFER, ((BufferGL*)mesh->getIndiceBuffer())->getBuffer());

	static auto de_func = Gaff::BindSTDCall(glDrawElements);
	Gaff::CachedFunction<void, GLenum, GLsizei, GLenum, const GLvoid*> de_cache(de_func, mesh->getGLTopology(), mesh->getIndexCount(), GL_UNSIGNED_INT, 0);

	_command_list.addCommand(bb_cache);
	_command_list.addCommand(de_cache);
}

NS_END
