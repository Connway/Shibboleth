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

#pragma once

#include <Shibboleth_HashString.h>
#include <Shibboleth_VectorMap.h>
#include <Shibboleth_SmartPtrs.h>
#include <Shibboleth_Vector.h>
#include <Shibboleth_String.h>
#include <Gaff_ReflectionInterfaces.h>

NS_SHIBBOLETH

class ReflectionManager
{
public:
	static bool CompareRefHash(const Gaff::IReflectionDefinition* lhs, Gaff::Hash64 rhs);

	ReflectionManager(void);

	void destroy(void);

	const Gaff::IEnumReflectionDefinition* getEnumReflection(Gaff::Hash64 name) const;
	void registerEnumReflection(Gaff::Hash64 name, Gaff::IEnumReflectionDefinition* ref_def);
	void registerEnumOwningModule(Gaff::Hash64 name, const char* module_name);

	const Gaff::IReflectionDefinition* getReflection(Gaff::Hash64 name) const;
	void registerReflection(Gaff::Hash64 name, Gaff::IReflectionDefinition* ref_def);
	void registerOwningModule(Gaff::Hash64 name, const char* module_name);

	const Vector<const Gaff::IReflectionDefinition*>* getTypeBucket(Gaff::Hash64 name, Gaff::Hash64 module_name) const;
	const Vector<const Gaff::IReflectionDefinition*>* getTypeBucket(Gaff::Hash64 name) const;
	void registerTypeBucket(Gaff::Hash64 name);

	Vector<const Gaff::IEnumReflectionDefinition*> getEnumReflectionWithAttribute(Gaff::Hash64 name) const;
	Vector<const Gaff::IReflectionDefinition*> getReflectionWithAttribute(Gaff::Hash64 name) const;

private:
	using TypeBucket = Vector<const Gaff::IReflectionDefinition*>;
	using TypeBucketMap = VectorMap<Gaff::Hash64, TypeBucket>;

	VectorMap< Gaff::Hash64, UniquePtr<Gaff::IEnumReflectionDefinition> > _enum_reflection_map;
	VectorMap< Gaff::Hash64, UniquePtr<Gaff::IReflectionDefinition> > _reflection_map;
	TypeBucketMap _type_buckets;

	VectorMap< HashString64, Vector<const Gaff::IEnumReflectionDefinition*> > _module_enum_owners;
	VectorMap<HashString64, TypeBucketMap> _module_owners;

	void insertType(TypeBucket& bucket, const Gaff::IReflectionDefinition* ref_def);
	void removeType(TypeBucket& bucket, const Gaff::IReflectionDefinition* ref_def);

	GAFF_NO_COPY(ReflectionManager);
	GAFF_NO_MOVE(ReflectionManager);
};

NS_END
