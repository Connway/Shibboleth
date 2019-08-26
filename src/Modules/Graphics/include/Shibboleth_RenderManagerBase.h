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
#include <Shibboleth_IManager.h>
#include <Shibboleth_Vector.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderOutput.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IRenderTarget.h>
#include <Gleam_ISamplerState.h>
#include <Gleam_IProgram.h>
#include <Gleam_ITexture.h>
#include <Gleam_IWindow.h>
#include <Gaff_Hash.h>

NS_GLEAM
	class IDepthStencilState;
	class ICommandList;
	class IRasterState;
	class IBlendState;
	class IShader;
	class IBuffer;
	class ILayout;
	class IModel;
	class IMesh;
NS_END

NS_SHIBBOLETH

class RenderManagerBase : public IManager
{
public:
	RenderManagerBase(void);
	virtual ~RenderManagerBase(void);

	bool init(void) override;

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

	virtual Gleam::IRenderDevice::AdapterList getDisplayModes(void) const = 0;
	virtual Gleam::IWindow* createWindow(void) const = 0;

	virtual void updateWindows(void) = 0;

	void addRenderDeviceTag(Gleam::IRenderDevice* device, const char* tag);
	void manageRenderDevice(Gleam::IRenderDevice* device);

	const Vector<Gleam::IRenderDevice*>* getDevicesByTag(const char* tag) const;
	Gleam::IRenderDevice& getDevice(int32_t index) const;
	int32_t getNumDevices(void) const;

	Gleam::IRenderOutput* getOutput(const char* tag) const
	{
		return getOutput(Gaff::FNV1aHash32String(tag));
	}

	Gleam::IWindow* getWindow(const char* tag) const
	{
		return getWindow(Gaff::FNV1aHash32String(tag));
	}

	Gleam::IRenderOutput* getOutput(Gaff::Hash32 tag) const;
	Gleam::IWindow* getWindow(Gaff::Hash32 tag) const;

	Gleam::IProgramBuffers* getCameraProgramBuffers(Gaff::Hash32 tag) const;
	Gleam::IRenderTarget* getCameraRenderTarget(Gaff::Hash32 tag) const;

private:
	using OutputPtr = UniquePtr<Gleam::IRenderOutput>;
	using WindowPtr = UniquePtr<Gleam::IWindow>;
	using WindowOutputPair = eastl::pair<WindowPtr, OutputPtr>;

	using ProgramBuffersPtr = UniquePtr<Gleam::IProgramBuffers>;
	using SamplerPtr = UniquePtr<Gleam::ISamplerState>;
	using SRVPtr = UniquePtr<Gleam::IShaderResourceView>;
	using RTVPtr = UniquePtr<Gleam::IRenderTarget>;
	using TexturePtr = UniquePtr<Gleam::ITexture>;

	struct GBufferData final
	{
		ProgramBuffersPtr program_buffers;
		RTVPtr render_target;

		TexturePtr diffuse;
		TexturePtr specular;
		TexturePtr normal;
		TexturePtr position;
		TexturePtr depth;

		SRVPtr diffuse_srv;
		SRVPtr specular_srv;
		SRVPtr normal_srv;
		SRVPtr position_srv;
		SRVPtr depth_srv;
	};

	VectorMap<Gleam::IRenderDevice*, SamplerPtr> _to_screen_samplers;
	VectorMap<Gaff::Hash32, GBufferData> _g_buffers;

	VectorMap< Gaff::Hash32, Vector<Gleam::IRenderDevice*> > _render_device_tags{ ProxyAllocator("Graphics") };
	Vector< UniquePtr<Gleam::IRenderDevice> > _render_devices{ ProxyAllocator("Graphics") };
	VectorMap<Gaff::Hash32, WindowOutputPair> _window_outputs{ ProxyAllocator("Graphics") };


	Gleam::IRenderDevice* createRenderDevice(int32_t adapter_id);
	bool createGBuffer(const char* window_name);
};

NS_END
