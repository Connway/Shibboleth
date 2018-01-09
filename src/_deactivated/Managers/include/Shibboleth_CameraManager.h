/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_ICameraManager.h"
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>

namespace Gleam
{
	class Vector4CPU;
}

NS_SHIBBOLETH

class IOcclusionManager;
class ModelComponent;
class Object;

struct ObjectData;

class CameraManager : public IManager, public IUpdateQuery, public ICameraManager
{
public:
	CameraManager(void);
	~CameraManager(void);

	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;

	void registerCamera(CameraComponent* camera) override;
	void removeCamera(CameraComponent* camera) override;

private:
	Array<CameraComponent*> _cameras;
	IOcclusionManager* _occlusion_mgr;

	void addModelComponent(ObjectData& od, ModelComponent* mc, const Gleam::Vector4CPU& eye_pos);
	void update(double, void* frame_data);

	SHIB_REF_DEF(CameraManager);
	REF_DEF_REQ;
};

NS_END
