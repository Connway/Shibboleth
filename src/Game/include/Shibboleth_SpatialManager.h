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
#include <Shibboleth_IManager.h>
#include <Shibboleth_Watcher.h>
#include <Shibboleth_Array.h>
#include <Shibboleth_Map.h>
#include <Gleam_Quaternion.h>
#include <Gleam_AABB.h>

NS_SHIBBOLETH

class Object;
class IApp;

class SpatialManager : public IManager, public IUpdateQuery
{
public:
	SpatialManager(IApp& app);
	~SpatialManager(void);

	void requestUpdateEntries(Array<UpdateEntry>& entries);
	void* rawRequestInterface(unsigned int class_id) const;
	const char* getName(void) const;

	unsigned int addObject(Object* object);
	void removeObject(unsigned int id);

	void update(double);

private:
	struct Node
	{
		// other data for pos, aabb, and children nodes

		Gaff::WatchReceipt receipts[3];
		Object* object;
		unsigned int id;

		// The lock is in the node instead of the watcher because SpinLock has no copy or assignment.
		Gaff::SpinLock _lock;
		bool dirty;
	};

	class WatchUpdater
	{
	public:
		WatchUpdater(SpatialManager* spatial_mgr, Node* node);
		~WatchUpdater(void);

		// Need const otherwise get funky compiler errors
		void operator()(const Gleam::Quaternion&) const;
		void operator()(const Gleam::AABB&) const;
		void operator()(const Gleam::Vec4&) const;

	private:
		SpatialManager* _spatial_mgr;
		Node* _node;

		void addDirtyNode(void) const;
	};

	Map<unsigned int, Node*> _node_map;
	Array<Node*> _dirty_nodes;
	Node* _bvh;

	IApp& _app;

	Gaff::SpinLock _dirty_lock;
	Gaff::SpinLock _bvh_lock;

	unsigned int _next_id;

	void removeNode(Node* node);
	void insertNode(Node* node);

	friend class WatchUpdater;

	GAFF_NO_COPY(SpatialManager);

	REF_DEF_SHIB(SpatialManager);
};

NS_END
