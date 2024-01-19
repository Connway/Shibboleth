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

#include "Shibboleth_ReflectionVar.h"
#include "Shibboleth_String.h"
#include <Gaff_Flags.h>

NS_REFLECTION

template <class T, class Enum>
class VarFlagBit final : public IVar<T>
{
public:
	const IReflection& getReflection(void) const override;
	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool load(const Shibboleth::ISerializeReader& reader, void* object) override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object) override;
	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

private:
	bool _cache = false;
};

template <class T, class Enum>
class VarFlags final : public IVar<T>
{
public:
	VarFlags(Gaff::Flags<Enum> T::* ptr);
	VarFlags(void) = default;

	const IReflection& getReflection(void) const override;
	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	int32_t size(const void* object) const override;

	bool load(const Shibboleth::ISerializeReader& reader, void* object) override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object) override;
	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	bool isFlags(void) const override;

	void setFlagValue(void* object, int32_t flag_index, bool value) override;
	bool getFlagValue(const void* object, int32_t flag_index) const override;

	const Shibboleth::Vector<IReflectionVar::SubVarData>& getSubVars(void) override;
	void setSubVarBaseName(eastl::u8string_view base_name) override;

private:
	using RefVarType = VarFlagBit<T, Enum>;

	Shibboleth::Vector<IReflectionVar::SubVarData> _cached_element_vars{ Shibboleth::ProxyAllocator("Reflection") };
	eastl::array<RefVarType, static_cast<size_t>(Enum::Count)> _elements;
};



template <class T, class Enum>
struct VarTypeHelper< T, Gaff::Flags<Enum> > final
{
	using Type = VarFlags<T, Enum>;
	static constexpr bool k_can_copy = true;
};

NS_END
