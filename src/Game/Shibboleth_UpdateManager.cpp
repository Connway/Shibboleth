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

#include "Shibboleth_UpdateManager.h"
#include <Shibboleth_App.h>

NS_SHIBBOLETH

UpdateManager::UpdateManager(App& app):
	_app(app)
{
}

UpdateManager::~UpdateManager(void)
{
}

const char* UpdateManager::getName(void) const
{
	return "Update Manager";
}

UpdateManager::UpdateID UpdateManager::registerForUpdate(const UpdateCallback& callback, unsigned int row)
{
	assert(row < _table.size());
	UpdateRow& row_ref = _table[row];

	UpdateID id = { row, row_ref.next_id };
	++row_ref.next_id;

	row_ref.callbacks.push(RowEntry(id.id, callback));

	return id;
}

void UpdateManager::unregisterForUpdate(const UpdateID& id)
{
	assert(id.row < _table.size() && id.id < _table[id.row].next_id);
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_unreg_queue_lock);
	_unregister_queue.push(id);
}

void UpdateManager::update(double dt)
{
	// Handle unregistration
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_unreg_queue_lock);
		for (auto it_unreq = _unregister_queue.begin(); it_unreq != _unregister_queue.end(); ++it_unreq) {
			unregister(*it_unreq);
		}

		// Might want to add code that will resize if the capacity is too large
		_unregister_queue.clearNoFree();
	}

	for (auto it_row = _table.begin(); it_row != _table.end(); ++it_row) {
		for (auto it_entry = it_row->callbacks.begin(); it_entry != it_row->callbacks.end(); ++it_entry) {
			// add update callback to job queue
			Gaff::TaskPtr<ProxyAllocator> task(_app.getAllocator().template allocT<UpdateTask>(*it_entry, dt));
			_tasks_cache.push(task);

			_app.addTask(task);
		}

		// wait for the row of tasks to finish before proceeding onto the next row
		for (auto it_task = _tasks_cache.begin(); it_task != _tasks_cache.end(); ++it_task) {
			(*it_task)->spinWait();
		}

		_tasks_cache.clearNoFree(); // To avoid a ton of allocations/deallocations
	}
}

void UpdateManager::unregister(const UpdateID& id)
{
	assert(id.row < _table.size() && id.id < _table[id.row].next_id);
	UpdateRow& row = _table[id.row];

	auto it = row.callbacks.binarySearch(id.id, [](const RowEntry& lhs, unsigned int rhs) -> bool
	{
		return lhs.first < rhs;
	});

	if (it != row.callbacks.end() && it->first == id.id) {
		row.callbacks.erase(it);
	}
}

NS_END
