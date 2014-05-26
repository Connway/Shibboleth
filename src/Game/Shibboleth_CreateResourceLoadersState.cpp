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

#include "Shibboleth_CreateResourceLoadersState.h"
#include "Shibboleth_ShaderProgramLoader.h"
#include "Shibboleth_ResourceManager.h"
#include "Shibboleth_TextureLoader.h"
#include "Shibboleth_ShaderLoader.h"
#include "Shibboleth_String.h"
#include "Shibboleth_App.h"

NS_SHIBBOLETH

CreateResourceLoadersState::CreateResourceLoadersState(App& app) :
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

	RenderManager& render_manager = _app.getManager<RenderManager>("Render Manager");
	ResourceManager& res_mgr = _app.getManager<ResourceManager>("Resource Manager");

	// TEXTURE LOADER
	{
		TextureLoader* texture_loader = _app.getAllocator().template allocT<TextureLoader>(render_manager);

		if (!texture_loader) {
			// log error
			_app.getGameLogFile().first.printf("ERROR - Failed to create texture loader.\n");
			_app.quit();
			return;
		}

		Array<AString> extensions;
		extensions.movePush(AString(".png"));
		extensions.movePush(AString(".jpeg"));
		extensions.movePush(AString(".jpg"));
		extensions.movePush(AString(".bmp"));
		extensions.movePush(AString(".tiff"));
		extensions.movePush(AString(".tif"));
		extensions.movePush(AString(".dds"));
		extensions.movePush(AString(".tga"));

		_app.getGameLogFile().first.printf("Adding Texture Loader\n");
		res_mgr.registerResourceLoader(texture_loader, extensions);
	}

	// SHADER LOADER
	{
		ShaderLoader* shader_loader = _app.getAllocator().template allocT<ShaderLoader>(render_manager);

		if (!shader_loader) {
			// log error
			_app.getGameLogFile().first.printf("ERROR - Failed to create shader loader.\n");
			_app.quit();
			return;
		}

		Array<AString> extensions;
		//extensions.movePush(AString(".shader"));
		extensions.movePush(AString(".hlsl"));
		extensions.movePush(AString(".glsl"));

		_app.getGameLogFile().first.printf("Adding Shader Loader\n");
		res_mgr.registerResourceLoader(shader_loader, extensions);
	}

	// SHADER PROGRAM LOADER
	{
		ShaderProgramLoader* shader_program_loader = _app.getAllocator().template allocT<ShaderProgramLoader>(render_manager);

		if (!shader_program_loader) {
			// log error
			_app.getGameLogFile().first.printf("ERROR - Failed to create shader program loader.\n");
			_app.quit();
			return;
		}

		_app.getGameLogFile().first.printf("Adding Shader Program Loader\n");
		res_mgr.registerResourceLoader(shader_program_loader, ".program");
	}

	_app.getGameLogFile().first.printf("Finished Creating Resource Loaders\n\n");
	_app.getStateMachine().switchState(_transitions[0]);
}

void CreateResourceLoadersState::exit(void)
{
}

NS_END
