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

#pragma once

#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_String.h>
#include <Shibboleth_Array.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_Matrix4x4.h>
#include <Gleam_ITexture.h>
#include <Gaff_RefPtr.h>

NS_SHIBBOLETH

class SpatialManager;
class RenderManager;
class IApp;

class RasterManager : public IManager, public IUpdateQuery
{
public:
	typedef Gaff::RefPtr<Gleam::IShaderResourceView> SRVPtr;
	typedef Gaff::RefPtr<Gleam::ITexture> TexturePtr;
	typedef Gaff::Pair<TexturePtr, SRVPtr> RenderData;

	RasterManager(IApp& app);
	~RasterManager(void);

	void requestUpdateEntries(Array<UpdateEntry>& entries);
	void allManagersCreated(void);

	INLINE unsigned int getNumCameras(void) const;
	INLINE void setCameraEnabled(unsigned int camera, bool enabled);
	INLINE bool isCameraEnabled(unsigned int camera);
	INLINE void setCameraTransform(unsigned int camera, const Gleam::Matrix4x4* transform);
	INLINE void setCameraDeviceEnabled(unsigned int camera, unsigned int device, bool enabled);

	// Don't hold on to this reference if you plan on dynamically creating cameras.
	// Just copy the structure instead.
	INLINE RenderData& getCameraRenderData(unsigned int camera, unsigned int device);

	unsigned int createCamera(
		unsigned int width, unsigned int height,
		Gleam::ITexture::FORMAT format = Gleam::ITexture::RGBA_8_UNORM,
		const AString& name = AString()
	);

	INLINE void deleteCameras(void);

	void setCameraRenderOrder(const unsigned int* order, unsigned int size);
	INLINE void setCameraRenderOrder(const Array<unsigned int>& order);

	void* rawRequestInterface(unsigned int class_id) const;

private:
	struct CameraDeviceData
	{
		RenderData render_data;
		bool enabled;
	};

	struct CameraData
	{
		Array<CameraDeviceData> device_data;
		AString name;
		const Gleam::Matrix4x4* transform;
		bool enabled;
	};

	Array<unsigned int> _camera_order;
	Array<CameraData> _camera_data;

	SpatialManager* _spatial_mgr;
	RenderManager* _render_mgr;
	IApp& _app;

	GAFF_NO_COPY(RasterManager);
	GAFF_NO_MOVE(RasterManager);

	REF_DEF_SHIB(RasterManager);
};

NS_END
