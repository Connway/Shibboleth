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

#include "Shibboleth_ResourceDefines.h"
#include "Shibboleth_ResourceWrapper.h"
#include <Gaff_IVirtualDestructor.h>
#include <Gaff_SceneImporter.h>
#include <esprit_Skeleton.h>
#include <Gleam_IModel.h>
#include <Gleam_AABB.h>

NS_SHIBBOLETH

struct HoldingData : public Gaff::IVirtualDestructor
{
	HoldingData(void) {}
	~HoldingData(void) {}

	Gaff::SceneImporter importer;
	Gaff::Scene scene;
};

struct AnimationData : public Gaff::IVirtualDestructor
{
	AnimationData() {}
	~AnimationData() {}
};

struct ModelData : public Gaff::IVirtualDestructor
{
	ModelData(void) {}
	~ModelData(void) {}

	Array< Array<ModelPtr> > models; // [Device][LOD]
	Array< Array<Gleam::AABB> > aabbs; // [LOD][Mesh]

	ResourceWrapper<HoldingData> holding_data;
	esprit::Skeleton skeleton;
};

NS_END
