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

#include "Gaff_Camera.h"
#include "Gaff_Assert.h"
#include <assimp/camera.h>

NS_GAFF

Camera::Camera(const aiCamera* camera):
	_camera(camera)
{
}

Camera::Camera(const Camera& camera):
	_camera(camera._camera)
{
}

Camera::Camera(void):
	_camera(nullptr)
{
}

Camera::~Camera(void)
{
}

const char* Camera::getName(void) const
{
	GAFF_ASSERT(_camera);
	return _camera->mName.C_Str();
}

const float* Camera::getPosition(void) const
{
	GAFF_ASSERT(_camera);
	return &_camera->mPosition.x;
}

const float* Camera::getDirection(void) const
{
	GAFF_ASSERT(_camera);
	return &_camera->mLookAt.x;
}

const float* Camera::getUp(void) const
{
	GAFF_ASSERT(_camera);
	return &_camera->mUp.x;
}

float Camera::getHorizFOV(void) const
{
	GAFF_ASSERT(_camera);
	return _camera->mHorizontalFOV;
}

float Camera::getClipNear(void) const
{
	GAFF_ASSERT(_camera);
	return _camera->mClipPlaneNear;
}

float Camera::getClipFar(void) const
{
	GAFF_ASSERT(_camera);
	return _camera->mClipPlaneFar;
}

float Camera::getAspectRatio(void) const
{
	GAFF_ASSERT(_camera);
	return _camera->mAspect;
}

bool Camera::valid(void) const
{
	return _camera != nullptr;
}

const Camera& Camera::operator=(const Camera& rhs)
{
	_camera = rhs._camera;
	return *this;
}

bool Camera::operator==(const Camera& rhs) const
{
	return _camera == rhs._camera;
}

bool Camera::operator!=(const Camera& rhs) const
{
	return _camera != rhs._camera;
}

NS_END
