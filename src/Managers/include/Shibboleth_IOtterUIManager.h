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

#pragma once

#include <Shibboleth_ReflectionDefinitions.h>

#define OTTERUI_DEFAULT_MEMORY_BUFFER_SIZE 2 * 1024 * 1024

namespace Otter
{
	class ISoundSystem;
	class IFileSystem;
	class IRenderer;
	class IPlugin;
	class Scene;
}

NS_SHIBBOLETH

class IOtterUIManager
{
public:
	IOtterUIManager(void) {}
	virtual ~IOtterUIManager(void) {}

	virtual bool init(unsigned int memory_size_bytes = OTTERUI_DEFAULT_MEMORY_BUFFER_SIZE, bool enable_pre_transformed_verts = true) = 0;
	virtual void destroy(void) = 0;

	virtual bool loadScene(const char* filename, const char* name, Otter::Scene** scene_out = nullptr) = 0;
	virtual bool loadScene(unsigned char* buffer, unsigned int buffer_size, bool copy_buffer, const char* name, Otter::Scene** scene_out = nullptr) = 0;

	virtual void clear(void) = 0;

	virtual unsigned int getFPS(void) const = 0;
	virtual void setFPS(unsigned int fps) = 0;

	virtual void setResolution(unsigned int width, unsigned int height) = 0;
	virtual void setSoundSystem(Otter::ISoundSystem* sound_system) = 0;
	virtual void setFileSystem(Otter::IFileSystem* file_system) = 0;
	virtual void setRenderer(Otter::IRenderer* renderer) = 0;
	virtual void addPlugin(Otter::IPlugin* plugin) = 0;

	virtual Otter::Scene* getScene(unsigned int index) = 0;
	virtual unsigned int getNumScenes(void) const = 0;

	SHIB_INTERFACE_REFLECTION(IOtterUIManager)
	SHIB_INTERFACE_NAME("OtterUI Manager")
};

NS_END
