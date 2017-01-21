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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_Array.h>

namespace Gleam
{
	class FrustumCPU;
}

NS_SHIBBOLETH

class Object;

class IOcclusionManager
{
public:
	using UserData = Gaff::Pair<uint64_t, uint64_t>;
	using QueryResult = Gaff::Pair<Object*, const UserData&>;

	enum OBJ_TYPE
	{
		OT_STATIC = 0,
		OT_DYNAMIC,
		OT_LIGHT,
		OT_SIZE
	};

	struct OcclusionID
	{
		size_t index = SIZE_T_FAIL;
		OBJ_TYPE object_type = OT_SIZE;
	};

	struct QueryData
	{
		Array<QueryResult> results[OT_SIZE];
	};

	IOcclusionManager(void) {}
	virtual ~IOcclusionManager(void) {}

	virtual OcclusionID addObject(Object* object, OBJ_TYPE object_type, const UserData& user_data = UserData(0, 0)) = 0;
	virtual void removeObject(Object* object) = 0;
	virtual void removeObject(OcclusionID id) = 0;

	virtual void constructStaticTree(const Array<Object*>& objects, Array<OcclusionID>* id_out = nullptr) = 0;

	virtual void findObjectsInFrustum(const Gleam::FrustumCPU& frustum, OBJ_TYPE object_type, Array<QueryResult>& out) const = 0;
	virtual void findObjectsInFrustum(const Gleam::FrustumCPU& frustum, QueryData& out) const = 0;
	virtual Array<QueryResult> findObjectsInFrustum(const Gleam::FrustumCPU& frustum, OBJ_TYPE object_type) const = 0;
	virtual QueryData findObjectsInFrustum(const Gleam::FrustumCPU& frustum) const = 0;

	SHIB_INTERFACE_REFLECTION(IOcclusionManager)
	SHIB_INTERFACE_NAME("Occlusion Manager")
};

NS_END
