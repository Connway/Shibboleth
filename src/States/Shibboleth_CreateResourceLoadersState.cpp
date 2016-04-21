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

#include "Shibboleth_CreateResourceLoadersState.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_RenderManager.h>

#include <Shibboleth_ProgramBuffersCreator.h>
#include <Shibboleth_ShaderProgramLoader.h>
#include <Shibboleth_SamplerStateLoader.h>
#include <Shibboleth_RenderTargetLoader.h>
#include <Shibboleth_BufferCreator.h>
#include <Shibboleth_PhysicsLoader.h>
#include <Shibboleth_TextureLoader.h>
#include <Shibboleth_HoldingLoader.h>
#include <Shibboleth_ShaderLoader.h>
#include <Shibboleth_ModelLoader.h>
#include <Shibboleth_LuaLoader.h>

#include <Shibboleth_Utilities.h>
#include <Shibboleth_String.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

const char* CreateResourceLoadersState::GetFriendlyName(void)
{
	return "Create Resource Loaders State";
}

CreateResourceLoadersState::CreateResourceLoadersState(void)
{
}

CreateResourceLoadersState::~CreateResourceLoadersState(void)
{
}

bool CreateResourceLoadersState::init(unsigned int /*state_id*/)
{
	// Make sure we have a transition for when we finish loading.
	// If we have more than one transition, you're using this state wrong
	// and should fix it.
	GAFF_ASSERT(_transitions.size() == 1);
	//_state_id = state_id;
	return true;
}

void CreateResourceLoadersState::enter(void)
{
}

