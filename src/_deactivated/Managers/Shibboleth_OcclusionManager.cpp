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

#include "Shibboleth_OcclusionManager.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_Object.h>
#include <Shibboleth_IApp.h>
#include <Gleam_Frustum_CPU.h>
#include <Gaff_ScopedLock.h>

#define BVH_STARTING_CACHE 500

NS_SHIBBOLETH

REF_IMPL_REQ(OcclusionManager);
SHIB_REF_IMPL(OcclusionManager)
.addBaseClassInterfaceOnly<OcclusionManager>()
.ADD_BASE_CLASS_INTERFACE_ONLY(IOcclusionManager)
.ADD_BASE_CLASS_INTERFACE_ONLY(IUpdateQuery)
;

static float SurfaceArea(Gleam::AABBCPU& aabb)
{
	Gleam::Vector4CPU extent = aabb.getMax() - aabb.getMin();
	return 2.0f * (extent[0]*extent[1] + extent[0]*extent[2] + extent[1]*extent[2]);
}

void OcclusionManager::BVHTree::ProcessBranch(FrustumData* frustum_data, size_t node_index)
{
	const BVHNode& node = frustum_data->tree->_node_cache[node_index];

	if (frustum_data->frustum->contains(node.aabb)) {
		if (node.object) {
			frustum_data->result.emplacePush(node.object, node.user_data);

		} else {
			ProcessBranch(frustum_data, node.left);
			ProcessBranch(frustum_data, node.right);
		}
	}
}

void OcclusionManager::BVHTree::FrustumJob(void* data)
{
	FrustumData* frustum_data = reinterpret_cast<FrustumData*>(data);
	ProcessBranch(frustum_data, frustum_data->branch_root);
}


// BVH Tree
OcclusionManager::BVHTree::BVHTree(void):
	_node_cache(BVH_STARTING_CACHE, BVHNode()), _dirty_indices(BVH_STARTING_CACHE),
	_free_indices(BVH_STARTING_CACHE), _root(SIZE_T_FAIL), _is_static(false)
{
	for (size_t i = BVH_STARTING_CACHE - 1; i > 0; --i) {
		_free_indices.push(i);
	}

	_free_indices.push(0);
}

OcclusionManager::BVHTree::~BVHTree(void)
{
}

void OcclusionManager::BVHTree::setIsStatic(bool is_static)
{
	_is_static = is_static;
}

size_t OcclusionManager::BVHTree::addObject(Object* object, const UserData& user_data)
{
	size_t index = 0;

	{
		Gaff::ScopedLock<Gaff::SpinLock> fi_lock(_fi_lock);

		if (_free_indices.empty()) {
			Gaff::ScopedLock<Gaff::SpinLock> nc_lock(_nc_lock);
			growArrays();
		}

		index = _free_indices.last();
		_free_indices.pop();
	}

	AddBufferData buffer_data = {
		user_data,
		object,
		index
	};

	Gaff::ScopedLock<Gaff::SpinLock> add_lock(_add_lock);
	_add_buffer.emplacePush(buffer_data);

	return index;
}

void OcclusionManager::BVHTree::removeObject(size_t index)
{
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_remove_lock);
	_remove_buffer.push(index);
}

void OcclusionManager::BVHTree::findObjectsInFrustum(const Gleam::FrustumCPU& frustum, FrustumQueryData& out) const
{
	out.first.frustum = out.second.frustum = &frustum;
	out.first.tree = out.second.tree = this;
	out.first.counter = out.second.counter = nullptr;
	out.first.branch_root = SIZE_T_FAIL;
	out.second.branch_root = SIZE_T_FAIL;

	if (_root != SIZE_T_FAIL && frustum.contains(_node_cache[_root].aabb)) {
		out.first.branch_root = _node_cache[_root].left;
		out.second.branch_root = _node_cache[_root].right;

		// We only have one object in the tree, just return.
		if (_node_cache[_root].object) {
			out.first.result.emplacePush(_node_cache[_root].object, _node_cache[_root].user_data);
			return;
		}

		Gaff::JobData job_data[2] = {
			Gaff::JobData(&FrustumJob, &out.first),
			Gaff::JobData(&FrustumJob, &out.second)
		};

		GetApp().getJobPool().addJobs(job_data, 2, &out.first.counter);
	}
}

