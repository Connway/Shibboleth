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

#include "Shibboleth_ReflectionManager.h"
#include <EASTL/sort.h>

NS_SHIBBOLETH

static bool CompareRefDef(const Gaff::IReflectionDefinition* lhs, const Gaff::IReflectionDefinition* rhs)
{
	return lhs->getReflectionInstance().getHash() < rhs->getReflectionInstance().getHash();
}

bool ReflectionManager::CompareRefHash(const Gaff::IReflectionDefinition* lhs, Gaff::Hash64 rhs)
{
	return lhs->getReflectionInstance().getHash() < rhs;
}

ReflectionManager::ReflectionManager(void)
{
	registerTypeBucket(Gaff::FNV1aHash64Const("**")); // All types not registered with a type bucket.
	registerTypeBucket(Gaff::FNV1aHash64Const("*"));  // All reflection.
}

ReflectionManager::~ReflectionManager(void)
{
	destroy();
}

void ReflectionManager::destroy(void)
{
	_module_enum_owners.clear();
	_module_owners.clear();

	_enum_reflection_map.clear();
	_reflection_map.clear();
	_type_buckets.clear();
}

const Gaff::IEnumReflectionDefinition* ReflectionManager::getEnumReflection(Gaff::Hash64 name) const
{
	auto it = _enum_reflection_map.find(name);
	return (it == _enum_reflection_map.end()) ? nullptr : it->second.get();
}

void ReflectionManager::registerEnumReflection(Gaff::Hash64 name, Gaff::IEnumReflectionDefinition* ref_def)
{
	GAFF_ASSERT(_enum_reflection_map.find(name) == _enum_reflection_map.end());
	_enum_reflection_map[name].reset(ref_def);
}

void ReflectionManager::registerEnumOwningModule(Gaff::Hash64 name, const char* module_name)
{
	const auto it_enum = _enum_reflection_map.find(name);
	GAFF_ASSERT(it_enum != _enum_reflection_map.end());

	Vector<const Gaff::IEnumReflectionDefinition*>& module_bucket = _module_enum_owners[HashString64(module_name)];
	const auto it = eastl::lower_bound(module_bucket.begin(), module_bucket.end(), it_enum->second.get());
	module_bucket.insert(it, it_enum->second.get());
}

const Gaff::IReflectionDefinition* ReflectionManager::getReflection(Gaff::Hash64 name) const
{
	auto it = _reflection_map.find(name);
	return (it == _reflection_map.end()) ? nullptr : it->second.get();
}

void ReflectionManager::registerReflection(Gaff::Hash64 name, Gaff::IReflectionDefinition* ref_def)
{
	GAFF_ASSERT(_reflection_map.find(name) == _reflection_map.end());
	_reflection_map[name].reset(ref_def);

	bool was_inserted = false;

	// Check if this type implements an interface that has a type bucket request.
	for (auto it = _type_buckets.begin(); it != _type_buckets.end(); ++it) {
		if (ref_def->hasInterface(it->first)) {
			insertType(it->second, ref_def);
			was_inserted = true;
		}
	}

	// Insert into no bucket bucket.
	if (!was_inserted) {
		const auto it = _type_buckets.find(Gaff::FNV1aHash64Const("**"));
		insertType(it->second, ref_def);
	}

	// Insert into all bucket.
	const auto it = _type_buckets.find(Gaff::FNV1aHash64Const("*"));
	insertType(it->second, ref_def);
}

void ReflectionManager::registerOwningModule(Gaff::Hash64 name, const char* module_name)
{
	const auto it_refl = _reflection_map.find(name);
	GAFF_ASSERT(it_refl != _reflection_map.end());

	TypeBucketMap& module_types = _module_owners[HashString64(module_name)];

	// If the type is found in a global type bucket, then put it in the module type bucket.
	for (const auto& bucket_pair : _type_buckets) {
		const auto it = eastl::lower_bound(bucket_pair.second.begin(), bucket_pair.second.end(), name, CompareRefHash);

		if (it != bucket_pair.second.end() && (*it)->getReflectionInstance().getHash() == name) {
			insertType(module_types[bucket_pair.first], it_refl->second.get());
		}
	}
}

