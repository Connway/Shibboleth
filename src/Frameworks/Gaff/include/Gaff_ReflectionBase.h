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

#include "Gaff_EnumReflectionDefinition.h"
#include "Gaff_ReflectionInterfaces.h"
#include "Gaff_ReflectionDefinition.h"
#include "Gaff_SerializeInterfaces.h"
#include "Gaff_ReflectionVersion.h"
#include "Gaff_Hashable.h"
#include "Gaff_Utils.h"

NS_GAFF

template <class T, class Allocator, bool is_enum>
struct RefDefTypeHelper;

template <class T, class Allocator>
struct RefDefTypeHelper<T, Allocator, true> final
{
	using Type = EnumReflectionDefinition<T, Allocator>;
	using Interface = IEnumReflectionDefinition;
};

template <class T, class Allocator>
struct RefDefTypeHelper<T, Allocator, false> final
{
	using Type = ReflectionDefinition<T, Allocator>;
	using Interface = IReflectionDefinition;
};

template <class T, class Allocator>
using RefDefInterface = typename RefDefTypeHelper<T, Allocator, std::is_enum<T>::value>::Interface;

template <class T, class Allocator>
using RefDefType = typename RefDefTypeHelper<T, Allocator, std::is_enum<T>::value>::Type;


void AddToAttributeReflectionChain(IReflection* reflection);
IReflection* GetAttributeReflectionChainHead(void);

void AddToReflectionChain(IReflection* reflection);
IReflection* GetReflectionChainHead(void);

void AddToEnumReflectionChain(IReflection* reflection);
IReflection* GetEnumReflectionChainHead(void);

void InitAttributeReflection(void);
void InitClassReflection(void);
void InitEnumReflection(void);


template <class T, class Allocator>
class ReflectionBase : public IReflection
{
public:
	//constexpr static bool HasReflection = true;

	constexpr static Hash64 GetHash(void);
	constexpr static const char* GetName(void);

	constexpr static int32_t Size(void);

	constexpr static bool IsEnum(void);

	static Hash64 GetInstanceHash(const T& object, Hash64 init = INIT_HASH64);
	static Hash64 GetVersion(void);

	static const typename RefDefType<T, Allocator>& GetReflectionDefinition(void);

	static bool IsDefined(void);

	static void RegisterOnDefinedCallback(const eastl::function<void (void)>& callback);
	static void RegisterOnDefinedCallback(eastl::function<void (void)>&& callback);

	static bool Load(const ISerializeReader& reader, T& object, bool refl_load = false);
	static void Save(ISerializeWriter& writer, const T& object, bool refl_save = false);

	static void SetAllocator(const Allocator& a);

	ReflectionBase(void);

	bool isEnum(void) const override;

	Hash64 getInstanceHash(const void* object, Hash64 init = INIT_HASH64) const override;
	Hash64 getInstanceHash(const T& object, Hash64 init = INIT_HASH64) const;

	const char* getName(void) const override;
	Hash64 getHash(void) const override;

	Hash64 getVersion(void) const override;

	int32_t size(void) const override;

	const IEnumReflectionDefinition& getEnumReflectionDefinition(void) const override;
	const IReflectionDefinition& getReflectionDefinition(void) const override;

	bool load(const ISerializeReader& reader, void* object, bool refl_load = false) const override;
	void save(ISerializeWriter& writer, const void* object, bool refl_save = false) const override;

protected:
	static Vector<eastl::function<void (void)>, Allocator> g_on_defined_callbacks;
	static typename RefDefType<T, Allocator>* g_ref_def;
	static ReflectionVersion<T> g_version;
	static bool g_defined;
};

#include "Gaff_ReflectionBase.inl"

NS_END
