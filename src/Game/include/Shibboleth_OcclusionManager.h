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

#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Watcher.h>
#include <Shibboleth_Array.h>
#include <Shibboleth_ITask.h>
#include <Shibboleth_Map.h>
#include <Gleam_AABB_CPU.h>
#include <Gaff_ReadWriteSpinLock.h>

NS_GLEAM
	class FrustumCPU;
NS_END

NS_SHIBBOLETH

class Object;
class IApp;

class OcclusionManager : public IManager, public IUpdateQuery
{
public:
	typedef Gaff::Pair<unsigned long long, unsigned long long> UserData;
	typedef Gaff::Pair<Object*, const UserData&> QueryResult;

	enum OBJ_TYPE
	{
		OT_STATIC = 0,
		OT_DYNAMIC,
		OT_LIGHT,
		OT_SIZE
	};

	struct OcclusionID
	{
		size_t index;
		OBJ_TYPE object_type;
	};

	struct QueryData
	{
		Array<QueryResult> results[OT_SIZE];
	};

	OcclusionManager(void);
	~OcclusionManager(void);

	void requestUpdateEntries(Array<UpdateEntry>& entries);
	void* rawRequestInterface(unsigned int class_id) const;
	const char* getName(void) const;

	OcclusionID addObject(Object* object, OBJ_TYPE object_type, const UserData& user_data = UserData(0, 0));
	INLINE void removeObject(Object* object);
	INLINE void removeObject(OcclusionID id);

	INLINE void constructStaticTree(const Array<Object*>& objects, Array<OcclusionID>* id_out = nullptr);

	void update(double);

	void findObjectsInFrustum(const Gleam::FrustumCPU& frustum, QueryData& out) const;
	INLINE QueryData findObjectsInFrustum(const Gleam::FrustumCPU& frustum) const;

private:
	class BVHTree
	{
	public:
		class FrustumTask : public ITask
		{
		public:
			FrustumTask(const Gleam::FrustumCPU& frustum, size_t branch_root, const BVHTree& tree);
			~FrustumTask(void);

			void doTask(void);

			const Array<QueryResult>& getResult(void) const;
			Array<QueryResult>& getResult(void);

		private:
			Array<QueryResult> _result;
			const Gleam::FrustumCPU& _frustum;
			const BVHTree& _tree;
			size_t _branch_root;

			void processBranch(size_t node_index);

			SHIB_REF_COUNTED(FrustumTask);
		};

		typedef Gaff::RefPtr<FrustumTask> FrustumTaskPtr;
		typedef Gaff::Pair<FrustumTaskPtr, FrustumTaskPtr> FrustumQueryTasks;

		BVHTree(void);
		~BVHTree(void);

		INLINE void setIsStatic(bool is_static);

		size_t addObject(Object* object, const UserData& user_data);
		void removeObject(size_t index);

		FrustumQueryTasks findObjectsInFrustum(const Gleam::FrustumCPU& frustum) const;

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

		void dirtyObjectCallback(Object* object, unsigned long long index);
		void growArrays(void);

		void addObjectHelper(const AddBufferData& data);
		void removeObjectHelper(size_t index);
		void updateAABBs(size_t index);

		friend class FrustumTask;
	};

	BVHTree _bvh_trees[OT_SIZE];
	Map<Object*, OcclusionID> _node_map;

	GAFF_NO_COPY(OcclusionManager);
	GAFF_NO_MOVE(OcclusionManager);

	REF_DEF_SHIB(OcclusionManager);
};

NS_END
