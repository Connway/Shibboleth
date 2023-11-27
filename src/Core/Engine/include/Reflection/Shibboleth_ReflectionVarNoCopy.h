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
#include "Shibboleth_SerializeablePtr.h"

NS_REFLECTION

template <class T, class VarType, class ReflectionType = VarType>
class VarNoCopy final : public IVar<T>
{
public:
	static_assert(std::is_base_of_v<ReflectionType, VarType>);

	VarNoCopy(VarType T::*ptr);

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

template <class T, class VarType, class ReflectionType, class Vec_Allocator>
class VectorVarNoCopy final : public IVar<T>
{
public:
	static_assert(std::is_base_of_v<ReflectionType, VarType>);

	using VectorType = Gaff::Vector<VarType, Vec_Allocator>;

	VectorVarNoCopy(VectorType T::*ptr);

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

template <class T, class VarType, class ReflectionType, size_t array_size>
class ArrayVarNoCopy final : public IVar<T>
{
public:
	ArrayVarNoCopy(VarType (T::*ptr)[array_size]);

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


#define SHIB_REFLECTION_TEMPLATE_VAR_NO_COPY_WITH_BASE(ClassType, BaseType) \
	NS_REFLECTION \
	template <class T, class VarType> \
	struct VarTypeHelper< T, ClassType<VarType> > final \
	{ \
		using Type = VarNoCopy<T, ClassType<VarType>, BaseType>; \
	}; \
	template <class T, class VarType, class Vec_Allocator> \
	struct VarTypeHelper< T, Gaff::Vector<ClassType<VarType>, Vec_Allocator> > final \
	{ \
		using Type = VectorVarNoCopy<T, ClassType<VarType>, BaseType, Vec_Allocator>; \
	}; \
	template <class T, class VarType, size_t array_size> \
	struct VarTypeHelper< T, ClassType<VarType> (T::*)[array_size] > final \
	{ \
		using Type = ArrayVarNoCopy<T, ClassType<VarType>, BaseType, array_size>; \
	}; \
	NS_END

#define SHIB_REFLECTION_TEMPLATE_VAR_NO_COPY(ClassType) \
	NS_REFLECTION \
	template <class T, class VarType> \
	struct VarTypeHelper< T, ClassType<VarType> > final \
	{ \
		using Type = VarNoCopy< T, ClassType<VarType>, ClassType<VarType> >; \
	}; \
	template <class T, class VarType, class Vec_Allocator> \
	struct VarTypeHelper< T, Gaff::Vector<ClassType<VarType>, Vec_Allocator> > final \
	{ \
		using Type = VectorVarNoCopy<T, ClassType<VarType>, ClassType<VarType>, Vec_Allocator>; \
	}; \
	template <class T, class VarType, size_t array_size> \
	struct VarTypeHelper< T, ClassType<VarType> (T::*)[array_size] > final \
	{ \
		using Type = ArrayVarNoCopy<T, ClassType<VarType>, ClassType<VarType>, array_size>; \
	}; \
	NS_END

#define SHIB_REFLECTION_VAR_NO_COPY_WITH_BASE(ClassType, BaseType) \
	NS_REFLECTION \
	template <class T> \
	struct VarTypeHelper<T, ClassType> final \
	{ \
		using Type = VarNoCopy<T, ClassType, BaseType>; \
	}; \
	template <class T, class Vec_Allocator> \
	struct VarTypeHelper< T, Gaff::Vector<ClassType, Vec_Allocator> > final \
	{ \
		using Type = VectorVarNoCopy<T, ClassType, BaseType, Vec_Allocator>; \
	}; \
	template <class T, size_t array_size> \
	struct VarTypeHelper< T, ClassType (T::*)[array_size] > final \
	{ \
		using Type = ArrayVarNoCopy<T, ClassType, BaseType, array_size>; \
	}; \
	NS_END

#define SHIB_REFLECTION_VAR_NO_COPY(ClassType) SHIB_REFLECTION_VAR_NO_COPY_WITH_BASE(ClassType, ClassType)
