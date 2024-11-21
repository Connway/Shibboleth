/************************************************************************************
Copyright (C) by Nicholas LaCroix

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

template <class T, class HashStorage>
class VarHash final : public IVar<T>
{
public:
	VarHash(Gaff::Hash<HashStorage> T::* ptr);

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
	Shibboleth::U8String _string;
};



template <class T, class HashStorage>
struct VarTypeHelper< T, Gaff::Hash<HashStorage> > final
{
	static_assert(std::is_same_v<HashStorage, Gaff::Hash32Storage> || std::is_same_v<HashStorage, Gaff::Hash32Storage>, "Hash storage is not 32-bit or 64-bit.");

	// using ReflectionType = Gaff::Hash<HashStorage>;
	// using VariableType = Gaff::Hash<HashStorage>;
	using Type = VarHash<T, HashStorage>;

	static constexpr bool k_can_copy = true;
	static constexpr bool k_can_move = true;
};

NS_END
