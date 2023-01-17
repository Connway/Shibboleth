/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_IReflection.h"
#include "Shibboleth_IAttribute.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_VectorMap.h"
#include "Shibboleth_SmartPtrs.h"
#include "Shibboleth_String.h"
#include <Gaff_Hashable.h>

NS_SHIBBOLETH

class ReflectionManager final
{
public:
	using TypeBucket = Vector<const Refl::IReflectionDefinition*>;

	static bool CompareRefHash(const Refl::IReflectionDefinition* lhs, Gaff::Hash64 rhs);

	template <class T>
	void registerEnumOwningModule(const char8_t* module_name)
	{
		return registerEnumOwningModule(Hash::template ClassHashable<T>::GetHash(), module_name);
	}

	template <class T>
	void registerOwningModule(const char8_t* module_name)
	{
		return registerOwningModule(Hash::template ClassHashable<T>::GetHash(), module_name);
	}

	template <class T>
	void registerAttributeBucket(void)
	{
		registerAttributeBucket(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	void registerTypeBucket(void)
	{
		registerTypeBucket(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	const TypeBucket* getAttributeBucket(Gaff::Hash64 module_name) const
	{
		static_assert(std::is_base_of<Refl::IAttribute, T>::value, "T is not an attribute.");
		return getAttributeBucket(Hash::template ClassHashable<T>::GetHash(), module_name);
	}

	template <class T>
	const TypeBucket* getAttributeBucket(void) const
	{
		static_assert(std::is_base_of<Refl::IAttribute, T>::value, "T is not an attribute.");
		return getAttributeBucket(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	const TypeBucket* getTypeBucket(Gaff::Hash64 module_name) const
	{
		return getTypeBucket(Hash::template ClassHashable<T>::GetHash(), module_name);
	}

	template <class T>
	const TypeBucket* getTypeBucket(void) const
	{
		return getTypeBucket(Hash::template ClassHashable<T>::GetHash());
	}

	template <class T>
	TypeBucket getReflectionWithAttribute(Gaff::Hash64 module_name) const
	{
		static_assert(std::is_base_of<Refl::IAttribute, T>::value, "T is not an attribute.");
		return getReflectionWithAttribute(Hash::template ClassHashable<T>::GetHash(), module_name);
	}

	template <class T>
	TypeBucket getReflectionWithAttribute(void) const
	{
		static_assert(std::is_base_of<Refl::IAttribute, T>::value, "T is not an attribute.");
		return getReflectionWithAttribute(Hash::template ClassHashable<T>::GetHash());
	}



	ReflectionManager(void);
	~ReflectionManager(void);

	void destroy(void);

	const Refl::IEnumReflectionDefinition* getEnumReflection(Gaff::Hash64 name) const;
	Vector<const Refl::IEnumReflectionDefinition*> getEnumReflection(void) const;
	void registerEnumReflection(Refl::IEnumReflectionDefinition* ref_def);
	void registerEnumOwningModule(Gaff::Hash64 name, const char8_t* module_name);
	void registerReflection(Refl::IEnumReflectionDefinition* ref_def);

	const Refl::IReflectionDefinition* getReflection(Gaff::Hash64 name) const;
	void registerReflection(Refl::IReflectionDefinition* ref_def);
	void registerOwningModule(Gaff::Hash64 name, const char8_t* module_name);

	const TypeBucket* getTypeBucket(Gaff::Hash64 name, Gaff::Hash64 module_name) const;
	const TypeBucket* getTypeBucket(Gaff::Hash64 name) const;
	void registerTypeBucket(Gaff::Hash64 name);

	const TypeBucket* getAttributeBucket(Gaff::Hash64 name) const;
	void registerAttributeBucket(Gaff::Hash64 attr_name);

	TypeBucket getReflectionWithAttribute(Gaff::Hash64 name, Gaff::Hash64 module_name) const;
	TypeBucket getReflectionWithAttribute(Gaff::Hash64 name) const;

	Vector< HashString64<> > getModules(void) const;

private:
	using TypeBucketMap = VectorMap<Gaff::Hash64, TypeBucket>;

	VectorMap< Gaff::Hash64, UniquePtr<Refl::IEnumReflectionDefinition> > _enum_reflection_map{ ProxyAllocator("Reflection") };
	VectorMap< Gaff::Hash64, UniquePtr<Refl::IReflectionDefinition> > _reflection_map{ ProxyAllocator("Reflection") };

	TypeBucketMap _attr_buckets{ ProxyAllocator("Reflection") };
	TypeBucketMap _type_buckets{ ProxyAllocator("Reflection") };

	VectorMap< HashString64<>, Vector<const Refl::IEnumReflectionDefinition*> > _module_enum_owners{ ProxyAllocator("Reflection") };
	VectorMap<HashString64<>, TypeBucketMap> _module_owners{ ProxyAllocator("Reflection") };

	void insertType(TypeBucket& bucket, const Refl::IReflectionDefinition* ref_def);
	void removeType(TypeBucket& bucket, const Refl::IReflectionDefinition* ref_def);

	bool hasAttribute(const Refl::IReflectionDefinition& ref_def, Gaff::Hash64 name) const;
	void addToAttributeBuckets(const Refl::IReflectionDefinition* ref_def);
	void addToTypeBuckets(const Refl::IReflectionDefinition* ref_def);

	GAFF_NO_COPY(ReflectionManager);
	GAFF_NO_MOVE(ReflectionManager);
};

NS_END
