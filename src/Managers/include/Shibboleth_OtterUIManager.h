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

#include "Shibboleth_IOtterUIManager.h"
#include <Shibboleth_IManager.h>
#include <Shibboleth_HashMap.h>

namespace Otter
{
	class System;
}

NS_SHIBBOLETH

class OtterUIManager : public IManager, public IOtterUIManager
{
public:
	OtterUIManager(void);
	~OtterUIManager(void);

	const char* getName(void) const override;

	bool init(unsigned int memory_size_bytes = OTTERUI_DEFAULT_MEMORY_BUFFER_SIZE, bool enable_pre_transformed_verts = true) override;
	void destroy(void) override;

	bool loadScene(const char* filename, const char* name, Otter::Scene** scene_out = nullptr) override;
	bool loadScene(unsigned char* buffer, unsigned int buffer_size, bool copy_buffer, const char* name, Otter::Scene** scene_out = nullptr) override;

	void clear(void) override;

	unsigned int getFPS(void) const override;
	void setFPS(unsigned int fps) override;

	void setResolution(unsigned int width, unsigned int height) override;
	void setSoundSystem(Otter::ISoundSystem* sound_system) override;
	void setFileSystem(Otter::IFileSystem* file_system) override;
	void setRenderer(Otter::IRenderer* renderer) override;
	void addPlugin(Otter::IPlugin* plugin) override;

	Otter::Scene* getScene(unsigned int index) override;
	unsigned int getNumScenes(void) const override;

private:
	HashMap<const char*, Otter::Scene*> _scenes;

	unsigned char* _memory_buffer;
	Otter::System* _system;
	unsigned int _fps;

	// TODO: Convert to use UpdateManager
	void update(float dt); // Time is in seconds
	bool draw(void);

	GAFF_NO_COPY(OtterUIManager);
	GAFF_NO_MOVE(OtterUIManager);
	
	SHIB_REF_DEF(OtterUIManager);
	REF_DEF_REQ;
};

NS_END
