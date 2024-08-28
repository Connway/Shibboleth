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
#include <Gaff_IncludeEASTLArray.h>

NS_REFLECTION

template <class T>
struct DimensionsHelper final
{
	static constexpr int32_t Dimensions = 0;
};

template <class T, class Allocator>
struct DimensionsHelper< Gaff::Vector<T, Allocator> > final
{
	static constexpr int32_t Dimensions = 1 + DimensionsHelper<T>::Dimensions;
};

template <class T, size_t array_size>
struct DimensionsHelper< eastl::array<T, array_size> > final
{
	static constexpr int32_t Dimensions = 1 + DimensionsHelper<T>::Dimensions;
};

template <class T>
static constexpr int32_t Dimensions = DimensionsHelper<T>::Dimensions;



template <class T, class ContainerType>
class VectorVar;



template <class T, class VarType, class Vec_Allocator>
struct VarTypeHelper< T, Gaff::Vector<VarType, Vec_Allocator> > final
{
	using ReflectionType = VarTypeHelper<T, VarType>::ReflectionType;
	using ContainerType = Gaff::Vector<VarType, Vec_Allocator>;
	using VariableType = VarType;
	using Type = VectorVar<T, ContainerType>;

	// $TODO: Support for N-dimensional vectors.
	static constexpr int32_t Dimensions = Refl::Dimensions< Gaff::Vector<VarType, Vec_Allocator> >;

	static constexpr bool k_can_copy = VarTypeHelper<T, VarType>::k_can_copy;
	static constexpr bool k_can_move = true;

	static constexpr bool k_is_fixed_array = false;
};

template <class T, class VarType, size_t array_size>
struct VarTypeHelper< T, eastl::array<VarType, array_size> > final
{
	using ReflectionType = VarTypeHelper<T, VarType>::ReflectionType;
	using ContainerType = eastl::array<VarType, array_size>;
	using VariableType = VarType;
	using Type = VectorVar<T, ContainerType>;

	// $TODO: Support for N-dimensional arrays.
	static constexpr int32_t Dimensions = Refl::Dimensions< eastl::array<VarType, array_size> >;

	static constexpr bool k_can_copy = VarTypeHelper<T, VarType>::k_can_copy;
	static constexpr bool k_can_move = true;

	static constexpr bool k_is_fixed_array = true;
};



template <class T, class ContainerType>
class VectorVar final : public IVar<T>
{
public:
	using ReflectionType = VarTypeHelper<T, ContainerType>::ReflectionType;
	using VarType = VarTypeHelper<T, ContainerType>::VariableType;

	static_assert(Reflection<ReflectionType>::HasReflection);

	explicit VectorVar(ContainerType T::*ptr);

	static const Reflection<ReflectionType>& GetReflection(void);
	const IReflection& getReflection(void) const override;

	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool isVector(void) const override;
	bool isFixedArray(void) const override;
	int32_t size(const void* object) const override;

	const void* getElement(const void* object, int32_t index) const override;
	void* getElement(void* object, int32_t index) override;
	void setElement(void* object, int32_t index, const void* data) override;
	void setElementMove(void* object, int32_t index, void* data) override;
	void swap(void* object, int32_t index_a, int32_t index_b) override;
	void resize(void* object, size_t new_size) override;
	void remove(void* object, int32_t index) override;

	bool load(const Shibboleth::ISerializeReader& reader, void* object) override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object) override;
	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	const Shibboleth::Vector<IReflectionVar::SubVarData>& getSubVars(void) override;
	void setSubVarBaseName(eastl::u8string_view base_name) override;
	void regenerateSubVars(int32_t range_begin, int32_t range_end);

private:
	using RefVarType = VarTypeHelper<T, VarType>::Type;

	Shibboleth::Vector<IReflectionVar::SubVarData> _cached_element_vars{ Shibboleth::ProxyAllocator("Reflection") };
	Shibboleth::Vector<RefVarType> _elements{ Shibboleth::ProxyAllocator("Reflection") };
	eastl::u8string_view _base_name;
};

NS_END
