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

#include "Shibboleth_ReflectionVar.h"
#include "Shibboleth_String.h"

NS_REFLECTION

template <class T, class Enum>
class VarFlags final : public IVar<T>
{
public:
	VarFlags(Gaff::Flags<Enum> T::* ptr);

	const IReflection& getReflection(void) const override;
	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	bool isFlags(void) const override;

	void setFlagValue(void* object, int32_t flag_index, bool value) override;
	bool getFlagValue(void* object, int32_t flag_index) const override;

private:
	Gaff::Flags<Enum> T::* _ptr = nullptr;
};



template <class T, class Enum>
class VarFlagBit final : public IVar<T>
{
public:
	VarFlagBit(Gaff::Flags<Enum> T::* ptr, uint8_t flag_index);

	const IReflection& getReflection(void) const override;
	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

private:
	Gaff::Flags<Enum> T::* _ptr = nullptr;
	uint8_t _flag_index = static_cast<uint8_t>(-1); // Unless flags have the craziest amount of flags, this should hold all possible flag values.
	bool _cache = false;
};




template <class T, class Enum>
struct VarPtrTypeHelper< T, Gaff::Flags<Enum> > final
{
	static IVar<T>* Create(
		eastl::u8string_view name,
		Gaff::Flags<Enum> T::* ptr,
		Shibboleth::ProxyAllocator& allocator,
		Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& extra_vars
	);
};

NS_END
