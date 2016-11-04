/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gaff_IReflectionDefinition.h"
#include "Gaff_ReflectionVersion.h"
#include "Gaff_HashString.h"
#include "Gaff_VectorMap.h"
#include "Gaff_Assert.h"
#include "Gaff_Utils.h"

NS_GAFF

template <class T, class Allocator>
class ReflectionDefinition final : public IReflectionDefinition
{
public:
	GAFF_STRUCTORS_DEFAULT(ReflectionDefinition);
	GAFF_NO_COPY(ReflectionDefinition);
	GAFF_NO_MOVE(ReflectionDefinition);

	void load(ISerializeReader& reader, void* object) const override;
	void save(ISerializeWriter& writer, const void* object) const override;
	void load(ISerializeReader& reader, T& object) const;
	void save(ISerializeWriter& writer, const T& object) const;

	const void* getInterface(ReflectionHash class_hash, const void* object) const override;
	void* getInterface(ReflectionHash class_hash, void* object) const override;

	void setAllocator(const Allocator& allocator);

	void setReflectionInstance(const ISerializeInfo* reflection_instance);
	const ISerializeInfo& getReflectionInstance(void) const override;

	Hash64 getVersionHash(void) const;

	ReflectionDefinition& baseClass(const char* name, ReflectionHash hash, ptrdiff_t offset);

	template <class Base>
	ReflectionDefinition& baseClass(void);

private:
	VectorMap<HashString32<Allocator>, ptrdiff_t, Allocator> _base_class_offsets;

	//HashMap<HashString32<Allocator>, ValueContainerPtr, Allocator> _value_containers;
	//Array<Pair< ReflectionHash, FunctionBinder<void*, const void*> >, Allocator> _base_ids;
	//Array<IOnCompleteFunctor*, Allocator> _callback_references;

	U8String<Allocator> _name;
	const ISerializeInfo* _reflection_instance = nullptr;
	Allocator _allocator;

	unsigned int _base_classes_remaining;
	bool _defined;

	ReflectionVersion<T> _version;
};

NS_END

#include "Gaff_ReflectionDefinition.inl"

#define BASE_CLASS(type) baseClass(#type, REFL_HASH_CONST(#type), Gaff::OffsetOfClass<ThisType, Base>())