void OcclusionManager::BVHTree::construct(const Array<Object*>& objects, Array<OcclusionID>* id_out)
{
	// bottom-up tree construction
	// intended for static tree only
	
	if (id_out) {
		if (id_out->size() < objects.size()) {
			id_out->resize(objects.size());
		}
	}
}

void OcclusionManager::BVHTree::update(void)
{
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_remove_lock);

		for (auto it = _remove_buffer.begin(); it != _remove_buffer.end(); ++it) {
			removeObjectHelper(*it);
			_free_indices.push(*it);
		}

		_remove_buffer.clearNoFree();
	}

	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_add_lock);

		for (auto it = _add_buffer.begin(); it != _add_buffer.end(); ++it) {
			addObjectHelper(*it);
		}

		_add_buffer.clearNoFree();
	}

	// maybe occasionally rebalance the tree
}

// These are going to be called in an environment where there is no thread contention
void OcclusionManager::BVHTree::dirtyObjectCallback(Object*, uint64_t index)
{
	_node_cache[static_cast<size_t>(index)].dirty = true;
	_dirty_indices.push(static_cast<size_t>(index));
}

void OcclusionManager::BVHTree::growArrays(void)
{
	_node_cache.reserve(_node_cache.size() * 2);

	for (size_t i = _node_cache.size(); i < _node_cache.capacity(); ++i) {
		_free_indices.push(i);
	}
}

void OcclusionManager::BVHTree::addObjectHelper(const AddBufferData& data)
{
	if (!_is_static) {
		data.object->registerForLocalDirtyCallback(Gaff::Bind(this, &BVHTree::dirtyObjectCallback), data.index);
	}

	BVHNode& node = _node_cache[data.index];
	node.aabb = data.object->getWorldAABB();
	node.user_data = data.user_data;
	node.object = data.object;
	node.index = data.index;
	node.parent = node.left = node.right = SIZE_T_FAIL;
	node.dirty = false;

	// Tree is empty
	if (_root == SIZE_T_FAIL) {
		_root = data.index;
		return;
	}

	BVHNode* travel_node = &_node_cache[_root];
	Gleam::AABBCPU final_aabb;

	// Tree only has one node, set final_aabb for when we add the new parent node
	if (travel_node->left == SIZE_T_FAIL && travel_node->right == SIZE_T_FAIL) {
		final_aabb = travel_node->aabb;
		final_aabb.addAABB(node.aabb);
	}

	Gleam::AABBCPU aabb_left, aabb_right;
	float surface_left, surface_right;

	// Go down the tree until we hit a leaf node.
	// A leaf node is denoted by the Object* not being null.
	while (!travel_node->object) {
		aabb_left = aabb_right = node.aabb;

		aabb_left.addAABB(_node_cache[travel_node->left].aabb);
		aabb_right.addAABB(_node_cache[travel_node->right].aabb);

		surface_left = SurfaceArea(aabb_left);
		surface_right = SurfaceArea(aabb_right);

		if (surface_left < surface_right) {
			travel_node = &_node_cache[travel_node->left];
			final_aabb = aabb_left;
		} else {
			travel_node = &_node_cache[travel_node->right];
			final_aabb = aabb_right;
		}
	}

	// Create a new node
	if (_free_indices.empty()) {
		growArrays();
	}

	size_t new_parent_index = _free_indices.last();
	_free_indices.pop();

	// Insert the new parent node into the tree.
	BVHNode* new_parent = &_node_cache[new_parent_index];
	new_parent->aabb = final_aabb;
	new_parent->object = nullptr;
	new_parent->index = new_parent_index;
	new_parent->parent = travel_node->parent;
	new_parent->left = data.index;
	new_parent->right = travel_node->index;

	travel_node->parent = node.parent = new_parent_index;


	// Set root to the correct node if we are replacing root.
	if (new_parent->parent == SIZE_T_FAIL) {
		_root = new_parent_index;

	} else {
		BVHNode& parent = _node_cache[new_parent->parent];

		if (parent.left == travel_node->index) {
			parent.left = new_parent_index;
		} else {
			parent.right = new_parent_index;
		}
	}

	// Walk back up the tree and update the AABBs.
	updateAABBs(new_parent->parent);

	// potentially balance
}

