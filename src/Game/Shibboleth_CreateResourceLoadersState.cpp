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

#include "Shibboleth_CreateResourceLoadersState.h"
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_RenderManager.h>
#include <Shibboleth_TaskPoolTags.h>

#include <Shibboleth_ProgramBuffersCreator.h>
#include <Shibboleth_ShaderProgramLoader.h>
#include <Shibboleth_SamplerStateLoader.h>
#include <Shibboleth_RenderTargetLoader.h>
#include <Shibboleth_BufferCreator.h>
#include <Shibboleth_TextureLoader.h>
#include <Shibboleth_HoldingLoader.h>
#include <Shibboleth_ShaderLoader.h>
#include <Shibboleth_ModelLoader.h>
#include <Shibboleth_LuaLoader.h>

#include <Shibboleth_String.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

static ProxyAllocator g_Esprit_Allocator;

CreateResourceLoadersState::CreateResourceLoadersState(IApp& app):
	_app(app)
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
	assert(_transitions.size() == 1);
	//_state_id = state_id;
	return true;
}

void CreateResourceLoadersState::enter(void)
{
}

void CreateResourceLoadersState::update(void)
{
	_app.getGameLogFile().first.writeString("==================================================\n");
	_app.getGameLogFile().first.writeString("==================================================\n");
	_app.getGameLogFile().first.printf("Creating Resource Loaders...\n");

	RenderManager& render_manager = _app.getManagerT<RenderManager>("Render Manager");
	ResourceManager& res_mgr = _app.getManagerT<ResourceManager>("Resource Manager");

	// PROGRAM BUFFERS CREATOR
	{
		ProgramBuffersCreator* program_buffers_creator = GetAllocator()->template allocT<ProgramBuffersCreator>();

		if (!program_buffers_creator) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create program buffers creator.\n");
			_app.quit();
			return;
		}

		_app.getGameLogFile().first.printf("Adding Program Buffers Creator\n");
		res_mgr.registerResourceLoader(program_buffers_creator, "ProgramBuffers");
	}

	// BUFFER CREATOR
	{
		BufferCreator* buffer_creator = GetAllocator()->template allocT<BufferCreator>();

		if (!buffer_creator) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create buffer creator.\n");
			_app.quit();
			return;
		}

		_app.getGameLogFile().first.printf("Adding Buffer Creator\n");
		res_mgr.registerResourceLoader(buffer_creator, "Buffer", TPT_GRAPHICS);
	}

	// TEXTURE LOADER
	{
		TextureLoader* texture_loader = GetAllocator()->template allocT<TextureLoader>(render_manager);

		if (!texture_loader) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create texture loader.\n");
			_app.quit();
			return;
		}

		//Array<AString> extensions;
		//extensions.emplacePush(".png");
		//extensions.emplacePush(".jpeg");
		//extensions.emplacePush(".jpg");
		//extensions.emplacePush(".bmp");
		//extensions.emplacePush(".tiff");
		//extensions.emplacePush(".tif");
		//extensions.emplacePush(".dds");
		//extensions.emplacePush(".tga");

		Array<ResourceManager::JSONModifiers> json_elements;
		ResourceManager::JSONModifiers modifiers = { AString("image_file"), AString(), false };
		json_elements.emplacePush(modifiers);

		_app.getGameLogFile().first.printf("Adding Texture Loader\n");
		res_mgr.registerResourceLoader(texture_loader, ".texture", TPT_GRAPHICS, json_elements);
	}

	// SAMPLER STATE LOADER
	{
		SamplerStateLoader* sampler_loader = GetAllocator()->template allocT<SamplerStateLoader>(render_manager);

		if (!sampler_loader) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create sampler state loader.\n");
			_app.quit();
			return;
		}

		_app.getGameLogFile().first.printf("Adding Sampler State Loader\n");
		res_mgr.registerResourceLoader(sampler_loader, ".sampler", TPT_GRAPHICS);
	}

	// SHADER LOADER
	{
		ShaderLoader* shader_loader = GetAllocator()->template allocT<ShaderLoader>(render_manager);

		if (!shader_loader) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create shader loader.\n");
			_app.quit();
			return;
		}

		_app.getGameLogFile().first.printf("Adding Shader Loader\n");
		res_mgr.registerResourceLoader(shader_loader, render_manager.getShaderExtension(), TPT_GRAPHICS);
	}

	// SHADER PROGRAM LOADER
	{
		ShaderProgramLoader* shader_program_loader = GetAllocator()->template allocT<ShaderProgramLoader>(res_mgr, render_manager);

		if (!shader_program_loader) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create shader program loader.\n");
			_app.quit();
			return;
		}

		Array<ResourceManager::JSONModifiers> json_elements;
		ResourceManager::JSONModifiers modifiers = { AString("Vertex"), AString(render_manager.getShaderExtension()), true };
		json_elements.emplacePush(modifiers);

		modifiers.json_element = "Pixel";
		json_elements.emplacePush(modifiers);

		modifiers.json_element = "Hull";
		json_elements.emplacePush(modifiers);

		modifiers.json_element = "Geometry";
		json_elements.emplacePush(modifiers);

		modifiers.json_element = "Domain";
		json_elements.emplacePush(modifiers);

		_app.getGameLogFile().first.printf("Adding Shader Program Loader\n");
		res_mgr.registerResourceLoader(shader_program_loader, ".material", TPT_GRAPHICS, json_elements);
	}

	// LUA LOADER
	{
		LuaLoader* lua_loader = GetAllocator()->template allocT<LuaLoader>();

		if (!lua_loader) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create Lua loader.\n");
			_app.quit();
			return;
		}

		_app.getGameLogFile().first.printf("Adding Lua Loader\n");
		res_mgr.registerResourceLoader(lua_loader, ".lua");
	}

	// HOLDING LOADER
	{
		HoldingLoader* holding_loader = GetAllocator()->template allocT<HoldingLoader>();

		if (!holding_loader) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create Holding loader.\n");
			_app.quit();
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

		_app.getGameLogFile().first.printf("Adding Holding Loader\n");
		res_mgr.registerResourceLoader(holding_loader, extensions);
	}

	// MODEL LOADER
	{
		ModelLoader* model_loader = GetAllocator()->template allocT<ModelLoader>(render_manager, res_mgr, *_app.getFileSystem());

		if (!model_loader) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create Model loader.\n");
			_app.quit();
			return;
		}

		Array<ResourceManager::JSONModifiers> json_elements;
		ResourceManager::JSONModifiers modifiers = { AString("mesh_file"), AString(), true };
		json_elements.emplacePush(modifiers);

		_app.getGameLogFile().first.printf("Adding Model Loader\n");
		res_mgr.registerResourceLoader(model_loader, ".model", TPT_GRAPHICS, json_elements);
	}

	// RENDER TARGET LOADER
	{
		RenderTargetLoader* render_target_loader = GetAllocator()->template allocT<RenderTargetLoader>();

		if (!render_target_loader) {
			_app.getGameLogFile().first.printf("ERROR - Failed to create Render Target loader.\n");
			_app.quit();
			return;
		}

		_app.getGameLogFile().first.printf("Adding Render Target Loader\n");
		res_mgr.registerResourceLoader(render_target_loader, ".rendertarget", TPT_GRAPHICS);
	}

	_app.getGameLogFile().first.printf("Finished Creating Resource Loaders\n\n");
	_app.switchState(_transitions[0]);
}

void CreateResourceLoadersState::exit(void)
{
}

NS_END
