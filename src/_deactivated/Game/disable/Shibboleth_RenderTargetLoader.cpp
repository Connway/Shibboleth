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

/*
	NOTE: data->width/height calculations are going to be wrong for data with mixed sizes. (eg: -1/-1 and actual with/height values mixing)
*/

#include "Shibboleth_RenderTargetLoader.h"
#include "Shibboleth_ResourceDefines.h"
#include <Shibboleth_ISchemaManager.h>
#include <Shibboleth_IRenderManager.h>
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_ITexture.h>

NS_SHIBBOLETH

static bool addOutput(Gleam::IRenderDevice& rd, IRenderManager& rm, Gleam::IRenderTarget* rt, RenderTargetData* data, int window_width, int window_height, const Gaff::JSON& settings, const char* file_name)
{
	Gleam::ITexture::FORMAT t_fmt = GetEnumRefDef<Gleam::ITexture::FORMAT>().getValue(settings["Format"].getString());
	int width = static_cast<int>(settings["Width"].getInt());
	int height = static_cast<int>(settings["Height"].getInt());

	if (width == -1 || height == -1) {
		// If window_width and window_height are also -1, then we are using a bad tag.
		if (window_width == -1 || window_height == -1) {
			GetApp().getLogManager().logMessage(
				LogManager::LOG_ERROR, GetApp().getLogFileName(),
				"ERROR - Depth-Stencil buffer for Render Target '%s' was marked to use window dimensions, but window tag has more than one window associated with it.\n",
				file_name
			);

			return false;
		}

		width = window_width;
		height = window_height;

		data->width = static_cast<unsigned int>(window_width);
		data->height = static_cast<unsigned int>(window_height);

	} else {
		data->width = Gaff::Max(static_cast<unsigned int>(width), data->width);
		data->height = Gaff::Max(static_cast<unsigned int>(height), data->height);
	}

	TexturePtr texture(rm.createTexture());

	if (!texture) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate texture for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!texture->init2D(rd, width, height, t_fmt)) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize texture data for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!rt->addTexture(rd, texture.get())) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate texture data for Render Target '%s'.\n", file_name);
		return false;
	}

	ShaderResourceViewPtr srv(rm.createShaderResourceView());

	if (!srv) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate shader resource view for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!srv->init(rd, texture.get())) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize shader resource view for Render Target '%s'.\n", file_name);
		return false;
	}

	for (size_t i = 0; i < data->textures.width(); ++i) {
		if (!data->textures[rd.getCurrentDevice()][i]) {
			data->textures[rd.getCurrentDevice()][i] = std::move(texture);
			data->texture_srvs[rd.getCurrentDevice()][i] = std::move(srv);
		}
	}

	return true;
}

static bool addDepthStencil(Gleam::IRenderDevice& rd, IRenderManager& rm, Gleam::IRenderTarget* rt, RenderTargetData* data, int window_width, int window_height, const Gaff::JSON& settings, const char* file_name)
{
	// We're not adding Depth-Stencil to this Render Target.
	if (!settings["Depth-Stencil Format"].isString()) {
		return true;
	}

	U8String depth_stencil_format(settings["Depth-Stencil Format"].getString());

	Gleam::ITexture::FORMAT ds_fmt = GetEnumRefDef<Gleam::ITexture::FORMAT>().getValue(depth_stencil_format.getBuffer());

	if (ds_fmt < Gleam::ITexture::DEPTH_16_UNORM) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Render Target file '%s' is malformed. Value at 'Depth-Stencil Format' is not a depth-stencil format.\n", file_name);
		return false;
	}

	int width = static_cast<int>(settings["Depth-Stencil Width"].getInt());
	int height = static_cast<int>(settings["Depth-Stencil Height"].getInt());

	if (width == -1 || height == -1) {
		// If window_width and window_height are also -1, then we are using a bad tag.
		if (window_width == -1 || window_height == -1) {
			GetApp().getLogManager().logMessage(
				LogManager::LOG_ERROR, GetApp().getLogFileName(),
				"ERROR - Depth-Stencil buffer for Render Target '%s' was marked to use window dimensions, but window tag has more than one window associated with it.\n",
				file_name
			);

			return false;
		}

		width = window_width;
		height = window_height;

		// Use the largest texture in the group for width/height.
		data->width = static_cast<unsigned int>(window_width);
		data->height = static_cast<unsigned int>(window_height);

	} else {
		// Just take the largest texture size as the width/height value for this render target.
		data->width = Gaff::Max(static_cast<unsigned int>(width), data->width);
		data->height = Gaff::Max(static_cast<unsigned int>(height), data->height);
	}

	TexturePtr texture(rm.createTexture());

	if (!texture) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate depth-stencil texture for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!texture->initDepthStencil(rd, width, height, ds_fmt)) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize depth-stencil texture data for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!rt->addDepthStencilBuffer(rd, texture.get())) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate depth-stencil texture data for Render Target '%s'.\n", file_name);
		return false;
	}

	ShaderResourceViewPtr srv(rm.createShaderResourceView());

	if (!srv) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate depth-stencil shader resource view for Render Target '%s'.\n", file_name);
		return false;
	}

	if (!srv->init(rd, texture.get())) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize depth-stencil shader resource view for Render Target '%s'.\n", file_name);
		return false;
	}

	data->depth_stencil_srvs[rd.getCurrentDevice()] = srv;
	data->depth_stencils[rd.getCurrentDevice()] = texture;

	return true;
}


