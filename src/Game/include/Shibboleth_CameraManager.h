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

#pragma once

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>

NS_SHIBBOLETH

class OcclusionManager;
class CameraComponent;
class RenderManager;
class IApp;

class CameraManager : public IManager, public IUpdateQuery
{
public:
	CameraManager(IApp& app);
	~CameraManager(void);

	const char* getName(void) const;
	void allManagersCreated(void);

	void* rawRequestInterface(unsigned int class_id) const;

	void requestUpdateEntries(Array<UpdateEntry>& entries);
	void update(double);

	void registerCamera(CameraComponent* camera, size_t position = SIZE_T_FAIL);
	void unregisterCamera(CameraComponent* camera);

private:
	Array<CameraComponent*> _cameras;

	OcclusionManager* _occlusion_manager;
	RenderManager* _render_manager;
	IApp& _app;

	GAFF_NO_COPY(CameraManager);
	GAFF_NO_MOVE(CameraManager);

	REF_DEF_SHIB(CameraManager);
};

NS_END
