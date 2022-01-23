/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include "Shibboleth_IReflectionDefinition.h"
#include "Shibboleth_IReflection.h"
#include <EASTL/sort.h>

NS_SHIBBOLETH

static bool CompareRefDef(const Reflection::IReflectionDefinition* lhs, const Reflection::IReflectionDefinition* rhs)
{
	return lhs->getReflectionInstance().getHash() < rhs->getReflectionInstance().getHash();
}

bool ReflectionManager::CompareRefHash(const Reflection::IReflectionDefinition* lhs, Gaff::Hash64 rhs)
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

	_attr_buckets.clear();
	_type_buckets.clear();
}

const Reflection::IEnumReflectionDefinition* ReflectionManager::getEnumReflection(Gaff::Hash64 name) const
{
	auto it = _enum_reflection_map.find(name);
	return (it == _enum_reflection_map.end()) ? nullptr : it->second.get();
}

Vector<const Reflection::IEnumReflectionDefinition*> ReflectionManager::getEnumReflection(void) const
{
	Vector<const Reflection::IEnumReflectionDefinition*> ret;

	for (const auto& pair : _enum_reflection_map) {
		ret.emplace_back(pair.second.get());
	}

	return ret;
}

void ReflectionManager::registerEnumReflection(Reflection::IEnumReflectionDefinition* ref_def)
{
	const Gaff::Hash64 name = ref_def->getReflectionInstance().getHash();

	GAFF_ASSERT(_enum_reflection_map.find(name) == _enum_reflection_map.end());
	_enum_reflection_map[name].reset(ref_def);
}

void ReflectionManager::registerEnumOwningModule(Gaff::Hash64 name, const char* module_name)
{
	const auto it_enum = _enum_reflection_map.find(name);
	GAFF_ASSERT(it_enum != _enum_reflection_map.end());

	const HashString64<> name_hash(module_name);
	const auto it_module = _module_enum_owners.find(name_hash);
	Vector<const Reflection::IEnumReflectionDefinition*>& module_bucket = (it_module == _module_enum_owners.end()) ? _module_enum_owners[name_hash] : it_module->second;

	if (it_module == _module_enum_owners.end()) {
		module_bucket.set_allocator(ProxyAllocator("Reflection"));
	}

	const auto it = eastl::lower_bound(module_bucket.begin(), module_bucket.end(), it_enum->second.get());
	module_bucket.insert(it, it_enum->second.get());
}

void ReflectionManager::registerReflection(Reflection::IEnumReflectionDefinition* ref_def)
{
	registerEnumReflection(ref_def);
}

const Reflection::IReflectionDefinition* ReflectionManager::getReflection(Gaff::Hash64 name) const
{
	auto it = _reflection_map.find(name);
	return (it == _reflection_map.end()) ? nullptr : it->second.get();
}

void ReflectionManager::registerReflection(Reflection::IReflectionDefinition* ref_def)
{
	const Gaff::Hash64 name = ref_def->getReflectionInstance().getHash();

	GAFF_ASSERT(_reflection_map.find(name) == _reflection_map.end());
	_reflection_map[name].reset(ref_def);

	addToAttributeBuckets(ref_def);
	addToTypeBuckets(ref_def);
}

void ReflectionManager::registerOwningModule(Gaff::Hash64 name, const char* module_name)
{
	const auto it_refl = _reflection_map.find(name);
	GAFF_ASSERT(it_refl != _reflection_map.end());

	const HashString64<> name_hash(module_name);
	const auto it_module = _module_owners.find(name_hash);
	TypeBucketMap& module_types = (it_module == _module_owners.end()) ? _module_owners[name_hash] : it_module->second;

	if (it_module == _module_owners.end()) {
		module_types.set_allocator(ProxyAllocator("Reflection"));
	}

	// If the type is found in a global type bucket, then put it in the module type bucket.
	for (const auto& bucket_pair : _type_buckets) {
		const auto it = eastl::lower_bound(bucket_pair.second.begin(), bucket_pair.second.end(), name, CompareRefHash);

		if (it != bucket_pair.second.end() && (*it)->getReflectionInstance().getHash() == name) {
			const auto it_bucket = module_types.find(bucket_pair.first);
			TypeBucket& type_bucket = (it_bucket == module_types.end()) ? module_types[bucket_pair.first] : it_bucket->second;

			if (it_bucket == module_types.end()) {
				type_bucket.set_allocator(ProxyAllocator("Reflection"));
			}

			insertType(module_types[bucket_pair.first], it_refl->second.get());
		}
	}
}

const ReflectionManager::TypeBucket* ReflectionManager::getTypeBucket(Gaff::Hash64 name, Gaff::Hash64 module_name) const
{
	const auto it_module = _module_owners.find(HashString64<>(module_name));

	if (it_module == _module_owners.end() || it_module->first.getHash() != module_name) {
		return nullptr;
	}

	const auto it_bucket = it_module->second.find(name);

	if (it_bucket == it_module->second.end() || it_bucket->first != name) {
		return nullptr;
	}

	return &it_bucket->second;
}

