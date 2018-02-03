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

#include "Shibboleth_IOcclusionManager.h"
#include <Shibboleth_IUpdateQuery.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Watcher.h>
#include <Shibboleth_Map.h>
#include <Gleam_AABB_CPU.h>

NS_GAFF
	struct Counter;
NS_END

NS_GLEAM
	class FrustumCPU;
NS_END

NS_SHIBBOLETH

class OcclusionManager : public IManager, public IUpdateQuery, public IOcclusionManager
{
public:

	OcclusionManager(void);
	~OcclusionManager(void);

	void getUpdateEntries(Array<UpdateEntry>& entries) override;
	const char* getName(void) const override;

	OcclusionID addObject(Object* object, OBJ_TYPE object_type, const UserData& user_data = UserData(0, 0)) override;
	void removeObject(Object* object) override;
	void removeObject(OcclusionID id) override;

	void constructStaticTree(const Array<Object*>& objects, Array<OcclusionID>* id_out = nullptr) override;

	void findObjectsInFrustum(const Gleam::FrustumCPU& frustum, OBJ_TYPE object_type, Array<QueryResult>& out) const override;
	void findObjectsInFrustum(const Gleam::FrustumCPU& frustum, QueryData& out) const override;
	Array<QueryResult> findObjectsInFrustum(const Gleam::FrustumCPU& frustum, OBJ_TYPE object_type) const override;
	QueryData findObjectsInFrustum(const Gleam::FrustumCPU& frustum) const override;

private:
	class BVHTree
	{
	public:
		struct FrustumData
		{
			Array<QueryResult> result;
			const Gleam::FrustumCPU* frustum;
			const BVHTree* tree;
			size_t branch_root;
			Gaff::Counter* counter;
		};

		using FrustumQueryData = Gaff::Pair<FrustumData, FrustumData>;

		BVHTree(void);
		~BVHTree(void);

		void setIsStatic(bool is_static);

		size_t addObject(Object* object, const UserData& user_data);
		void removeObject(size_t index);

		void findObjectsInFrustum(const Gleam::FrustumCPU& frustum, FrustumQueryData& out) const;

		// Bottom-up construction
		void construct(const Array<Object*>& objects, Array<OcclusionID>* id_out);

		void update(void);

	private:
		struct BVHNode
		{
			Gleam::AABBCPU aabb;
			UserData user_data;
			Object* object;
			size_t index; // index in the _node_cache we live in
			size_t parent;
			size_t left;
			size_t right;

			bool dirty;
		};

		struct AddBufferData
		{
			UserData user_data;
			Object* object;
			size_t index;
		};

		Array<BVHNode> _node_cache; // Avoids allocations and keeps some cache coherency
		Array<size_t> _dirty_indices;
		Array<size_t> _free_indices;

		Array<AddBufferData> _add_buffer;
		Array<size_t> _remove_buffer;

		Gaff::SpinLock _remove_lock;
		Gaff::SpinLock _add_lock;
		Gaff::SpinLock _fi_lock;
		Gaff::SpinLock _nc_lock;

		size_t _root;

		bool _is_static;

		void dirtyObjectCallback(Object* object, uint64_t index);
		void growArrays(void);

		void addObjectHelper(const AddBufferData& data);
		void removeObjectHelper(size_t index);
		void updateAABBs(size_t index);

		static void ProcessBranch(FrustumData* frustum_data, size_t node_index);
		static void FrustumJob(void* data);
	};

	BVHTree _bvh_trees[OT_SIZE];
	Map<Object*, OcclusionID> _node_map;

	void update(double, void*);

	GAFF_NO_COPY(OcclusionManager);
	GAFF_NO_MOVE(OcclusionManager);

	SHIB_REF_DEF(OcclusionManager);
	REF_DEF_REQ;
};

NS_END
