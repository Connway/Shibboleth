/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_IRenderManager.h"
#include <Shibboleth_Reflection.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class RenderManager final : public IManager, public IRenderManager
{
public:
	Gleam::IShaderResourceView* createShaderResourceView(void) const override;
	Gleam::IDepthStencilState* createDepthStencilState(void) const override;
	Gleam::IRenderDevice* createRenderDevice(void) const override;
	Gleam::IRenderOutput* createRenderOutput(void) const override;
	Gleam::IRenderTarget* createRenderTarget(void) const override;
	Gleam::ISamplerState* createSamplerState(void) const override;
	Gleam::ICommandList* createCommandList(void) const override;
	Gleam::IRasterState* createRasterState(void) const override;
	Gleam::IBlendState* createBlendState(void) const override;
	Gleam::ITexture* createTexture(void) const override;
	Gleam::IProgramBuffers* createProgramBuffers(void) const override;
	Gleam::IProgram* createProgram(void) const override;
	Gleam::IShader* createShader(void) const override;
	Gleam::IBuffer* createBuffer(void) const override;
	Gleam::ILayout* createLayout(void) const override;
	Gleam::IModel* createModel(void) const override;
	Gleam::IMesh* createMesh(void) const override;

	void setActiveCamera(CameraComponent* camera) override;

	SHIB_REFLECTION_CLASS_DECLARE(RenderManager);
};

NS_END

SHIB_REFLECTION_DECLARE(RenderManager)
