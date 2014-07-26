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
	_app(app), _next_id(0)
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
	// Do we want to scrub row value down if it's greater than _table.size()?
	//if (row >= _table.size()) {
	//	row = _table.size();
	//}

	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_reg_queue_lock);

	UpdateID id = { row, _next_id };
	++_next_id;

	_register_queue.push(Gaff::Pair<unsigned int, RowEntry>(row, RowEntry(id.id, callback)));

	return id;
}

void UpdateManager::unregisterForUpdate(const UpdateID& id)
{
	assert(id.row < _table.size() && id.id < _next_id);
	Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_unreg_queue_lock);
	_unregister_queue.push(id);
}

void UpdateManager::update(double dt)
{
	// Handle unregistration
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_unreg_queue_lock);
		for (auto it_unreg = _unregister_queue.begin(); it_unreg != _unregister_queue.end(); ++it_unreg) {
			unregisterHelper(*it_unreg);
		}

		// Might want to add code that will resize if the capacity is too large
		_unregister_queue.clearNoFree();
	}

	// Handle registration
	{
		Gaff::ScopedLock<Gaff::SpinLock> scoped_lock(_reg_queue_lock);
		for (auto it_reg = _register_queue.begin(); it_reg != _register_queue.end(); ++it_reg) {
			registerHelper(*it_reg);
		}

		// Might want to add code that will resize if the capacity is too large
		_register_queue.clearNoFree();
	}

	for (auto it_row = _table.begin(); it_row != _table.end(); ++it_row) {
		for (auto it_entry = it_row->begin(); it_entry != it_row->end(); ++it_entry) {
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

void UpdateManager::registerHelper(const RegisterEntry& entry)
{
	if (_table.size() < entry.first) {
		_table.resize(entry.first);
	}

	UpdateRow& row = _table[entry.first];
	row.push(entry.second);
}

void UpdateManager::unregisterHelper(const UpdateID& id)
{
	UpdateRow& row = _table[id.row];

	// Rows should be sorted by id, so it should be safe to use a binary search
	auto it = row.binarySearch(id.id, [](const RowEntry& lhs, unsigned int rhs) -> bool
	{
		return lhs.first < rhs;
	});

	if (it != row.end() && it->first == id.id) {
		row.erase(it);
	}
}

NS_END
