/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_ECSArchetype.h"
#include "Shibboleth_ECSAttributes.h"

NS_SHIBBOLETH

void ECSArchetype::addShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	add(_shared_vars, _shared_alloc_size, ref_defs);
}

void ECSArchetype::addShared(const Gaff::IReflectionDefinition* ref_def)
{
	add(_shared_vars, _shared_alloc_size, ref_def);
}

void ECSArchetype::removeShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	remove(_shared_vars, _shared_alloc_size, ref_defs);
}

void ECSArchetype::removeShared(const Gaff::IReflectionDefinition* ref_def)
{
	remove(_shared_vars, _shared_alloc_size, ref_def);
}

void ECSArchetype::removeShared(int32_t index)
{
	remove(_shared_vars, _shared_alloc_size, index);
}

void ECSArchetype::add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	add(_vars, _alloc_size, ref_defs);
}

void ECSArchetype::add(const Gaff::IReflectionDefinition* ref_def)
{
	add(_vars, _alloc_size, ref_def);
}

void ECSArchetype::remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	remove(_vars, _alloc_size, ref_defs);
}

void ECSArchetype::remove(const Gaff::IReflectionDefinition* ref_def)
{
	remove(_vars, _alloc_size, ref_def);

}

void ECSArchetype::remove(int32_t index)
{
	remove(_vars, _alloc_size, index);
}

int32_t ECSArchetype::sharedSize(void) const
{
	return _shared_alloc_size;
}

int32_t ECSArchetype::size(void) const
{
	return _alloc_size;
}

Gaff::Hash64 ECSArchetype::getHash(void) const
{
	if (_hash == Gaff::INIT_HASH64) {
		for (const Gaff::IReflectionDefinition* ref_def : _shared_vars) {
			const Gaff::Hash64 hash = ref_def->getReflectionInstance().getHash();
			_hash = Gaff::FNV1aHash64T(&hash, _hash);
		}

		for (const Gaff::IReflectionDefinition* ref_def : _vars) {
			const Gaff::Hash64 hash = ref_def->getReflectionInstance().getHash();
			_hash = Gaff::FNV1aHash64T(&hash, _hash);
		}
	}

	return _hash;
}

bool ECSArchetype::fromJSON(const Gaff::JSON& json)
{
	if (!json.isObject()) {
		return false;
	}

	const ReflectionManager& refl_mgr = GetApp().getReflectionManager();
	const Gaff::JSON shared_components = json["SharedComponents"];
	const Gaff::JSON components = json["Components"];

	if (shared_components.isArray()) {
		const bool failed = shared_components.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool {
			if (!value.isString()) {
				// $TODO: Log error
				return true;
			}

			const Gaff::IReflectionDefinition* const ref_def = refl_mgr.getReflection(Gaff::FNV1aHash64String(value.getString()));

			if (!ref_def) {
				// $TODO: Log error
				return true;
			}

			addShared(ref_def);

			return false;
		});

		if (failed) {
			return false;
		}
	}

	if (components.isArray()) {
		const bool failed = components.forEachInArray([&](int32_t, const Gaff::JSON& value) -> bool {
			if (!value.isString()) {
				// $TODO: Log error
				return true;
			}

			const Gaff::IReflectionDefinition* const ref_def = refl_mgr.getReflection(Gaff::FNV1aHash64String(value.getString()));

			if (!ref_def) {
				// $TODO: Log error
				return true;
			}

			add(ref_def);

			return false;
		});

		if (failed) {
			return false;
		}
	}

	return true;
}

Gaff::JSON ECSArchetype::toJSON(void) const
{
	Gaff::JSON shared_components = Gaff::JSON::CreateArray();
	Gaff::JSON components = Gaff::JSON::CreateArray();

	for (const Gaff::IReflectionDefinition* ref_def : _shared_vars) {
		shared_components.push(Gaff::JSON::CreateString(ref_def->getReflectionInstance().getName()));
	}

	for (const Gaff::IReflectionDefinition* ref_def : _vars) {
		components.push(Gaff::JSON::CreateString(ref_def->getReflectionInstance().getName()));
	}

	Gaff::JSON json = Gaff::JSON::CreateObject();
	json.setObject("SharedComponents", shared_components);
	json.setObject("Components", components);

	return json;
}

void ECSArchetype::add(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		add(vars, alloc_size, ref_def);
	}
}

void ECSArchetype::add(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Gaff::IReflectionDefinition* ref_def)
{
	Vector<const IECSVarAttribute*> attrs;
	ref_def->getClassAttrs(CLASS_HASH(IECSVarAttribute), attrs);

	if (attrs.empty()) {
		return;
	}

	for (const IECSVarAttribute* attr : attrs) {
		alloc_size += attr->getType().getReflectionInstance().size();
		vars.emplace_back(ref_def);
	}
}

void ECSArchetype::remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Vector<const Gaff::IReflectionDefinition*>& ref_defs)
{
	for (const Gaff::IReflectionDefinition* ref_def : ref_defs) {
		remove(vars, alloc_size, ref_def);
	}
}

void ECSArchetype::remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, const Gaff::IReflectionDefinition* ref_def)
{
	const auto it = Gaff::Find(vars, ref_def);

	if (it == vars.end()) {
		return;
	}

	vars.erase_unsorted(it);

	Vector<const IECSVarAttribute*> attrs;
	ref_def->getClassAttrs(CLASS_HASH(IECSVarAttribute), attrs);

	for (const IECSVarAttribute* attr : attrs) {
		alloc_size -= attr->getType().getReflectionInstance().size();
	}

	_hash = Gaff::INIT_HASH64;
}

void ECSArchetype::remove(Vector<const Gaff::IReflectionDefinition*>& vars, int32_t& alloc_size, int32_t index)
{
	GAFF_ASSERT(index < static_cast<int32_t>(vars.size()));
	const auto it = vars.begin() + index;
	vars.erase(it);

	Vector<const IECSVarAttribute*> attrs;
	(*it)->getClassAttrs(CLASS_HASH(IECSVarAttribute), attrs);

	for (const IECSVarAttribute* attr : attrs) {
		alloc_size -= attr->getType().getReflectionInstance().size();
	}

	_hash = Gaff::INIT_HASH64;
}

NS_END
