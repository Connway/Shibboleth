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

static void UpdateSystemJob(void* data)
{
	ISystem* const system = reinterpret_cast<ISystem*>(data);
	system->update();
}

bool MainLoop::init(void)
{
	IApp& app = GetApp();

	_render_mgr = &app.GETMANAGERT(IRenderManager, RenderManager);

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

	_systems.set_allocator(allocator);
	_systems.resize(reader.size());

	_job_data.set_allocator(allocator);
	_job_data.resize(reader.size());

	for (int32_t i = 0; i < reader.size(); ++i) {
		const auto guard_1 = reader.enterElementGuard(i);

		if (!reader.isArray()) {
			LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Config is not a 3-dimensional array of strings.");
			return false;
		}

		_systems[i].set_allocator(allocator);
		_systems[i].resize(reader.size());

		_job_data[i].set_allocator(allocator);
		_job_data[i].resize(reader.size());

		for (int32_t j = 0; j < reader.size(); ++j) {
			const auto guard_2 = reader.enterElementGuard(j);

			if (!reader.isArray()) {
				LogErrorDefault("MainLoop: Malformed cfg/update_phases.cfg[.bin]. Config is not a 3-dimensional array of strings.");
				return false;
			}

			_systems[i][j].set_allocator(allocator);
			_systems[i][j].resize(reader.size());

			_job_data[i][j].set_allocator(allocator);
			_job_data[i][j].resize(reader.size());

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
				_systems[i][j][k].reset(system);

				_job_data[i][j][k].job_func = UpdateSystemJob;
				_job_data[i][j][k].job_data = system;
			}
		}
	}

	return true;
}

void MainLoop::destroy(void)
{
}

void MainLoop::update(void)
{
	_render_mgr->updateWindows(); // This has to happen in the main thread.

	if (_counter) {
		GetApp().getJobPool().doAJob();

	// Jobs are done. Move up to the next section.
	} else {
		++_system_group;

		while (_system_group >= static_cast<int32_t>(_systems[_update_block].size())) {
			_system_group = 0;
			++_update_block;

			if (_update_block >= static_cast<int32_t>(_systems.size())) {
				_update_block = 0;
			}
		}

		const auto& job_data = _job_data[_update_block][_system_group];
		GetApp().getJobPool().addJobs(job_data.data(), job_data.size(), _counter);
	}

	// Give some time to other threads.
	EA::Thread::ThreadSleep();
}

NS_END
