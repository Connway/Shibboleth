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

#include "Shibboleth_Defines.h"
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IDepthStencilState.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IRenderOutput.h>
#include <Gleam_IRenderTarget.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_ICommandList.h>
#include <Gleam_IRasterState.h>
#include <Gleam_IBlendState.h>
#include <Gleam_ITexture.h>
#include <Gleam_IProgram.h>
#include <Gleam_IShader.h>
#include <Gleam_IBuffer.h>
#include <Gleam_ILayout.h>
#include <Gleam_IModel.h>
#include <Gleam_IMesh.h>

NS_SHIBBOLETH

class CameraComponent;

// Only reason this exists is to force create*() code to run through the module DLL.
class IRenderManager
{
public:
	virtual ~IRenderManager(void) {}

	virtual Gleam::IShaderResourceView* createShaderResourceView(void) const = 0;
	virtual Gleam::IDepthStencilState* createDepthStencilState(void) const = 0;
	virtual Gleam::IRenderDevice* createRenderDevice(void) const = 0;
	virtual Gleam::IRenderOutput* createRenderOutput(void) const = 0;
	virtual Gleam::IRenderTarget* createRenderTarget(void) const = 0;
	virtual Gleam::ISamplerState* createSamplerState(void) const = 0;
	virtual Gleam::ICommandList* createCommandList(void) const = 0;
	virtual Gleam::IRasterState* createRasterState(void) const = 0;
	virtual Gleam::IBlendState* createBlendState(void) const = 0;
	virtual Gleam::ITexture* createTexture(void) const = 0;
	virtual Gleam::IProgramBuffers* createProgramBuffers(void) const = 0;
	virtual Gleam::IProgram* createProgram(void) const = 0;
	virtual Gleam::IShader* createShader(void) const = 0;
	virtual Gleam::IBuffer* createBuffer(void) const = 0;
	virtual Gleam::ILayout* createLayout(void) const = 0;
	virtual Gleam::IModel* createModel(void) const = 0;
	virtual Gleam::IMesh* createMesh(void) const = 0;

	virtual void setActiveCamera(CameraComponent* camera) = 0;
};

NS_END
