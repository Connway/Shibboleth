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

#pragma once

#include <Shibboleth_RefCounted.h>
#include <Shibboleth_ECSEntity.h>
#include <Shibboleth_Vector.h>
#include <Gaff_RefPtr.h>
#include <Gaff_Hash.h>

NS_GAFF
	class IReflectionDefinition;
	class ISerializeReader;
NS_END

NS_SHIBBOLETH

class ECSManager;

class ECSArchetype final
{
	GAFF_NO_COPY(ECSArchetype);

public:
	template <class T>
	bool removeShared(void)
	{
		return removeShared(Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	bool remove(void)
	{
		return remove(Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	bool addShared(void)
	{
		static_assert(T::IsShared(), "Cannot add an ECS component that does not support shared operations.");
		return addShared(Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	bool add(bool has_default_value = false)
	{
		static_assert(std::is_standard_layout<T>::value, "Cannot add an ECS component that does not have a standard layout.");
		static_assert(T::IsNonShared(), "Cannot add an ECS component that does not support non-shared operations.");
		return add(Reflection<T>::GetReflectionDefinition(), has_default_value);
	}

	template <class T>
	int32_t getComponentSharedOffset(void) const
	{
		return getComponentSharedOffset(Reflection<T>::GetHash());
	}

	template <class T>
	int32_t getComponentOffset(void) const
	{
		return getComponentOffset(Reflection<T>::GetHash());
	}

	template <class T>
	bool hasSharedComponent(void) const
	{
		return hasSharedComponent(Reflection<T>::GetHash());
	}

	template <class T>
	bool hasComponent(void) const
	{
		return hasComponent(Reflection<T>::GetHash());
	}

	template <class T>
	const T* getSharedComponent(void) const
	{
		return reinterpret_cast<const T*>(getSharedComponent(Reflection<T>::GetHash()));
	}

	template <class T>
	T* getSharedComponent(void)
	{
		return reinterpret_cast<T*>(getSharedComponent(Reflection<T>::GetHash()));
	}

	template <class T>
	void setDefaultValues(const T& value)
	{
		const auto it = Gaff::LowerBound(_vars_defaults, Reflection<T>::GetHash(), [](const RefDefOffset& lhs, Gaff::Hash64 rhs) -> bool
		{
			return lhs.ref_def->getReflectionInstance().getHash() < rhs;
		});

		// Component was deleted.
		if (it == it_end || it->ref_def != rdo.ref_def) {
			continue;
		}

		void* const component = reinterpret_cast<int8_t*>(_default_data) + it->offset;
		*reinterpret_cast<T*>(component) = value;
	}


	ECSArchetype(ECSArchetype&& archetype);
	ECSArchetype(void) = default;
	~ECSArchetype(void);

	ECSArchetype& operator=(ECSArchetype&& rhs);

	bool addShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	bool addShared(const Gaff::IReflectionDefinition& ref_def);
	bool removeShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	bool removeShared(const Gaff::IReflectionDefinition& ref_def);

	bool finalize(const Gaff::ISerializeReader& reader, const ECSArchetype& base_archetype, bool read_default_overrides = true);
	bool finalize(const Gaff::ISerializeReader& reader);
	bool finalize(const ECSArchetype& base_archetype);
	bool finalize(bool is_base = false);

	bool add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs, bool has_default_values = false);
	bool add(const Gaff::IReflectionDefinition& ref_def, bool has_default_value = false);
	bool remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	bool remove(const Gaff::IReflectionDefinition& ref_def);

	void append(const ECSArchetype& base);

	void copy(const ECSArchetype& base, bool copy_shared_instance_data = false, bool copy_default_data = false);
	void copy(
		const ECSArchetype& old_archetype,
		void* old_data,
		int32_t old_index,
		void* new_data,
		int32_t new_index
	);

	int32_t getComponentSharedOffset(Gaff::Hash64 component) const;
	int32_t getComponentOffset(Gaff::Hash64 component) const;

	bool hasSharedComponent(Gaff::Hash64 component) const;
	bool hasComponent(Gaff::Hash64 component) const;

	int32_t sharedSize(void) const;
	int32_t size(void) const;

	Gaff::Hash64 getHash(void) const;
	void calculateHash(void);

	const void* getSharedData(int32_t index) const;
	void* getSharedData(int32_t index);

	const void* getSharedData(void) const;
	void* getSharedData(void);

	const Gaff::IReflectionDefinition& getSharedComponentRefDef(int32_t index) const;
	int32_t getNumSharedComponents(void) const;

	const Gaff::IReflectionDefinition& getComponentRefDef(int32_t index) const;
	int32_t getNumComponents(void) const;

	const void* getSharedComponent(Gaff::Hash64 component) const;
	void* getSharedComponent(Gaff::Hash64 component);

	bool loadComponent(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader, Gaff::Hash64 component) const;
	bool loadComponent(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader, int32_t index) const;
	void loadDefaults(ECSManager& ecs_mgr, EntityID id) const;

	bool isBase(void) const;

	void destroyEntity(EntityID id, void* entity, int32_t entity_index) const;
	void constructPage(void* page, int32_t num_entities) const;

private:
	struct RefDefOffset final
	{
		using CopyDefaultToNonSharedFunc = void (*)(ECSManager&, EntityID, const void*);
		using CopySharedFunc = void (*)(const void*, void*);
		using CopyFunc = void (*)(const void*, int32_t, void*, int32_t);
		using LoadFunc = bool (*)(ECSManager&, EntityID, const Gaff::ISerializeReader&);
		using ConstructorFunc = void (*)(EntityID, void*, int32_t);
		using DestructorFunc = void (*)(EntityID, void*, int32_t);

		const Gaff::IReflectionDefinition* ref_def;
		int32_t offset;

		CopyDefaultToNonSharedFunc copy_default_to_non_shared_func;
		CopySharedFunc copy_shared_func;
		CopyFunc copy_func;
		LoadFunc load_func;

		ConstructorFunc constructor_func;
		DestructorFunc destructor_func;
	};

	Vector<RefDefOffset> _shared_vars;
	Vector<RefDefOffset> _vars;
	Vector<RefDefOffset> _vars_defaults;

	mutable Gaff::Hash64 _hash = Gaff::INIT_HASH64;

	int32_t _shared_alloc_size = 0;
	int32_t _alloc_size = 0;

	void* _shared_instances = nullptr;
	void* _default_data = nullptr;

	bool _is_base = false;

	template <bool shared>
	bool add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs, bool has_default_values = false);

	template <bool shared>
	bool add(const Gaff::IReflectionDefinition& ref_def, bool has_default_value = false);

	template <bool shared>
	bool remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);

	template <bool shared>
	bool remove(const Gaff::IReflectionDefinition& ref_def);

	template <bool shared>
	bool finalize(const Gaff::ISerializeReader& reader, const ECSArchetype* base_archetype, bool read_default_overrides);

	void initShared(const Gaff::ISerializeReader& reader, const ECSArchetype* base_archetype);
	void initShared(void);

	void copySharedInstanceData(const ECSArchetype& old_archetype);
	void destroySharedData(void);

	void initDefaultData(const Gaff::ISerializeReader& reader, const ECSArchetype* base_archetype, bool read_default_overrides);
	void initDefaultData(void);

	void copyDefaultData(const ECSArchetype& old_archetype);
	void destroyDefaultData(void);

	static bool SearchPredicate(const RefDefOffset& lhs, Gaff::Hash64 rhs);
};

class ArchetypeReference final
{
public:
	ArchetypeReference(ECSManager& ecs_mgr, Gaff::Hash64 archetype);
	~ArchetypeReference(void);

	const ECSArchetype& getArchetype(void) const;
	Gaff::Hash64 getArchetypeHash(void) const;

private:
	Gaff::Hash64 _archetype;
	ECSManager& _ecs_mgr;

	GAFF_NO_COPY(ArchetypeReference);
	SHIB_REF_COUNTED();
};

using ArchetypeReferencePtr = Gaff::RefPtr<ArchetypeReference>;

NS_END
