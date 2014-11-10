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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_HashMap.h>

#define OTTERUI_DEFAULT_MEMORY_BUFFER_SIZE 2 * 1024 * 1024

namespace Otter {
	class ISoundSystem;
	class IFileSystem;
	class IRenderer;
	class IPlugin;
	class System;
	class Scene;
}

NS_SHIBBOLETH

class OtterUIManager : public IManager
{
public:
	OtterUIManager(void);
	~OtterUIManager(void);

	bool init(unsigned int memory_size_bytes = OTTERUI_DEFAULT_MEMORY_BUFFER_SIZE, bool enable_pre_transformed_verts = true);
	void destroy(void);

	INLINE bool loadScene(const char* filename, const char* name, Otter::Scene** scene_out = nullptr);
	INLINE bool loadScene(unsigned char* buffer, unsigned int buffer_size, bool copy_buffer, const char* name, Otter::Scene** scene_out = nullptr);

	INLINE void clear(void);

	INLINE unsigned int getFPS(void) const;
	INLINE void setFPS(unsigned int fps);

	INLINE void update(float dt); // Time is in seconds
	INLINE bool draw(void);

	INLINE void setResolution(unsigned int width, unsigned int height);
	INLINE void setSoundSystem(Otter::ISoundSystem* sound_system);
	INLINE void setFileSystem(Otter::IFileSystem* file_system);
	INLINE void setRenderer(Otter::IRenderer* renderer);
	INLINE void addPlugin(Otter::IPlugin* plugin);

	INLINE Otter::Scene* getScene(unsigned int index);
	INLINE unsigned int getNumScenes(void) const;

	const char* getName(void) const;

	void* rawRequestInterface(unsigned int class_id) const;

private:
	HashMap<const char*, Otter::Scene*> _scenes;

	unsigned char* _memory_buffer;
	Otter::System* _system;
	unsigned int _fps;

	GAFF_NO_COPY(OtterUIManager);
	GAFF_NO_MOVE(OtterUIManager);
	
	REF_DEF_SHIB(OtterUIManager);
};

NS_END
