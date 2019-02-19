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

void ECSQuery::addShared(const Gaff::IReflectionDefinition* ref_def, FilterFunc&& filter)
{
	_shared_components.emplace_back(QueryDataShared{ ref_def, std::move(filter) });
}

void ECSQuery::addShared(const Gaff::IReflectionDefinition* ref_def)
{
	_shared_components.emplace_back(QueryDataShared{ ref_def, nullptr });
}

void ECSQuery::add(const Gaff::IReflectionDefinition* ref_def)
{
	_components.emplace_back(ref_def);
}

bool ECSQuery::filter(const ECSArchetype& archetype) const
{
	const void* shared_data = archetype.getSharedData();
	const int32_t shared_size = archetype.sharedSize();

	if (!_shared_components.empty()) {
		if (shared_size > 0 && !shared_data) {
			return false;
		}
	}

	for (const QueryDataShared& data : _shared_components) {
		const int32_t offset = archetype.getComponentSharedOffset(data.ref_def->getReflectionInstance().getHash());

		if (offset == -1) {
			return false;
		}

		if (data.filter && !data.filter(reinterpret_cast<const int8_t*>(shared_data) + offset)) {
			return false;
		}
	}

	for (const Gaff::IReflectionDefinition* ref_def : _components) {
		const int32_t offset = archetype.getComponentOffset(ref_def->getReflectionInstance().getHash());

		if (offset == -1) {
			return false;
		}
	}

	return true;
}

NS_END
