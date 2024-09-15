/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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
#include "Shibboleth_EnumReflectionDefinition.h"
#include "Shibboleth_ReflectionDefinition.h"
#include "Shibboleth_ReflectionVersion.h"

NS_REFLECTION

template <class T, bool is_enum>
struct RefDefTypeHelper;

template <class T>
struct RefDefTypeHelper<T, true> final
{
	using Type = EnumReflectionDefinition<T>;
	using Interface = IEnumReflectionDefinition;
	using LoadFlags = IEnumReflectionDefinition::LoadFlags;
	using SaveFlags = IEnumReflectionDefinition::SaveFlags;
};

template <class T>
struct RefDefTypeHelper<T, false> final
{
	using Type = ReflectionDefinition<T>;
	using Interface = IReflectionDefinition;
	using LoadFlags = IReflectionDefinition::LoadFlags;
	using SaveFlags = IReflectionDefinition::SaveFlags;
};

template <class T>
using RefDefInterface = typename RefDefTypeHelper<T, std::is_enum<T>::value>::Interface;

template <class T>
using RefDefType = typename RefDefTypeHelper<T, std::is_enum<T>::value>::Type;

template <class T>
using RefDefLoadFlags = typename RefDefTypeHelper<T, std::is_enum<T>::value>::LoadFlags;

template <class T>
using RefDefSaveFlags = typename RefDefTypeHelper<T, std::is_enum<T>::value>::SaveFlags;

void AddToAttributeReflectionChain(IReflection* reflection);
IReflection* GetAttributeReflectionChainHead(void);

void AddToEnumReflectionChain(IReflection* reflection);
IReflection* GetEnumReflectionChainHead(void);

void AddToReflectionChain(IReflection* reflection);
IReflection* GetReflectionChainHead(void);

void InitAttributeReflection(void);
void InitClassReflection(void);
void InitEnumReflection(void);


template <class T>
class ReflectionBase : public IReflection
{
public:
	constexpr static int32_t Size(void);
	constexpr static bool IsEnum(void);

	ReflectionBase(void);

	Gaff::Hash64 getVersion(void) const override;
	bool isDefined(void) const override;
	int32_t size(void) const override;
	bool isEnum(void) const override;

	const IEnumReflectionDefinition& getEnumReflectionDefinition(void) const override;
	const IReflectionDefinition& getReflectionDefinition(void) const override;

	bool load(const Shibboleth::ISerializeReader& reader, T& object, Gaff::Flags< RefDefLoadFlags<T> > flags = Gaff::Flags< RefDefLoadFlags<T> >{});
	void save(Shibboleth::ISerializeWriter& writer, const T& object, Gaff::Flags< RefDefSaveFlags<T> > flags = Gaff::Flags< RefDefSaveFlags<T> >{});

protected:
	RefDefType<T>* _ref_def = nullptr;
	ReflectionVersion<T> _version;
	bool _defined = false;
};

NS_END

#include "Shibboleth_ReflectionBase.inl"
