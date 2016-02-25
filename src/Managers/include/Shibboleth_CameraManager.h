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

#include "Shibboleth_OcclusionManager.h"

NS_SHIBBOLETH

class OcclusionManager;
class CameraComponent;
class ModelComponent;
class Object;

struct ObjectData;

class CameraManager : public IManager, public IUpdateQuery
{
public:
	CameraManager(void);
	~CameraManager(void);

	void* rawRequestInterface(Gaff::ReflectionHash class_id) const override;
	const char* getName(void) const override;
	void allManagersCreated(void) override;

	void getUpdateEntries(Array<UpdateEntry>& entries) override;
	void update(double, void* frame_data);

	void registerCamera(CameraComponent* camera);
	void removeCamera(CameraComponent* camera);

private:
	Array<CameraComponent*> _cameras;
	OcclusionManager* _occlusion_mgr;

	void addModelComponent(ObjectData& od, ModelComponent* mc, const Gleam::Vector4CPU& eye_pos);

	SHIB_REF_DEF(CameraManager);
};

NS_END
