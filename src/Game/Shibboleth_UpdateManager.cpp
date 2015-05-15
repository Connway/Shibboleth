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
	_counter(nullptr), _frame_mgr(GetApp().getManagerT<FrameManager>("Frame Manager"))
{
}

UpdateManager::UpdatePhase::~UpdatePhase(void)
{
	if (_counter) {
		GetApp().getJobPool().freeCounter(_counter);
	}
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
	return !_counter || !_counter->count;
}

void UpdateManager::UpdatePhase::run(void)
{
	assert(isDone()); // We shouldn't be calling run() if we are already updating.
	Gaff::JobData job(&UpdatePhase::UpdatePhaseJob, this);
	GetApp().getJobPool().addJobs(&job);
}

void UpdateManager::UpdatePhase::UpdatePhaseJob(void* data)
{
	UpdatePhase* phase = reinterpret_cast<UpdatePhase*>(data);

	phase->_timer.stop();
	phase->_timer.start();

	JobPool& job_pool = GetApp().getJobPool();
	double dt = phase->_timer.getDeltaSec();

	// Go through each row of the phase and add their updates
	for (size_t i = 0; i < phase->_callbacks.size(); ++i) {
		phase->_data_cache.clearNoFree();
		phase->_jobs.clearNoFree();
		phase->_data_cache.reserve(phase->_callbacks[i].size());
		phase->_jobs.reserve(phase->_callbacks[i].size());

		for (size_t j = 0; j < phase->_callbacks[i].size(); ++j) {
			phase->_data_cache.emplacePush();
			phase->_data_cache[j].callback = &phase->_callbacks[i][j];
			phase->_data_cache[j].delta_time = dt;
			phase->_data_cache[j].phase_id = phase->_id;

			void* frame_data = phase->_frame_mgr.getNextFrameData(phase->_id);

			// If there are no frames free, help out until there are.
			while (!frame_data) {
				job_pool.doAJob();
				frame_data = phase->_frame_mgr.getNextFrameData(phase->_id);
			}

			phase->_data_cache[j].frame_data = frame_data;

			phase->_jobs.emplacePush(&UpdateJob, &phase->_data_cache[j]);
		}

		job_pool.addJobs(phase->_jobs.getArray(), phase->_jobs.size(), &phase->_counter);

		// We want to help while waiting, since we don't just want to consume a potential job thread with just waiting.
		job_pool.helpWhileWaiting(phase->_counter);
	}

	phase->_frame_mgr.finishFrame(phase->_id);
}

void UpdateManager::UpdatePhase::UpdateJob(void* data)
{
	UpdateData* update_data = reinterpret_cast<UpdateData*>(data);
	(*update_data->callback)(update_data->delta_time, update_data->frame_data);
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
