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

#include "Shibboleth_ISceneManager.h"
#include <Shibboleth_IManager.h>
#include <Shibboleth_String.h>
#include <Shibboleth_Array.h>

NS_GAFF
	struct JobData;
NS_END

NS_SHIBBOLETH

class Object;

class SceneManager : public IManager, public ISceneManager
{
public:
	SceneManager(void);
	~SceneManager(void);

	const char* getName(void) const;

	void loadScene(const char* scene) override;

	bool activateLayer(const char* layer) override;
	bool deactivateLayer(const char* layer) override;
	void activateLayer(size_t layer) override;
	void deactivateLayer(size_t layer) override;

private:
	struct Layer
	{
		Array<Object*> objects;
		U8String name;
		uint32_t hash;
	};

	struct Scene
	{
		Array<Layer> layers;
		U8String name;
	};

	Scene _scene;
	Array< Array<Gaff::JobData> > _job_cache[2];
	U8String _scene_path;

	void loadLayer(size_t index, const Gaff::JSON& layer);

	GAFF_NO_COPY(SceneManager);
	GAFF_NO_MOVE(SceneManager);

	SHIB_REF_DEF(SceneManager);
	REF_DEF_REQ;

	friend void LoadSceneJob(void* data);
};

NS_END
