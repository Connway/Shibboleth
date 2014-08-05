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
#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_App.h>
#include <Gaff_JSON.h>

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

void UpdateManager::update(double dt)
{
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

void UpdateManager::allManagersCreated(void)
{
	// read JSON file
	// get all update entries from app
	// add update entries

	Array<IUpdateQuery::UpdateEntry> entries;

	_app.forEachManager([&](IManager& manager) -> bool
	{
		IUpdateQuery* update_query = manager.requestInterface<IUpdateQuery>();

		if (update_query) {
			update_query->requestUpdateEntries(entries);
		}

		return false;
	});

	Gaff::JSON table;

	if (table.parseFile("./update_entries.json")) {
		assert(table.isArray());

		table.forEachInArray([&](size_t index, const Gaff::JSON& value) -> bool
		{
			assert(value.isArray());

			table.forEachInArray([&](size_t, const Gaff::JSON& value) -> bool
			{
				assert(value.isString());

				auto it = entries.linearSearch(value.getString(), [&](const IUpdateQuery::UpdateEntry& lhs, const char* rhs) -> bool
				{
					return lhs.first == rhs;
				});

				assert(it != entries.end());

				_table[index].push(it->second);

				return false;
			});

			return false;
		});
	}
}

NS_END