void CreateResourceLoadersState::update(void)
{
	IApp& app = GetApp();

	app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), 
		"\n==================================================\n"
		"==================================================\n"
		"Creating Resource Loaders...\n"
	);

	RenderManager& render_mgr = app.getManagerT<RenderManager>("Render Manager");
	ResourceManager& res_mgr = app.getManagerT<ResourceManager>("Resource Manager");

	// PROGRAM BUFFERS CREATOR
	{
		ProgramBuffersCreator* program_buffers_creator = SHIB_ALLOCT(ProgramBuffersCreator, *GetAllocator());

		if (!program_buffers_creator) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create program buffers creator.\n");
			app.quit();
			return;
		}

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Program Buffers Creator\n");
		res_mgr.registerResourceLoader(program_buffers_creator, "ProgramBuffers");
	}

	// BUFFER CREATOR
	{
		BufferCreator* buffer_creator = SHIB_ALLOCT(BufferCreator, *GetAllocator());

		if (!buffer_creator) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create buffer creator.\n");
			app.quit();
			return;
		}

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Buffer Creator\n");
		res_mgr.registerResourceLoader(buffer_creator, "Buffer");
	}

	// TEXTURE LOADER
	{
		TextureLoader* texture_loader = SHIB_ALLOCT(TextureLoader, *GetAllocator(), render_mgr);

		if (!texture_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create texture loader.\n");
			app.quit();
			return;
		}

		Array<ResourceManager::FileReadInfo> json_elements;
		ResourceManager::FileReadInfo modifiers = { AString("image_file"), AString(), false };
		json_elements.emplacePush(modifiers);

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Texture Loader\n");
		res_mgr.registerResourceLoader(texture_loader, ".texture", 0, json_elements);
	}

	// SAMPLER STATE LOADER
	{
		SamplerStateLoader* sampler_loader = SHIB_ALLOCT(SamplerStateLoader, *GetAllocator(),render_mgr);

		if (!sampler_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create sampler state loader.\n");
			app.quit();
			return;
		}

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Sampler State Loader\n");
		res_mgr.registerResourceLoader(sampler_loader, ".sampler");
	}

	// SHADER LOADER
	{
		ShaderLoader* shader_loader = SHIB_ALLOCT(ShaderLoader, *GetAllocator(), render_mgr);

		if (!shader_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create shader loader.\n");
			app.quit();
			return;
		}

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Shader Loader\n");
		res_mgr.registerResourceLoader(shader_loader, render_mgr.getShaderExtension());
	}

	// SHADER PROGRAM LOADER
	{
		SchemaManager& schema_mgr = app.getManagerT<SchemaManager>("Schema Manager");

		ShaderProgramLoader* shader_program_loader = SHIB_ALLOCT(ShaderProgramLoader, *GetAllocator(), res_mgr, schema_mgr, render_mgr);

		if (!shader_program_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create shader program loader.\n");
			app.quit();
			return;
		}

		Array<ResourceManager::FileReadInfo> json_elements;
		ResourceManager::FileReadInfo file_info = { AString("Vertex"), AString(render_mgr.getShaderExtension()), true };
		json_elements.emplacePush(file_info);

		file_info.json_element = "Pixel";
		json_elements.emplacePush(file_info);

		file_info.json_element = "Hull";
		json_elements.emplacePush(file_info);

		file_info.json_element = "Geometry";
		json_elements.emplacePush(file_info);

		file_info.json_element = "Domain";
		json_elements.emplacePush(file_info);

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Shader Program Loader\n");
		res_mgr.registerResourceLoader(shader_program_loader, ".material", 0, json_elements);
	}

	// LUA LOADER
	{
		LuaLoader* lua_loader = SHIB_ALLOCT(LuaLoader, *GetAllocator());

		if (!lua_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Lua loader.\n");
			app.quit();
			return;
		}

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Lua Loader\n");
		res_mgr.registerResourceLoader(lua_loader, ".lua");
	}

	// HOLDING LOADER
	{
		HoldingLoader* holding_loader = SHIB_ALLOCT(HoldingLoader, *GetAllocator());

		if (!holding_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Holding loader.\n");
			app.quit();
			return;
		}

		Array<AString> extensions;
		extensions.emplacePush(".b3d");
		extensions.emplacePush(".dae");
		extensions.emplacePush(".blend");
		extensions.emplacePush(".3ds");
		extensions.emplacePush(".ase");
		extensions.emplacePush(".obj");
		extensions.emplacePush(".ifc");
		extensions.emplacePush(".xgl");
		extensions.emplacePush(".zgl");
		extensions.emplacePush(".ply");
		extensions.emplacePush(".lwo");
		extensions.emplacePush(".lws");
		extensions.emplacePush(".lxo");
		extensions.emplacePush(".stl");
		extensions.emplacePush(".dxf");
		extensions.emplacePush(".x");
		extensions.emplacePush(".ac");
		extensions.emplacePush(".ms3d");
		extensions.emplacePush(".cob");
		extensions.emplacePush(".scn");
		extensions.emplacePush(".bvh");
		extensions.emplacePush(".csm");
		extensions.emplacePush(".irrmesh");
		extensions.emplacePush(".irr");
		extensions.emplacePush(".mdl");
		extensions.emplacePush(".md2");
		extensions.emplacePush(".md3");
		extensions.emplacePush(".pk3");
		extensions.emplacePush(".mdc");
		extensions.emplacePush(".md5");
		extensions.emplacePush(".smd");
		extensions.emplacePush(".vta");
		extensions.emplacePush(".m3");
		extensions.emplacePush(".3d");
		extensions.emplacePush(".q3d");
		extensions.emplacePush(".q3s");
		extensions.emplacePush(".nff");
		extensions.emplacePush(".off");
		extensions.emplacePush(".raw");
		extensions.emplacePush(".ter");
		extensions.emplacePush(".hmp");
		extensions.emplacePush(".ndo");
		extensions.emplacePush(".fbx");

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Holding Loader\n");
		res_mgr.registerResourceLoader(holding_loader, extensions);
	}

	// MODEL LOADER
	{
		ModelLoader* model_loader = SHIB_ALLOCT(ModelLoader, *GetAllocator(), render_mgr, res_mgr, *app.getFileSystem());

		if (!model_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Model loader.\n");
			app.quit();
			return;
		}

		Array<ResourceManager::FileReadInfo> json_elements;
		ResourceManager::FileReadInfo modifiers = { AString("mesh_file"), AString(), true };
		json_elements.emplacePush(modifiers);

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Model Loader\n");
		res_mgr.registerResourceLoader(model_loader, ".model", 0, json_elements);
	}

	// RENDER TARGET LOADER
	{
		RenderTargetLoader* render_target_loader = SHIB_ALLOCT(RenderTargetLoader, *GetAllocator());

		if (!render_target_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Render Target loader.\n");
			app.quit();
			return;
		}

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Render Target Loader\n");
		res_mgr.registerResourceLoader(render_target_loader, ".rendertarget");
	}

	// PHYSICS LOADER
	{
		PhysicsLoader* physics_loader = SHIB_ALLOCT(PhysicsLoader, *GetAllocator());

		if (!physics_loader) {
			app.getLogManager().logMessage(LogManager::LOG_ERROR, app.getLogFileName(), "ERROR - Failed to create Physics loader.\n");
			app.quit();
			return;
		}

		app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Adding Render Target Loader\n");
		res_mgr.registerResourceLoader(physics_loader, ".physics");
	}

	app.getLogManager().logMessage(LogManager::LOG_NORMAL, app.getLogFileName(), "Finished Creating Resource Loaders\n\n");
	app.switchState(_transitions[0]);
}

void CreateResourceLoadersState::exit(void)
{
}

NS_END
