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

#include "Shibboleth_SerializeInterfaces.h"
#include "Shibboleth_IReflection.h"
#include <EASTL/string_view.h>

NS_REFLECTION

template <class T>
class IVar : public IReflectionVar
{
public:
	virtual ~IVar(void) {}

	template <class VarType>
	const VarType& getDataT(const T& object) const;

	template <class VarType>
	void setDataT(T& object, const VarType& data);

	template <class VarType>
	void setDataMoveT(T& object, VarType&& data);

	template <class VarType>
	const VarType& getElementT(const T& object, int32_t index) const;

	template <class VarType>
	void setElementT(T& object, int32_t index, const VarType& data);

	template <class VarType>
	void setElementMoveT(T& object, int32_t index, VarType&& data);

	virtual bool load(const Shibboleth::ISerializeReader& reader, T& object) = 0;
	virtual void save(Shibboleth::ISerializeWriter& writer, const T& object) = 0;
};



template <class T, class VarType>
class Var final : public IVar<T>
{
public:
	Var(VarType T::*ptr);

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
	VarType T::* _ptr = nullptr;
};



template <class T, class VarType>
struct VarPtrTypeHelper final
{
	static IVar<T>* Create(
		eastl::u8string_view name,
		VarType T::* ptr,
		const Shibboleth::ProxyAllocator& allocator,
		Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& extra_vars
	);
};

NS_END