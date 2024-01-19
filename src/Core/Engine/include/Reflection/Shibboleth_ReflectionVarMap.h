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
#include "Shibboleth_VectorMap.h"

NS_REFLECTION

template <class T, class ContainerType>
class MapVar;



template <class T, class KeyType, class ValueType, class VecMap_Allocator>
struct VarTypeHelper< T, Gaff::VectorMap<KeyType, ValueType, VecMap_Allocator> > final
{
	using KeyReflectionType = VarTypeHelper<T, KeyType>::ReflectionType;
	using ValueReflectionType = VarTypeHelper<T, ValueType>::ReflectionType;
	using KeyVariableType = KeyType;
	using ValueVariableType = ValueType;
	using ContainerType = Gaff::VectorMap<KeyType, ValueType, VecMap_Allocator>;
	using Type = MapVar<T, ContainerType>;

	static constexpr bool k_key_can_copy = VarTypeHelper<T, KeyType>::k_can_copy;
	static constexpr bool k_value_can_copy = VarTypeHelper<T, ValueType>::k_can_copy;
};



template <class T, class ContainerType>
class MapVar final : public IVar<T>
{
public:
	using KeyReflectionType = VarTypeHelper<T, ContainerType>::KeyReflectionType;
	using ValueReflectionType = VarTypeHelper<T, ContainerType>::ValueReflectionType;
	using KeyVarType = VarTypeHelper<T, ContainerType>::KeyVariableType;
	using ValueVarType = VarTypeHelper<T, ContainerType>::ValueVariableType;

	static_assert(Reflection<KeyReflectionType>::HasReflection);
	static_assert(Reflection<ValueReflectionType>::HasReflection);

	explicit MapVar(ContainerType T::*ptr);
	MapVar(void);

	static const Reflection<KeyReflectionType>& GetReflectionKey(void);
	static const Reflection<ValueReflectionType>& GetReflectionValue(void);

	const IReflection& getReflectionKey(void) const override;
	const IReflection& getReflection(void) const override;

	int32_t getMapEntryIndex(const void* object, const void* key) override;
	const void* getMapEntry(const void* object, const void* key) const override;
	void* getMapEntry(void* object, const void* key) override;

	void* addMapEntry(void* object, const void* key, const void* value) override;
	void* addMapEntryMove(void* object, void* key, void* value) override;
	void* addMapEntry(void* object, const void* key) override;
	void* addMapEntryMove(void* object, void* key) override;

	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool isMap(void) const override;
	int32_t size(const void* object) const override;

	const void* getElement(const void* object, int32_t index) const override;
	void* getElement(void* object, int32_t index) override;
	void setElement(void* object, int32_t index, const void* data) override;
	void setElementMove(void* object, int32_t index, void* data) override;
	void swap(void* object, int32_t index_a, int32_t index_b) override;

	bool load(const Shibboleth::ISerializeReader& reader, void* object) override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object) override;
	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	//const Shibboleth::Vector<IReflectionVar::SubVarData>& getSubVars(void) override;
	void setSubVarBaseName(eastl::u8string_view base_name) override;
	void regenerateSubVars(int32_t range_begin, int32_t range_end);

private:
	using RefKeyVarType = VarTypeHelper<T, KeyVarType>::Type;
	using RefValueVarType = VarTypeHelper<T, ValueVarType>::Type;
	using RefVarType = eastl::pair<RefKeyVarType, RefValueVarType>;

	//Shibboleth::Vector<IReflectionVar::SubVarData> _cached_element_vars{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::Vector<RefVarType> _elements{ Shibboleth::ProxyAllocator("Reflection") };
	eastl::u8string_view _base_name;
};

NS_END
