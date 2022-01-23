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

#pragma once

#include "Shibboleth_IReflection.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_VectorMap.h"
#include "Shibboleth_SmartPtrs.h"
#include "Shibboleth_String.h"

namespace Reflection
{
	class IEnumReflectionDefinition;
	class IReflectionDefinition;
}

NS_SHIBBOLETH

class ReflectionManager final
{
public:
	using TypeBucket = Vector<const Reflection::IReflectionDefinition*>;

	static bool CompareRefHash(const Reflection::IReflectionDefinition* lhs, Gaff::Hash64 rhs);

	template <class T>
	TypeBucket getReflectionWithAttribute(void) const
	{
		return getReflectionWithAttribute(Reflection::Reflection<T>::GetHash());
	}

	template <class T>
	const TypeBucket* getAttributeBucket(void) const
	{
		static_assert(std::is_base_of<Reflection::IAttribute, T>::value, "T is not an attribute.");
		return getAttributeBucket(Reflection::Reflection<T>::GetHash());
	}


	ReflectionManager(void);
	~ReflectionManager(void);

	void destroy(void);

	const Reflection::IEnumReflectionDefinition* getEnumReflection(Gaff::Hash64 name) const;
	Vector<const Reflection::IEnumReflectionDefinition*> getEnumReflection(void) const;
	void registerEnumReflection(Reflection::IEnumReflectionDefinition* ref_def);
	void registerEnumOwningModule(Gaff::Hash64 name, const char* module_name);
	void registerReflection(Reflection::IEnumReflectionDefinition* ref_def);

	const Reflection::IReflectionDefinition* getReflection(Gaff::Hash64 name) const;
	void registerReflection(Reflection::IReflectionDefinition* ref_def);
	void registerOwningModule(Gaff::Hash64 name, const char* module_name);

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

	VectorMap< Gaff::Hash64, UniquePtr<Reflection::IEnumReflectionDefinition> > _enum_reflection_map{ ProxyAllocator("Reflection") };
	VectorMap< Gaff::Hash64, UniquePtr<Reflection::IReflectionDefinition> > _reflection_map{ ProxyAllocator("Reflection") };

	TypeBucketMap _attr_buckets{ ProxyAllocator("Reflection") };
	TypeBucketMap _type_buckets{ ProxyAllocator("Reflection") };

	VectorMap< HashString64<>, Vector<const Reflection::IEnumReflectionDefinition*> > _module_enum_owners{ ProxyAllocator("Reflection") };
	VectorMap<HashString64<>, TypeBucketMap> _module_owners{ ProxyAllocator("Reflection") };

	void insertType(TypeBucket& bucket, const Reflection::IReflectionDefinition* ref_def);
	void removeType(TypeBucket& bucket, const Reflection::IReflectionDefinition* ref_def);

	bool hasAttribute(const Reflection::IReflectionDefinition& ref_def, Gaff::Hash64 name) const;
	void addToAttributeBuckets(const Reflection::IReflectionDefinition* ref_def);
	void addToTypeBuckets(const Reflection::IReflectionDefinition* ref_def);

	GAFF_NO_COPY(ReflectionManager);
	GAFF_NO_MOVE(ReflectionManager);
};

NS_END
