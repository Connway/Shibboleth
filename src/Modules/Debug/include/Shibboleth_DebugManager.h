/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_IDebugManager.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_ISystem.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IDepthStencilState.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_IRasterState.h>
#include <Gleam_IBlendState.h>
#include <Gleam_IProgram.h>
#include <Gleam_IShader.h>
#include <Gleam_IBuffer.h>
#include <Gleam_ILayout.h>
#include <Gleam_IMesh.h>

NS_GLEAM
	class IRenderOutput;
NS_END

NS_SHIBBOLETH

class RenderManagerBase;
struct Time;

class DebugManager final : public IManager, public IDebugManager
{
public:
	bool initAllModulesLoaded(void) override;

	void update(void);
	void render(void);

	ImGuiContext* getImGuiContext(void) const override;

private:
	const Time* _time = nullptr;
	int32_t _prev_cursor = -1;

	Gleam::IRenderOutput* _main_output = nullptr;
	Gleam::IRenderDevice* _main_device = nullptr;

	RenderManagerBase* _render_mgr = nullptr;
	UniquePtr<Gleam::IBuffer> _vertex_buffer;
	UniquePtr<Gleam::IBuffer> _index_buffer;
	UniquePtr<Gleam::IMesh> _mesh;

	UniquePtr<Gleam::IDepthStencilState> _depth_stencil_state;
	UniquePtr<Gleam::IProgramBuffers> _program_buffers;
	UniquePtr<Gleam::IShaderResourceView> _font_srv;
	UniquePtr<Gleam::IBuffer> _vert_constant_buffer;
	UniquePtr<Gleam::IRasterState> _raster_state;
	UniquePtr<Gleam::IBlendState> _blend_state;
	UniquePtr<Gleam::ITexture> _font_texture;
	UniquePtr<Gleam::IShader> _vertex_shader;
	UniquePtr<Gleam::ISamplerState> _sampler;
	UniquePtr<Gleam::IShader> _pixel_shader;
	UniquePtr<Gleam::IProgram> _program;
	UniquePtr<Gleam::ILayout> _layout;

	SHIB_REFLECTION_CLASS_DECLARE(DebugManager);
};

class DebugRenderSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(void) override;

private:
	DebugManager* _debug_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(DebugRenderSystem);
};

class DebugSystem final : public ISystem
{
public:
	bool init(void) override;
	void update(void) override;

private:
	DebugManager* _debug_mgr = nullptr;

	SHIB_REFLECTION_CLASS_DECLARE(DebugSystem);
};

NS_END

SHIB_REFLECTION_DECLARE(DebugManager)
SHIB_REFLECTION_DECLARE(DebugRenderSystem)
SHIB_REFLECTION_DECLARE(DebugSystem)
