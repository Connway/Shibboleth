/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_ECSQuery.h"
#include "Shibboleth_ECSArchetype.h"
#include <Shibboleth_ReflectionInterfaces.h>

NS_SHIBBOLETH

ECSQuery::ECSQuery(const ProxyAllocator& allocator):
	_shared_components(allocator),
	_components(allocator),
	_callbacks(allocator),
	_entity_data(allocator)
{
}

void ECSQuery::addShared(const Gaff::IReflectionDefinition& ref_def, SharedPushToListFunc&& push_func, SharedEraseFromListFunc&& erase_func, FilterFunc&& filter_func)
{
	_shared_components.emplace_back(QueryDataShared{ &ref_def, std::move(push_func), std::move(erase_func), std::move(filter_func) });
}

void ECSQuery::addShared(const Gaff::IReflectionDefinition& ref_def, SharedPushToListFunc&& push_func, SharedEraseFromListFunc&& erase_func)
{
	_shared_components.emplace_back(QueryDataShared{ &ref_def, std::move(push_func), std::move(erase_func), nullptr });
}

void ECSQuery::addShared(const Gaff::IReflectionDefinition& ref_def)
{
	_shared_components.emplace_back(QueryDataShared{ &ref_def, nullptr, nullptr, nullptr });
}

void ECSQuery::add(const Gaff::IReflectionDefinition& ref_def, Output& output)
{
	_components.emplace_back(QueryData{ &ref_def, &output });
}

void ECSQuery::add(const Gaff::IReflectionDefinition& ref_def)
{
	_components.emplace_back(QueryData{ &ref_def, nullptr });
}

void ECSQuery::addArchetypeCallbacks(eastl::function<void (void)>&& added_callback, eastl::function<void (int32_t)>&& removed_callback)
{
	_callbacks.emplace_back(Callbacks{ std::move(added_callback), std::move(removed_callback) });
}

void ECSQuery::removeArchetype(const void* entity_data)
{
	for (int32_t i = 0; i < static_cast<int32_t>(_entity_data.size()); ++i) {
		if (entity_data == _entity_data[i]) {
			_entity_data.erase(_entity_data.begin() + i);
			
			for (const Callbacks& callbacks : _callbacks) {
				if (callbacks.remove) {
					callbacks.remove(i);
				}
			}

			for (const auto& shared_data : _shared_components) {
				shared_data.erase_func(i);
			}

			for (const auto& data : _components) {
				data.output->erase(data.output->begin() + i);
			}

			break;
		}
	}
}

bool ECSQuery::filter(const ECSArchetype& archetype, void* entity_data)
{
	const void* shared_data = archetype.getSharedData();
	const int32_t shared_size = archetype.sharedSize();

	if (!_shared_components.empty()) {
		// If we're checking for shared data that is not a tag, we should have a shared data buffer.
		if (shared_size > 0 && !shared_data) {
			return false;
		}
	}

	for (const QueryDataShared& data : _shared_components) {
		const int32_t offset = archetype.getComponentSharedOffset(data.ref_def->getReflectionInstance().getHash());

		if (offset == -1) {
			return false;
		}

		if (data.filter_func && !data.filter_func(reinterpret_cast<const int8_t*>(shared_data) + offset)) {
			return false;
		}
	}

	for (const QueryData& data : _components) {
		const int32_t offset = archetype.getComponentOffset(data.ref_def->getReflectionInstance().getHash());

		if (offset == -1) {
			return false;
		}
	}

	// Now that we know this archetype passes the filter, add to the outputs.
	for (QueryDataShared& data : _shared_components) {
		const int32_t offset = archetype.getComponentSharedOffset(data.ref_def->getReflectionInstance().getHash());

		if (data.push_func) {
			data.push_func(reinterpret_cast<const int8_t*>(shared_data) + offset);
		}
	}

	for (const QueryData& data : _components) {
		const int32_t offset = archetype.getComponentOffset(data.ref_def->getReflectionInstance().getHash());

		if (data.output) {
			data.output->emplace_back(ECSQueryResult{ offset, entity_data });
		}
	}

	for (const Callbacks& callbacks : _callbacks) {
		if (callbacks.add) {
			callbacks.add();
		}
	}

	_entity_data.emplace_back(entity_data);
	return true;
}

NS_END
