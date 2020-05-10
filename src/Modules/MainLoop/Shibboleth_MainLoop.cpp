/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_MainLoop.h"
#include <Shibboleth_LogManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_ISystem.h>
#include <Shibboleth_IApp.h>
#include <EAThread/eathread.h>

#include <Shibboleth_RenderManagerBase.h>
#include <Shibboleth_ResourceManager.h>
#include <Shibboleth_SamplerStateResource.h>
#include <Shibboleth_RasterStateResource.h>
#include <Shibboleth_MaterialResource.h>
#include <Shibboleth_ECSSceneResource.h>
#include <Shibboleth_TextureResource.h>
#include <Shibboleth_ModelResource.h>
#include <Gleam_IShaderResourceView.h>
#include <Gleam_IRenderDevice.h>
#include <Gleam_IRenderTarget.h>
#include <Gleam_IncludeMatrix.h>

SHIB_REFLECTION_DEFINE_BEGIN(MainLoop)
	.BASE(IMainLoop)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(MainLoop)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(MainLoop)

static void UpdateSystemJob(uintptr_t thread_id_int, void* data)
{
	ISystem* const system = reinterpret_cast<ISystem*>(data);
	system->update(thread_id_int);
}

bool MainLoop::init(void)
{
	IApp& app = GetApp();

	_render_mgr = &app.GETMANAGERT(IRenderManager, RenderManager);
	_job_pool = &app.getJobPool();

	const auto* const systems = app.getReflectionManager().getTypeBucket(CLASS_HASH(ISystem));

	if (!systems || systems->empty()) {
		return true;
	}

	ProxyAllocator allocator("MainLoop");
	SerializeReaderWrapper readerWrapper(allocator);

	if (!OpenJSONOrMPackFile(readerWrapper, "cfg/update_phases.cfg")) {
		LogErrorDefault("Failed to read cfg/update_phases.cfg[.bin].");
		return false;
	}

	const Gaff::ISerializeReader& reader = *readerWrapper.getReader();

	if (!reader.isArray()) {
		LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Root is not a 3-dimensional array of strings.");
		return false;
	}

	ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	const auto* bucket = refl_mgr.getTypeBucket(CLASS_HASH(ISystem));

	_blocks.resize(reader.size());

	//_systems.resize(reader.size());
	//_job_data.resize(reader.size());

	for (int32_t i = 0; i < reader.size(); ++i) {
		const auto guard_1 = reader.enterElementGuard(i);

		if (!reader.isArray()) {
			LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Config is not a 3-dimensional array of strings.");
			return false;
		}

		UpdateBlock& block = _blocks[i];
		block.rows.resize(reader.size());

		for (int32_t j = 0; j < reader.size(); ++j) {
			const auto guard_2 = reader.enterElementGuard(j);

			if (!reader.isArray()) {
				LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Config is not a 3-dimensional array of strings.");
				return false;
			}

			UpdateRow& row = block.rows[j];
			row.job_data.resize(reader.size());
			row.systems.resize(reader.size());

			for (int32_t k = 0; k < reader.size(); ++k) {
				const auto guard_3 = reader.enterElementGuard(k);

				if (!reader.isString()) {
					LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Config is not a 3-dimensional array of strings.");
					return false;
				}

				const char* system_name = reader.readString();
				const Gaff::Hash64 hash = Gaff::FNV1aHash64String(system_name);

				const auto it = eastl::lower_bound(bucket->begin(), bucket->end(), hash, ReflectionManager::CompareRefHash);

				if (it == bucket->end() || hash != (*it)->getReflectionInstance().getHash()) {
					LogErrorDefault("MainLoop: Could not find system '%s'.", system_name);
					reader.freeString(system_name);
					return false;
				}

				ISystem* const system = (*it)->createT<ISystem>(CLASS_HASH(ISystem), allocator);

				if (!system) {
					LogErrorDefault("MainLoop: Failed to create system '%s'", system_name);
					reader.freeString(system_name);
					return false;
				}

				if (!system->init()) {
					LogErrorDefault("MainLoop: Failed to initialize system '%s'", system_name);
					reader.freeString(system_name);
					return false;
				}

				reader.freeString(system_name);

				row.systems[k].reset(system);

				row.job_data[k].job_func = UpdateSystemJob;
				row.job_data[k].job_data = system;
			}
		}
	}

	return true;
}

void MainLoop::destroy(void)
{
	_blocks.clear();
}

void MainLoop::update(void)
{
	// This has to happen in the main thread.
	_render_mgr->updateWindows();

	const int32_t num_blocks = static_cast<int32_t>(_blocks.size());

	for (int32_t i = 0; i < num_blocks; ++i) {
		UpdateBlock& block = _blocks[i];
		const int32_t job_counter = block.counter;

		// This block is already active, leave it alone.
		if (job_counter > 0) {
			continue;
		}

		// We finished our jobs, increment the current row.
		if (job_counter == 0) {
			++block.curr_row;

			// We finished our block, increment the frame count and reset current row.
			if (block.curr_row >= static_cast<int32_t>(block.rows.size())) {
				block.frame = (block.frame + 1) % 3;
				block.curr_row = -1;
			}
		}

		// We're waiting for another block.
		if (block.curr_row < 0) {
			// Check previous block.
			if (i > 0) {
				// If we're on the same frame, then it hasn't given us anything to act on. Wait for it to finish.
				if (block.frame == _blocks[i - 1].frame) {
					block.counter = -1;
					continue;
				}
			}

			// Check next block.
			if (i < (num_blocks - 1)) {
				const int32_t next_block_curr_frame = _blocks[i + 1].frame;
				const int32_t next_block_next_frame = (_blocks[i + 1].frame + 1) % 3;

				// We're getting too far ahead of the next section. Wait for it to finish.
				if (block.frame != next_block_curr_frame && block.frame != next_block_next_frame) {
					block.counter = -1;
					continue;
				}
			}

			block.curr_row = 0;
		}

		// Process the next row.
		block.counter = 0;

		_job_pool->addJobs(
			block.rows[block.curr_row].job_data.data(),
			static_cast<int32_t>(block.rows[block.curr_row].job_data.size()),
			block.counter
		);
	}

	// Help out a little before the next iteration.
	_job_pool->doAJob(_job_pool->getMainThreadID());

	// Give some time to other threads.
	EA::Thread::ThreadSleep();
}

NS_END
