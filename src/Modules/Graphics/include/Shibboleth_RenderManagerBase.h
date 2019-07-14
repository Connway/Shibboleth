/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include <Shibboleth_SmartPtrs.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_Vector.h>
#include <Gleam_Defines.h>
#include <Gaff_Hash.h>

namespace Gleam
{
	class IShaderResourceView;
	class IDepthStencilState;
	class IProgramBuffers;
	class IRenderDevice;
	class IRenderOutput;
	class IRenderTarget;
	class ISamplerState;
	class ICommandList;
	class IRasterState;
	class IBlendState;
	class ITexture;
	class IProgram;
	class IShader;
	class IBuffer;
	class ILayout;
	class IModel;
	class IMesh;
}

NS_SHIBBOLETH

// Only reason this exists is to force create*() code to run through the module DLL.
class RenderManagerBase
{
public:
	virtual ~RenderManagerBase(void);

	virtual Gleam::RendererType getRendererType(void) const = 0;

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
	virtual Gleam::IMesh* createMesh(void) const = 0;

	void addRenderDeviceTag(Gleam::IRenderDevice* device, const char* tag);
	void manageRenderDevice(Gleam::IRenderDevice* device);

	const Vector<Gleam::IRenderDevice*>* getDevicesByTag(const char* tag) const;
	Gleam::IRenderDevice& getDevice(int32_t index) const;
	int32_t getNumDevices(void) const;

private:
	VectorMap< Gaff::Hash32, Vector<Gleam::IRenderDevice*> > _render_device_tags;
	Vector< UniquePtr<Gleam::IRenderDevice> > _render_devices;
};

NS_END
