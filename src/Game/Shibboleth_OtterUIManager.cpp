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

#include "Shibboleth_OtterUIManager.h"
#include "Shibboleth_Allocator.h"
#include <Gaff_IncludeAssert.h>
#include <Otter.h>

NS_SHIBBOLETH

REF_IMPL_REQ(OtterUIManager);
REF_IMPL_ASSIGN_SHIB(OtterUIManager)
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
	_memory_buffer = (unsigned char*)GetAllocator()->alloc(memory_size_bytes);

	if (!_memory_buffer) {
		return false;
	}

	_system = GetAllocator()->template allocT<Otter::System>(_memory_buffer, memory_size_bytes);

	if (!_system) {
		GetAllocator()->free(_memory_buffer);
	}

	return _system->EnablePreTransformVerts(enable_pre_transformed_verts) == Otter::kResult_OK &&
		_system->EnableLogging(true) == Otter::kResult_OK;
}

void OtterUIManager::destroy(void)
{
	if (_system) {
		GetAllocator()->freeT(_system);
	}

	if (_memory_buffer) {
		GetAllocator()->free(_memory_buffer);
	}
}

bool OtterUIManager::loadScene(const char* filename, const char* name, Otter::Scene** scene_out)
{
	assert(_system && name && strlen(name));
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
	assert(_system && buffer && buffer_size && name && strlen(name));
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
	assert(_system);
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
	assert(_system);
	_system->Update((float)_fps * dt);
}

bool OtterUIManager::draw(void)
{
	assert(_system);
	return _system->Draw() == Otter::kResult_OK;
}

void OtterUIManager::setResolution(unsigned int width, unsigned int height)
{
	_system->SetResolution(width, height);
}

void OtterUIManager::setSoundSystem(Otter::ISoundSystem* sound_system)
{
	assert(sound_system);
	_system->SetSoundSystem(sound_system);
}

void OtterUIManager::setFileSystem(Otter::IFileSystem* file_system)
{
	assert(file_system);
	_system->SetFileSystem(file_system);
}

void OtterUIManager::setRenderer(Otter::IRenderer* renderer)
{
	assert(renderer);
	_system->SetRenderer(renderer);
}

void OtterUIManager::addPlugin(Otter::IPlugin* plugin)
{
	assert(plugin);
	_system->AddPlugin(plugin);
}

const char* OtterUIManager::getName(void) const
{
	return "OtterUI Manager";
}

Otter::Scene* OtterUIManager::getScene(unsigned int index)
{
	assert(_system && index < _system->GetSceneCount());
	return _system->GetScene(index);
}

unsigned int OtterUIManager::getNumScenes(void) const
{
	assert(_system);
	return _system->GetSceneCount();
}

NS_END
