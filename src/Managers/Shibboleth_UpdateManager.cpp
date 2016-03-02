/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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
#include "Shibboleth_FrameManager.h"
#include "Shibboleth_IUpdateQuery.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_JobPool.h>
#include <Shibboleth_App.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

REF_IMPL_REQ(UpdateManager);
SHIB_REF_IMPL(UpdateManager)
.addBaseClassInterfaceOnly<UpdateManager>()
;

UpdateManager::UpdatePhase::UpdatePhase(void):
	_counter(0), _frame_mgr(GetApp().getManagerT<FrameManager>("Frame Manager"))
{
}

UpdateManager::UpdatePhase::~UpdatePhase(void)
{
}

const char* UpdateManager::UpdatePhase::getName(void) const
{
	return _name.getBuffer();
}

void UpdateManager::UpdatePhase::setName(const char* name)
{
	_name = name;
}

void UpdateManager::UpdatePhase::addUpdate(size_t row, const UpdateCallback& callback)
{
	_callbacks[row].emplacePush(callback);
}

void UpdateManager::UpdatePhase::setNumRows(size_t num_rows)
{
	_callbacks.resize(num_rows);
}

size_t UpdateManager::UpdatePhase::getID(void) const
{
	return _id;
}

void UpdateManager::UpdatePhase::setID(size_t id)
{
	_id = id;
}

bool UpdateManager::UpdatePhase::isDone(void) const
{
	return _done_lock.tryLock();
}

void UpdateManager::UpdatePhase::run(void)
{
	// We shouldn't be calling run() if we are already updating.
	GAFF_ASSERT(!_counter);

	void* frame_data = _frame_mgr.getNextFrameData(_id);

	if (!frame_data) {
		_done_lock.unlock();
		return;
	}

	_timer.stop();
	_timer.start();

	ProcessRow(this, 0, frame_data);
}

void UpdateManager::UpdatePhase::ProcessRow(UpdatePhase* phase, size_t row, void* frame_data)
{
	JobPool& job_pool = GetApp().getJobPool();
	auto& cbs = phase->_callbacks[row];
	phase->_data_cache.clearNoFree();
	phase->_jobs.clearNoFree();
	
	phase->_data_cache.reserve(cbs.size());
	phase->_jobs.reserve(cbs.size());

	for (size_t i = 0; i < cbs.size(); ++i) {
		phase->_data_cache.emplacePush();
		phase->_data_cache[i].phase = phase;
		phase->_data_cache[i].frame_data = frame_data;
		phase->_data_cache[i].row = row;
		phase->_data_cache[i].cb_index = i;

		phase->_jobs.emplacePush(&UpdateJob, &phase->_data_cache[i]);
	}

	phase->_counter = static_cast<unsigned int>(cbs.size());
	job_pool.addJobs(phase->_jobs.getArray(), phase->_jobs.size());
}

void UpdateManager::UpdatePhase::UpdateJob(void* data)
{
	UpdateData* update_data = reinterpret_cast<UpdateData*>(data);
	UpdatePhase* phase = update_data->phase;
	size_t row = update_data->row;

	phase->_callbacks[row][update_data->cb_index](phase->_timer.getDeltaSec(), update_data->frame_data);

	unsigned int new_val = AtomicDecrement(&phase->_counter);
	GAFF_ASSERT(new_val < phase->_callbacks[row].size()); // Double check we don't underflow.

	if (!new_val) {
		++row;

		if (row < phase->_callbacks.size()) {
			ProcessRow(phase, row, update_data->frame_data);

		} else {
			phase->_frame_mgr.finishFrame(update_data->phase->_id);
			phase->_done_lock.unlock();
		}
	}
}



UpdateManager::UpdateManager(void)
{
}

UpdateManager::~UpdateManager(void)
{
}

const char* UpdateManager::getName(void) const
{
	return "Update Manager";
}

void UpdateManager::update(void)
{
	for (auto it = _phases.begin(); it != _phases.end(); ++it) {
		if (it->isDone()) {
			it->run();
		}
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

	IFile* updates_file = GetApp().getFileSystem()->openFile("update_entries.json");

	if (!updates_file) {
		log.first.writeString("ERROR - Could not open file \"update_entries.json\".\n");
		GetApp().quit();
		return;
	}

	Gaff::JSON phases;
	bool success = phases.parse(updates_file->getBuffer());

	GetApp().getFileSystem()->closeFile(updates_file);

	if (success) {
		if (!phases.isArray()) {
			log.first.writeString("ERROR - Root of \"update_entries.json\" is not an array.\n");
			GetApp().quit();
			return;
		}

		GetApp().getManagerT<FrameManager>("Frame Manager").setNumPhases(phases.size());
		_phases.resize(phases.size());

		// Process each phase
		phases.forEachInArray([&](size_t index_phase, const Gaff::JSON& phase) -> bool
		{
			if (!phase.isObject()) {
				log.first.writeString("ERROR - A phase in \"update_entries.json\" is not an object.\n");
				GetApp().quit();
				return true;
			}

			Gaff::JSON phase_name = phase["name"];
			Gaff::JSON phase_entries = phase["entries"];

			if (!phase_name.isString()) {
				unsigned int i = static_cast<unsigned int>(index_phase); // For 64-bit builds where size_t is not 32-bits.
				log.first.printf("ERROR - The phase in \"update_entries.json\" at index %d does not have a name or name is not a string.\n", i);
				GetApp().quit();
				return true;
			}

			if (!phase_entries.isArray()) {
				log.first.printf("ERROR - The 'entries' element in phase '%s' in \"update_entries.json\" is not an array.\n", phase_name.getString());
				GetApp().quit();
				return true;
			}

			_phases[index_phase].setName(phase_name.getString());
			_phases[index_phase].setNumRows(phase_entries.size());
			_phases[index_phase].setID(index_phase);

			// Process each phase's entries
			success = !phase_entries.forEachInArray([&](size_t index_entries, const Gaff::JSON& row) -> bool
			{
				if (!row.isArray()) {
					log.first.printf("ERROR - A row in phase '%s' in \"update_entries.json\" is not an array.\n", phase_name.getString());
					GetApp().quit();
					return true;
				}

				// Process each row
				success = !row.forEachInArray([&](size_t, const Gaff::JSON& entry) -> bool
				{
					if (!entry.isString()) {
						unsigned int i = static_cast<unsigned int>(index_entries); // For 64-bit builds where size_t is not 32-bits.
						log.first.printf("ERROR - An entry in row %d in phase '%s' in \"update_entries.json\" is not a string.\n", i, phase_name.getString());
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

					_phases[index_phase].addUpdate(index_entries, it->second);
					return false;
				});

				return !success;
			});

			return !success;
		});

	} else {
		log.first.writeString("ERROR - Failed to parse file \"update_entries.json\".\n");
		GetApp().quit();
	}
}

NS_END