RenderTargetLoader::RenderTargetLoader(void)
{
}

RenderTargetLoader::~RenderTargetLoader(void)
{
}

ResourceLoadData RenderTargetLoader::load(const IFile* file, ResourceContainer* res_cont)
{
	const char* file_name = res_cont->getResourceKey().getBuffer();
	Gaff::JSON rt_settings;
	bool success = rt_settings.parse(file->getBuffer());

	if (!success) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to parse file '%s'.\n", file_name);
		return { nullptr };
	}

	static const Gaff::JSON& schema = GetApp().getManagerT<ISchemaManager>().getSchema("RenderTarget.schema");

	if (!rt_settings.validate(schema)) {
		GetApp().getLogManager().logMessage(
			LogManager::LOG_ERROR, GetApp().getLogFileName(),
			"ERROR - Render Target file '%s' has bad schema for reason '%s'.\n",
			file_name,
			rt_settings.getErrorText()
		);

		return { nullptr };
	}

	IRenderManager& rm = GetApp().getManagerT<IRenderManager>();
	Gaff::JSON display_tags = rt_settings["Display Tags"];
	uint16_t disp_tags = DT_ALL;

	if (!display_tags.isNull()) {
		disp_tags = 0;

		disp_tags = rm.getDislayTags(display_tags);

		if (!disp_tags) {
			// This should never get hit. Schema check would have failed.
			GetApp().getLogManager().logMessage(
				LogManager::LOG_ERROR, GetApp().getLogFileName(),
				"ERROR - Render Target file '%s' is malformed (an element in 'display_tags' is not a string) or no tags were found.\n",
				file_name
			);

			return { nullptr };
		}
	}

	// RenderTargetData setup
	RenderTargetData* data = SHIB_ALLOCT(RenderTargetData, *GetAllocator());
	Gleam::IRenderDevice& rd = rm.getRenderDevice();

	if (!data) {
		GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate data for Render Target for '%s'.\n", file_name);
		return { nullptr };
	}

	data->render_targets.resize(rd.getNumDevices());
	data->depth_stencils.resize(rd.getNumDevices());
	data->depth_stencil_srvs.resize(rd.getNumDevices());

	data->tags = disp_tags;
	data->any_display_with_tags = rt_settings["Any Display With Tags"].isTrue();

	const Gaff::JSON& outputs = rt_settings["Outputs"];

	// Get windows with these tags
	Array<const IRenderManager::WindowData*> windows = (data->any_display_with_tags) ?
		rm.getWindowsWithTagsAny(disp_tags) :
		rm.getWindowsWithTags(disp_tags);

	GAFF_ASSERT(!windows.empty());

	// Cache the window width/height if there is only one window with this tag
	int window_width = -1;
	int window_height = -1;

	if (windows.size() == 1) {
		window_width = static_cast<int>(windows[0]->window->getWidth());
		window_height = static_cast<int>(windows[0]->window->getHeight());
	}

	for (auto it = windows.begin(); it != windows.end(); ++it) {
		// Skip if we have already made a render target for this device
		if (data->render_targets[(*it)->device]) {
			continue;
		}

		RenderTargetPtr rt(rm.createRenderTarget());
		rd.setCurrentDevice((*it)->device);

		if (!rt) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to allocate Render Target '%s'.\n", file_name);
			SHIB_FREET(data, *GetAllocator());
			return { nullptr };
		}

		if (!rt->init()) {
			GetApp().getLogManager().logMessage(LogManager::LOG_ERROR, GetApp().getLogFileName(), "ERROR - Failed to initialize Render Target for '%s'.\n", file_name);
			SHIB_FREET(data, *GetAllocator());
			return { nullptr };
		}

		if (outputs.isArray()) {
			data->textures.resize(outputs.size(), rd.getNumDevices());
			data->texture_srvs.resize(outputs.size(), rd.getNumDevices());

			bool failed = outputs.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
			{
				if (!addOutput(rd, rm, rt.get(), data, window_width, window_height, value, file_name)) {
					return true;
				}

				return false;
			});

			if (failed) {
				SHIB_FREET(data, *GetAllocator());
				return { nullptr };
			}
		}

		if (!addDepthStencil(rd, rm, rt.get(), data, window_width, window_height, rt_settings, file_name)) {
			SHIB_FREET(data, *GetAllocator());
			return { nullptr };
		}

		data->render_targets[(*it)->device] = rt;
	}

	return { data };
}

NS_END