void OcclusionManager::BVHTree::removeObjectHelper(size_t index)
{
	if (index == _root) {
		_root = SIZE_T_FAIL;
		return;
	}

	BVHNode& node = _node_cache[index];
	BVHNode& parent = _node_cache[node.parent];
	BVHNode& sibling = _node_cache[(parent.left == index) ? parent.right : parent.left];

	// If our tree only has three nodes, remove index and parent and set the remaining node as root
	if (parent.parent == SIZE_T_FAIL) {
		_root = sibling.index;
		sibling.parent = SIZE_T_FAIL;

	// Remove me and my parent, set my sibling as my grandparent's child
	} else {
		BVHNode& grand_parent = _node_cache[parent.parent];

		if (grand_parent.left == parent.index) {
			grand_parent.left = sibling.index;
		} else {
			grand_parent.right = sibling.index;
		}

		sibling.parent = grand_parent.index;

		// Walk back up the tree and update the AABBs.
		updateAABBs(grand_parent.index);

		// potentially balance
	}
}

void OcclusionManager::BVHTree::updateAABBs(size_t index)
{
	while (index != SIZE_T_FAIL) {
		BVHNode& curr_node = _node_cache[index];
		BVHNode& left = _node_cache[curr_node.left];
		BVHNode& right = _node_cache[curr_node.right];

		curr_node.aabb = left.aabb;
		curr_node.aabb.addAABB(right.aabb);

		index = curr_node.parent;
	}
}



// Occlusion Manager
OcclusionManager::OcclusionManager(void)
{
	_bvh_trees[OT_STATIC].setIsStatic(true);
}

OcclusionManager::~OcclusionManager(void)
{
}

void OcclusionManager::getUpdateEntries(Array<UpdateEntry>& entries)
{
	entries.push(UpdateEntry(U8String("Occlusion Manager: Update"), Gaff::Bind(this, &OcclusionManager::update)));
}

const char* OcclusionManager::getName(void) const
{
	return GetFriendlyName();
}

OcclusionManager::OcclusionID OcclusionManager::addObject(Object* object, OBJ_TYPE object_type, const UserData& user_data)
{
	GAFF_ASSERT(object && !_node_map.hasElementWithKey(object) && object_type < OT_SIZE);

	OcclusionID id;
	id.index = _bvh_trees[object_type].addObject(object, user_data);
	id.object_type = object_type;;

	_node_map[object] = id;

	return id;
}

void OcclusionManager::removeObject(Object* object)
{
	GAFF_ASSERT(object && _node_map.hasElementWithKey(object));
	removeObject(_node_map[object]);
}

void OcclusionManager::removeObject(OcclusionID id)
{
	GAFF_ASSERT(id.index != SIZE_T_FAIL && id.object_type < OT_SIZE);
	_bvh_trees[id.object_type].removeObject(id.index);
}

void OcclusionManager::constructStaticTree(const Array<Object*>& objects, Array<OcclusionID>* id_out)
{
	GAFF_ASSERT(!objects.empty());
	_bvh_trees[OT_STATIC].construct(objects, id_out);
}

void OcclusionManager::findObjectsInFrustum(const Gleam::FrustumCPU& frustum, OBJ_TYPE object_type, Array<QueryResult>& out) const
{
	BVHTree::FrustumQueryData data;	
	_bvh_trees[object_type].findObjectsInFrustum(frustum, data);

	if (data.first.counter) {
		GetApp().getJobPool().helpAndFreeCounter(data.first.counter);

		out = std::move(data.first.result);
		out.append(std::move(data.second.result));

	} else if (!data.first.result.empty()) {
		out = std::move(data.first.result);
	}
}

void OcclusionManager::findObjectsInFrustum(const Gleam::FrustumCPU& frustum, QueryData& out) const
{
	for (unsigned int i = 0; i < OT_SIZE; ++i) {
		findObjectsInFrustum(frustum, static_cast<OBJ_TYPE>(i), out.results[i]);
	}
}

Array<OcclusionManager::QueryResult> OcclusionManager::findObjectsInFrustum(const Gleam::FrustumCPU& frustum, OBJ_TYPE object_type) const
{
	Array<QueryResult> out;
	findObjectsInFrustum(frustum, object_type, out);
	return out;
}

OcclusionManager::QueryData OcclusionManager::findObjectsInFrustum(const Gleam::FrustumCPU& frustum) const
{
	QueryData out;
	findObjectsInFrustum(frustum, out);
	return out;
}

void OcclusionManager::update(double, void*)
{
	for (unsigned int i = 0; i < OT_SIZE; ++i) {
		_bvh_trees[i].update();
	}
}

NS_END