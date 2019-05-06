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

#pragma once

#include <Shibboleth_Vector.h>
#include <Gaff_Hash.h>

NS_GAFF
	class IReflectionDefinition;
	class ISerializeReader;
NS_END

NS_SHIBBOLETH

class IECSVarAttribute;

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
		return addShared(Reflection<T>::GetReflectionDefinition());
	}

	template <class T>
	bool add(void)
	{
		return add(Reflection<T>::GetReflectionDefinition());
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

	ECSArchetype(ECSArchetype&& archetype);
	ECSArchetype(void) = default;
	~ECSArchetype(void);

	ECSArchetype& operator=(ECSArchetype&& rhs);

	bool addShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	bool addShared(const Gaff::IReflectionDefinition& ref_def);
	bool removeShared(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	bool removeShared(const Gaff::IReflectionDefinition& ref_def);
	bool removeShared(int32_t index);

	bool finalize(const Gaff::ISerializeReader& reader);
	bool finalize(void);

	bool add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	bool add(const Gaff::IReflectionDefinition& ref_def);
	bool remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	bool remove(const Gaff::IReflectionDefinition& ref_def);
	bool remove(int32_t index);

	void copy(const ECSArchetype& base, bool copy_shared_instance_data = false);
	void copy(
		const ECSArchetype& old_archetype,
		void* old_data,
		int32_t old_index,
		void* new_data,
		int32_t new_index
	);

	int32_t getComponentSharedOffset(Gaff::Hash64 component) const;
	int32_t getComponentOffset(Gaff::Hash64 component) const;

	int32_t sharedSize(void) const;
	int32_t size(void) const;

	Gaff::Hash64 getHash(void) const;

	const void* getSharedData(int32_t index) const;
	void* getSharedData(int32_t index);

	const void* getSharedData(void) const;
	void* getSharedData(void);

	const Gaff::IReflectionDefinition& getSharedComponentRefDef(int32_t index) const;
	int32_t getNumSharedComponents(void) const;

	const Gaff::IReflectionDefinition& getComponentRefDef(int32_t index) const;
	int32_t getNumComponents(void) const;

private:
	struct RefDefOffset final
	{
		using CopySharedFunc = void (*)(const void*, void*);
		using CopyFunc = void (*)(const void*, int32_t, void*, int32_t);

		const Gaff::IReflectionDefinition* ref_def;
		int32_t offset;

		CopySharedFunc copy_shared_func;
		CopyFunc copy_func;
	};

	Vector<RefDefOffset> _shared_vars;
	Vector<RefDefOffset> _vars;

	mutable Gaff::Hash64 _hash = Gaff::INIT_HASH64;

	int32_t _shared_alloc_size = 0;
	int32_t _alloc_size = 0;

	void* _shared_instances = nullptr;

	template <bool shared>
	bool add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);

	template <bool shared>
	bool add(const Gaff::IReflectionDefinition& ref_def);

	template <bool shared>
	bool remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);

	template <bool shared>
	bool remove(const Gaff::IReflectionDefinition& ref_def);

	template <bool shared>
	bool remove(int32_t index);

	void initShared(const Gaff::ISerializeReader& reader);
	void initShared(void);

	void copySharedInstanceData(const ECSArchetype& old_archetype);
	void destroySharedData(void);
	void calculateHash(void);
};

NS_END