void ReflectionManager::registerTypeBucket(Gaff::Hash64 name)
{
	GAFF_ASSERT(_type_buckets.find(name) == _type_buckets.end());

	// Add new type bucket to global type bucket list.
	TypeBucket& global_unbucketed = _type_buckets.find(Gaff::FNV1aHash64Const("**"))->second;
	TypeBucket& global_types = _type_buckets[name];

	for (const auto& ref_map_pair : _reflection_map) {
		const UniquePtr<Gaff::IReflectionDefinition>& ref_def = ref_map_pair.second;
		const Gaff::Hash64 class_hash = ref_def->getReflectionInstance().getHash();

		// Don't care about the concrete version of class. Only reflected types that inherit from the class.
		if (name != class_hash && ref_def->hasInterface(name)) {
			global_types.emplace_back(ref_def.get());
			removeType(global_unbucketed, ref_def.get());
		}
	}

	// Sort the list for quicker lookup.
	eastl::sort(global_types.begin(), global_types.end(), CompareRefDef);

	// For each module type bucket, add the new types to the type bucket list.
	for (auto& mod_pair : _module_owners) {
		TypeBucket& unbucketed = mod_pair.second[Gaff::FNV1aHash64Const("**")];
		TypeBucket& all_types = mod_pair.second[Gaff::FNV1aHash64Const("*")];
		TypeBucket& types = mod_pair.second[name];

		// Iterate through the all types registered to this module.
		for (auto it = all_types.begin(); it != all_types.end(); ++it) {
			const Gaff::Hash64 class_hash = (*it)->getReflectionInstance().getHash();

			if (name != class_hash && (*it)->hasInterface(name)) {
				types.emplace_back(*it);
				removeType(unbucketed, *it);
			}
		}
	}
}

const Vector<const Gaff::IReflectionDefinition*>* ReflectionManager::getTypeBucket(Gaff::Hash64 name, Gaff::Hash64 module_name) const
{
	const auto it_module = _module_owners.find(HashString64(module_name));

	if (it_module == _module_owners.end() || it_module->first.getHash() != module_name) {
		return nullptr;
	}

	const auto it_bucket = it_module->second.find(name);
	
	if (it_bucket == it_module->second.end() || it_bucket->first != name) {
		return nullptr;
	}

	return &it_bucket->second;
}

const Vector<const Gaff::IReflectionDefinition*>* ReflectionManager::getTypeBucket(Gaff::Hash64 name) const
{
	const auto it = _type_buckets.find(name);
	return (it == _type_buckets.end()) ? nullptr : &it->second;
}

Vector<const Gaff::IEnumReflectionDefinition*> ReflectionManager::getEnumReflectionWithAttribute(Gaff::Hash64 name) const
{
	Vector<const Gaff::IEnumReflectionDefinition*> out;

	for (const auto& entry : _enum_reflection_map) {
		if (entry.second->getEnumAttribute(name)) {
			out.push_back(entry.second.get());
		}
	}

	return out;
}

Vector<const Gaff::IReflectionDefinition*> ReflectionManager::getReflectionWithAttribute(Gaff::Hash64 name) const
{
	Vector<const Gaff::IReflectionDefinition*> out;

	for (const auto& entry : _reflection_map) {
		if (entry.second->getClassAttribute(name)) {
			out.push_back(entry.second.get());
		}
	}

	return out;
}

Vector<HashString64> ReflectionManager::getModules(void) const
{
	Vector<HashString64> out;

	for (const auto& modules : _module_owners) {
		out.emplace_back(modules.first);
	}

	return out;
}


void ReflectionManager::insertType(TypeBucket& bucket, const Gaff::IReflectionDefinition* ref_def)
{
	const auto it = eastl::lower_bound(bucket.begin(), bucket.end(), ref_def->getReflectionInstance().getHash(), CompareRefHash);
	bucket.insert(it, ref_def);
}

void ReflectionManager::removeType(TypeBucket& bucket, const Gaff::IReflectionDefinition* ref_def)
{
	const auto it = eastl::lower_bound(bucket.begin(), bucket.end(), ref_def, CompareRefDef);

	if (it != bucket.end() && *it == ref_def) {
		bucket.erase(it);
	}
}

NS_END
