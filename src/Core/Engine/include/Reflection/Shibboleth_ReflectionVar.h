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
#include "Shibboleth_HashString.h"
#include <EASTL/string_view.h>
#include <EASTL/array.h>

NS_REFLECTION

template <class T>
class IVar;

template <class T, class VarType>
class Var;

template <class T, class VarType, class Vec_Allocator>
class VectorVar;

template <class T, class VarType, size_t array_size>
class ArrayVar;



template <class T, class VarType>
struct VarTypeHelper final
{
	using Type = Var<T, VarType>;
};

template <class T, class VarType, class Vec_Allocator>
struct VarTypeHelper< T, Gaff::Vector<VarType, Vec_Allocator> > final
{
	using Type = VectorVar<T, VarType, Vec_Allocator>;
};

template <class T, class VarType, size_t array_size>
struct VarTypeHelper< T, VarType (T::*)[array_size] > final
{
	using Type = ArrayVar<T, VarType, array_size>;
};



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
	explicit Var(ptrdiff_t offset);
	explicit Var(VarType T::*ptr);
	Var(void) = default;

	const IReflection& getReflection(void) const override;
	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	void setOffset(ptrdiff_t offset);

private:
	ptrdiff_t _offset = -1;
};

template <class T, class VarType, class Vec_Allocator>
class VectorVar final : public IVar<T>
{
public:
	using VectorType = Gaff::Vector<VarType, Vec_Allocator>;

	VectorVar(VectorType T::*ptr);

	const IReflection& getReflection(void) const override;
	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool isVector(void) const override { return true; }
	int32_t size(const void* object) const override;

	const void* getElement(const void* object, int32_t index) const override;
	void* getElement(void* object, int32_t index) override;
	void setElement(void* object, int32_t index, const void* data) override;
	void setElementMove(void* object, int32_t index, void* data) override;
	void swap(void* object, int32_t index_a, int32_t index_b) override;
	void resize(void* object, size_t new_size) override;
	void remove(void* object, int32_t index) override;

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

	VectorType T::* _ptr = nullptr;
};

template <class T, class VarType, size_t array_size>
class ArrayVar final : public IVar<T>
{
public:
	ArrayVar(VarType (T::*ptr)[array_size]);

	const IReflection& getReflection(void) const override;
	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool isFixedArray(void) const override { return true; }
	int32_t size(const void*) const override { return static_cast<int32_t>(array_size); }

	const void* getElement(const void* object, int32_t index) const override;
	void* getElement(void* object, int32_t index) override;
	void setElement(void* object, int32_t index, const void* data) override;
	void setElementMove(void* object, int32_t index, void* data) override;
	void swap(void* object, int32_t index_a, int32_t index_b) override;

	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

	const Shibboleth::Vector<IReflectionVar::SubVarData>& getSubVars(void) override;
	void setSubVarBaseName(eastl::u8string_view base_name) override;

private:
	using RefVarType = VarTypeHelper<T, VarType>::Type;

	Shibboleth::Vector<IReflectionVar::SubVarData> _cached_element_vars{ Shibboleth::ProxyAllocator("Reflection") };
	eastl::array<RefVarType, array_size> _elements;

	VarType (T::*_ptr)[array_size] = nullptr;
};

NS_END