const ReflectionManager::TypeBucket* ReflectionManager::getTypeBucket(Gaff::Hash64 name) const
{
	const auto it = _type_buckets.find(name);
	return (it == _type_buckets.end()) ? nullptr : &it->second;
}

void ReflectionManager::registerTypeBucket(Gaff::Hash64 name)
{
	GAFF_ASSERT(_type_buckets.find(name) == _type_buckets.end());

	// Add new type bucket to global type bucket list.
	TypeBucket& global_unbucketed = _type_buckets.find(Gaff::FNV1aHash64Const("**"))->second;
	TypeBucket& global_types = _type_buckets[name];

	global_types.set_allocator(ProxyAllocator("Reflection"));

	for (const auto& ref_map_pair : _reflection_map) {
		const UniquePtr<Reflection::IReflectionDefinition>& ref_def = ref_map_pair.second;
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

const ReflectionManager::TypeBucket* ReflectionManager::getAttributeBucket(Gaff::Hash64 name) const
{
	const auto it = _attr_buckets.find(name);
	return (it == _attr_buckets.end()) ? nullptr : &it->second;
}

void ReflectionManager::registerAttributeBucket(Gaff::Hash64 attr_name)
{
	GAFF_ASSERT(_attr_buckets.find(attr_name) == _attr_buckets.end());

	TypeBucket& bucket = _attr_buckets[attr_name];
	bucket.set_allocator(ProxyAllocator("Reflection"));

	for (const auto& ref_map_pair : _reflection_map) {
		const UniquePtr<Reflection::IReflectionDefinition>& ref_def = ref_map_pair.second;

		// Check if we have anything with this attribute.
		if (ref_def->hasClassAttr(attr_name) ||
			ref_def->hasVarAttr(attr_name) ||
			ref_def->hasFuncAttr(attr_name)||
			ref_def->hasStaticFuncAttr(attr_name)) {

			bucket.emplace_back(ref_def.get());
		}
	}

	// Sort the list for quicker lookup.
	eastl::sort(bucket.begin(), bucket.end(), CompareRefDef);
}

ReflectionManager::TypeBucket ReflectionManager::getReflectionWithAttribute(Gaff::Hash64 name, Gaff::Hash64 module_name) const
{
	const auto* bucket = getTypeBucket(Gaff::FNV1aHash64Const("*"), module_name);
	TypeBucket out;

	if (!bucket) {
		return out;
	}

	for (const Reflection::IReflectionDefinition* ref_def : *bucket) {
		if (hasAttribute(*ref_def, name)) {
			out.emplace_back(ref_def);
		}
	}

	return out;
}

ReflectionManager::TypeBucket ReflectionManager::getReflectionWithAttribute(Gaff::Hash64 name) const
{
	const auto it = _attr_buckets.find(name);

	if (it != _attr_buckets.end()) {
		return it->second;
	}

	// Search manually.
	TypeBucket out;

	for (const auto& entry : _reflection_map) {
		if (hasAttribute(*entry.second, name)) {
			out.emplace_back(entry.second.get());
		}
	}

	return out;
}

Vector< HashString64<> > ReflectionManager::getModules(void) const
{
	Vector< HashString64<> > out;

	for (const auto& modules : _module_owners) {
		out.emplace_back(modules.first);
	}

	return out;
}


void ReflectionManager::insertType(TypeBucket& bucket, const Reflection::IReflectionDefinition* ref_def)
{
	const auto it = eastl::lower_bound(bucket.begin(), bucket.end(), ref_def->getReflectionInstance().getHash(), CompareRefHash);
	bucket.insert(it, ref_def);
}

void ReflectionManager::removeType(TypeBucket& bucket, const Reflection::IReflectionDefinition* ref_def)
{
	const auto it = eastl::lower_bound(bucket.begin(), bucket.end(), ref_def, CompareRefDef);

	if (it != bucket.end() && *it == ref_def) {
		bucket.erase(it);
	}
}

bool ReflectionManager::hasAttribute(const Reflection::IReflectionDefinition& ref_def, Gaff::Hash64 name) const
{
	return	ref_def.hasClassAttr(name) ||
			ref_def.hasVarAttr(name) ||
			ref_def.hasFuncAttr(name) ||
			ref_def.hasStaticFuncAttr(name);
}

void ReflectionManager::addToAttributeBuckets(const Reflection::IReflectionDefinition* ref_def)
{
	for (auto& bucket_pair : _attr_buckets) {
		if (hasAttribute(*ref_def, bucket_pair.first)) {
			insertType(bucket_pair.second, ref_def);
		}
	}
}

void ReflectionManager::addToTypeBuckets(const Reflection::IReflectionDefinition* ref_def)
{
	bool was_inserted = false;

	// Check if this type implements an interface that has a type bucket request.
	for (auto it = _type_buckets.begin(); it != _type_buckets.end(); ++it) {
		if (ref_def->getReflectionInstance().getHash() != it->first && ref_def->hasInterface(it->first)) {
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

NS_END
