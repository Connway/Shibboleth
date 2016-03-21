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

#include "Shibboleth_OtterUIManager.h"
#include <Otter.h>

NS_SHIBBOLETH

REF_IMPL_REQ(OtterUIManager);
SHIB_REF_IMPL(OtterUIManager)
.addBaseClassInterfaceOnly<OtterUIManager>()
;

OtterUIManager::OtterUIManager(void):
	_memory_buffer(nullptr), _system(nullptr), _fps(60)
{
}

OtterUIManager::~OtterUIManager(void)
{
	destroy();
}

bool OtterUIManager::init(unsigned int memory_size_bytes, bool enable_pre_transformed_verts)
{
	_memory_buffer = reinterpret_cast<unsigned char*>(SHIB_ALLOC_GLOBAL(memory_size_bytes, *GetAllocator()));

	if (!_memory_buffer) {
		return false;
	}

	_system = SHIB_ALLOCT(Otter::System, *GetAllocator(), _memory_buffer, memory_size_bytes);

	if (!_system) {
		SHIB_FREE(_memory_buffer, *GetAllocator());
	}

	return _system->EnablePreTransformVerts(enable_pre_transformed_verts) == Otter::kResult_OK &&
			_system->EnableLogging(true) == Otter::kResult_OK;
}

void OtterUIManager::destroy(void)
{
	if (_system) {
		SHIB_FREET(_system, *GetAllocator());
	}

	if (_memory_buffer) {
		SHIB_FREE(_memory_buffer, *GetAllocator());
	}
}

bool OtterUIManager::loadScene(const char* filename, const char* name, Otter::Scene** scene_out)
{
	GAFF_ASSERT(_system && name && strlen(name));
	Otter::Scene* out_temp = nullptr;
	_system->LoadScene(filename, &out_temp);

	if (out_temp) {
		_scenes[name] = out_temp;

		if (scene_out) {
			*scene_out = out_temp;
		}
	}

	return out_temp != nullptr;
}

bool OtterUIManager::loadScene(unsigned char* buffer, unsigned int buffer_size, bool copy_buffer, const char* name, Otter::Scene** scene_out)
{
	GAFF_ASSERT(_system && buffer && buffer_size && name && strlen(name));
	Otter::Scene* out_temp = nullptr;

	_system->LoadScene(buffer, buffer_size, copy_buffer, &out_temp);

	if (out_temp) {
		_scenes[name] = out_temp;

		if (scene_out) {
			*scene_out = out_temp;
		}
	}

	return out_temp != nullptr;
}

void OtterUIManager::clear(void)
{
	GAFF_ASSERT(_system);
	_system->UnloadAllScenes();
	_scenes.clear();
}

unsigned int OtterUIManager::getFPS(void) const
{
	return _fps;
}

void OtterUIManager::setFPS(unsigned int fps)
{
	_fps = fps;
}

void OtterUIManager::update(float dt)
{
	GAFF_ASSERT(_system);
	_system->Update((float)_fps * dt);
}

bool OtterUIManager::draw(void)
{
	GAFF_ASSERT(_system);
	return _system->Draw() == Otter::kResult_OK;
}

void OtterUIManager::setResolution(unsigned int width, unsigned int height)
{
	_system->SetResolution(width, height);
}

void OtterUIManager::setSoundSystem(Otter::ISoundSystem* sound_system)
{
	GAFF_ASSERT(sound_system);
	_system->SetSoundSystem(sound_system);
}

void OtterUIManager::setFileSystem(Otter::IFileSystem* file_system)
{
	GAFF_ASSERT(file_system);
	_system->SetFileSystem(file_system);
}

void OtterUIManager::setRenderer(Otter::IRenderer* renderer)
{
	GAFF_ASSERT(renderer);
	_system->SetRenderer(renderer);
}

void OtterUIManager::addPlugin(Otter::IPlugin* plugin)
{
	GAFF_ASSERT(plugin);
	_system->AddPlugin(plugin);
}

const char* OtterUIManager::getName(void) const
{
	return "OtterUI Manager";
}

Otter::Scene* OtterUIManager::getScene(unsigned int index)
{
	GAFF_ASSERT(_system && index < _system->GetSceneCount());
	return _system->GetScene(index);
}

unsigned int OtterUIManager::getNumScenes(void) const
{
	GAFF_ASSERT(_system);
	return _system->GetSceneCount();
}

NS_END
