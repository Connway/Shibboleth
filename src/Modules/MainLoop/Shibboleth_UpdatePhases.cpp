/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_UpdatePhases.h"
#include <Log/Shibboleth_LogManager.h>
#include <Config/Shibboleth_Config.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::UpdateRow)
	.template ctor<>()

	.var("systems", &Shibboleth::UpdateRow::systems, Shibboleth::InstancedOptionalAttribute{ u8"!", Shibboleth::InstancedOptionalAttribute::Flag::StripPrefix })
SHIB_REFLECTION_DEFINE_END(Shibboleth::UpdateRow)



SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::UpdateBlock)
	.template ctor<>()

	.var("update_rows", &Shibboleth::UpdateBlock::update_rows)
SHIB_REFLECTION_DEFINE_END(Shibboleth::UpdateBlock)



SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::UpdatePhases)
	.classAttrs(
		Shibboleth::ConfigFileAttribute(u8"update_phases"),
		Shibboleth::InitFromConfigAttribute()
	)

	.var("update_blocks", &Shibboleth::UpdatePhases::update_blocks)
SHIB_REFLECTION_DEFINE_END(Shibboleth::UpdatePhases)



namespace
{
	static void UpdateSystemJob(uintptr_t thread_id_int, void* data)
	{
		Shibboleth::ISystem* const system = reinterpret_cast<Shibboleth::ISystem*>(data);
		system->update(thread_id_int);
	}
}

NS_SHIBBOLETH

UpdateBlock::UpdateBlock(UpdateBlock&& block):
	update_rows(std::move(block.update_rows)),
	counter(static_cast<int32_t>(block.counter)), // Atomics aren't moveable. Just copy for now.
	curr_row(block.curr_row),
	frame(block.frame)
{
}

UpdateBlock& UpdateBlock::operator=(UpdateBlock&& rhs)
{
	update_rows = std::move(rhs.update_rows);
	counter = static_cast<int32_t>(rhs.counter); // Atomics aren't moveable. Just copy for now.
	curr_row = rhs.curr_row;
	frame = rhs.frame;

	return *this;
}



Error UpdatePhases::init(void)
{
	_job_pool = &GetApp().getJobPool();

	const auto& update_phases_ref_def = Refl::Reflection<UpdatePhases>::GetReflectionDefinition();
	const InitFromConfigAttribute* const init_attr = update_phases_ref_def.getClassAttr<InitFromConfigAttribute>();
	GAFF_ASSERT(init_attr);

	if (const Error result = init_attr->loadConfig(this, update_phases_ref_def)) {
		// $TODO: Log error.
		return result;
	}

	for (UpdateBlock& block : update_blocks) {
		for (UpdateRow& row : block.update_rows) {
			row.job_data.reserve(row.systems.size());

			for (ISystem& system : row.systems) {
				row.job_data.emplace_back(Gaff::JobData{ UpdateSystemJob, &system });

				if (!system.init()) {
					LogErrorDefault("MainLoop: Failed to initialize system '%s'", system.getReflectionDefinition().getReflectionInstance().getName());
					return Error::k_fatal_error;
				}
			}
		}
	}

	return Error::k_no_error;
}

void UpdatePhases::update(void)
{
	const int32_t num_blocks = static_cast<int32_t>(update_blocks.size());

	for (int32_t i = 0; i < num_blocks; ++i) {
		UpdateBlock& block = update_blocks[i];
		const int32_t job_counter = block.counter;

		// This block is already active, leave it alone.
		if (job_counter > 0) {
			continue;
		}

		// We finished our jobs, increment the current row.
		if (job_counter == 0) {
			++block.curr_row;

			// We finished our block, increment the frame count and reset current row.
			if (block.curr_row >= static_cast<int32_t>(block.update_rows.size())) {
				block.frame = (block.frame + 1) % 3;
				block.curr_row = -1;
			}
		}

		// We're waiting for another block.
		if (block.curr_row < 0) {
			// Check previous block.
			if (i > 0) {
				// If we're on the same frame, then it hasn't given us anything to act on. Wait for it to finish.
				if (block.frame == update_blocks[static_cast<size_t>(i - 1)].frame) {
					block.counter = -1;
					continue;
				}
			}

			// Check next block.
			if (i < (num_blocks - 1)) {
				const int32_t next_block_curr_frame = update_blocks[static_cast<size_t>(i + 1)].frame;
				const int32_t next_block_next_frame = (update_blocks[static_cast<size_t>(i + 1)].frame + 1) % 3;

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
			block.update_rows[block.curr_row].job_data.data(),
			static_cast<int32_t>(block.update_rows[block.curr_row].job_data.size()),
			block.counter
		);
	}

	// Help out a little before the next iteration.
	_job_pool->help(_job_pool->getMainThreadID());

	// Give some time to other threads.
	EA::Thread::ThreadSleep();
}

void UpdatePhases::clear(void)
{
	update_blocks.clear();
}

NS_END
