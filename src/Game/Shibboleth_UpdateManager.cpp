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

#include "Shibboleth_UpdateManager.h"
#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_App.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

static void UpdateJob(void* data)
{
	UpdateData* update_data = reinterpret_cast<UpdateData*>(data);
	update_data->callback(update_data->dt);
}

REF_IMPL_REQ(UpdateManager);
SHIB_REF_IMPL(UpdateManager)
.addBaseClassInterfaceOnly<UpdateManager>()
;

UpdateManager::UpdateManager(void):
	_counter(nullptr)
{
}

UpdateManager::~UpdateManager(void)
{
	if (_counter) {
		GetApp().getJobPool().waitForAndFreeCounter(_counter);
	}
}

const char* UpdateManager::getName(void) const
{
	return "Update Manager";
}

void UpdateManager::update(double dt)
{
	for (auto it_row = _table.begin(); it_row != _table.end(); ++it_row) {
		_job_data.clearNoFree();
		_update_data.clearNoFree();
		_job_data.reserve(it_row->size());
		_update_data.reserve(it_row->size());

		for (auto it_entry = it_row->begin(); it_entry != it_row->end(); ++it_entry) {
			// add update callback to job queue
			_update_data.emplacePush(*it_entry, dt);
			_job_data.emplacePush(&UpdateJob, &_update_data.last());

		}

		GetApp().getJobPool().addJobs(_job_data.getArray(), _job_data.size(), &_counter);
		GetApp().getJobPool().waitForCounter(_counter);
	}
}

void UpdateManager::allManagersCreated(void)
{
	LogManager::FileLockPair& log = GetApp().getGameLogFile();
	Array<IUpdateQuery::UpdateEntry> entries;

	unsigned int update_query_hash = CLASS_HASH(IUpdateQuery);

	reinterpret_cast<App&>(GetApp()).forEachManager([&](IManager& manager) -> bool
	{
		IUpdateQuery* update_query = manager.requestInterface<IUpdateQuery>(update_query_hash);

		if (update_query) {
			update_query->getUpdateEntries(entries);
		}

		return false;
	});

	Gaff::JSON table;

	if (table.parseFile("./update_entries.json")) {
		if (!table.isArray()) {
			log.first.writeString("ERROR - Root of \"update_entries.json\" is not an array.\n");
			GetApp().quit();
			return;
		}

		table.forEachInArray([&](size_t index, const Gaff::JSON& row) -> bool
		{
			if (!row.isArray()) {
				log.first.writeString("ERROR - A row in \"update_entries.json\" is not an array.\n");
				GetApp().quit();
				return false;
			}

			_table.push(Array<UpdateCallback>());

			row.forEachInArray([&](size_t, const Gaff::JSON& entry) -> bool
			{
				if (!entry.isString()) {
					log.first.writeString("ERROR - An entry in \"update_entries.json\" is not a string.\n");
					GetApp().quit();
					return true;
				}

				auto it = entries.linearSearch(entry.getString(), [&](const IUpdateQuery::UpdateEntry& lhs, const char* rhs) -> bool
				{
					return lhs.first == rhs;
				});

				if (it == entries.end()) {
					log.first.printf("ERROR - UpdateEntry for '%s' in \"update_entries.json\" was not found.\n", entry.getString());
					GetApp().quit();
					return true;
				}

				_table[index].push(it->second);

				return false;
			});

			return false;
		});

	} else {
		log.first.writeString("ERROR - Failed to find/parse file \"update_entries.json\".\n");
		GetApp().quit();
	}
}

NS_END
