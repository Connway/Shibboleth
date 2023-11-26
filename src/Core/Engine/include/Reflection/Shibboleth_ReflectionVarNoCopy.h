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

private:
	VarType T::* _ptr = nullptr;
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

	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;

private:
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

private:
	VarType (T::*_ptr)[array_size] = nullptr;
};

NS_END


#define SHIB_REFLECTION_TEMPLATE_VAR_NO_COPY_WITH_BASE(ClassType, BaseType) \
	NS_REFLECTION \
	template <class T, class VarType> \
	struct VarPtrTypeHelper< T, ClassType<VarType> > final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			ClassType<VarType> T::* ptr, \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			static_assert(!std::is_reference<VarType>::value, "Cannot reflect references."); \
			static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers."); \
			static_assert(!std::is_const<VarType>::value, "Cannot reflect const values."); \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(VarNoCopy<T, ClassType<VarType>, BaseType>), allocator, ptr); \
			return var_ptr; \
		} \
	}; \
	template <class T, class VarType, class Vec_Allocator> \
	struct VarPtrTypeHelper< T, Gaff::Vector<ClassType<VarType>, Vec_Allocator> > final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			Gaff::Vector<ClassType<VarType>, Vec_Allocator> T::* ptr, \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			static_assert(!std::is_reference<VarType>::value, "Cannot reflect references."); \
			static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers."); \
			static_assert(!std::is_const<VarType>::value, "Cannot reflect const values."); \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(VectorVarNoCopy<T, ClassType<VarType>, BaseType, Vec_Allocator>), allocator, ptr); \
			return var_ptr; \
		} \
	}; \
	template <class T, class VarType, size_t array_size> \
	struct VarPtrTypeHelper< T, ClassType<VarType> (T::*)[array_size] > final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			ClassType<VarType> (T::*arr)[array_size], \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			static_assert(!std::is_reference<VarType>::value, "Cannot reflect references."); \
			static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers."); \
			static_assert(!std::is_const<VarType>::value, "Cannot reflect const values."); \
			static_assert(array_size > 0, "Cannot reflect a zero size array."); \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(ArrayVarNoCopy<T, ClassType<VarType>, BaseType, array_size>), allocator, arr); \
			return var_ptr; \
		} \
	}; \
	NS_END

#define SHIB_REFLECTION_TEMPLATE_VAR_NO_COPY(ClassType) \
	NS_REFLECTION \
	template <class T, class VarType> \
	struct VarPtrTypeHelper< T, ClassType<VarType> > final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			ClassType<VarType> T::* ptr, \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			static_assert(!std::is_reference<VarType>::value, "Cannot reflect references."); \
			static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers."); \
			static_assert(!std::is_const<VarType>::value, "Cannot reflect const values."); \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(VarNoCopy< T, ClassType<VarType>, ClassType<VarType> >), allocator, ptr); \
			return var_ptr; \
		} \
	}; \
	template <class T, class VarType, class Vec_Allocator> \
	struct VarPtrTypeHelper< T, Gaff::Vector<ClassType<VarType>, Vec_Allocator> > final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			Gaff::Vector<ClassType<VarType>, Vec_Allocator> T::* ptr, \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			static_assert(!std::is_reference<VarType>::value, "Cannot reflect references."); \
			static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers."); \
			static_assert(!std::is_const<VarType>::value, "Cannot reflect const values."); \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(VectorVarNoCopy<T, ClassType<VarType>, ClassType<VarType>, Vec_Allocator>), allocator, ptr); \
			return var_ptr; \
		} \
	}; \
	template <class T, class VarType, size_t array_size> \
	struct VarPtrTypeHelper< T, ClassType<VarType> (T::*)[array_size] > final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			ClassType<VarType> (T::*arr)[array_size], \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			static_assert(!std::is_reference<VarType>::value, "Cannot reflect references."); \
			static_assert(!std::is_pointer<VarType>::value, "Cannot reflect pointers."); \
			static_assert(!std::is_const<VarType>::value, "Cannot reflect const values."); \
			static_assert(array_size > 0, "Cannot reflect a zero size array."); \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(ArrayVarNoCopy<T, ClassType<VarType>, ClassType<VarType>, array_size>), allocator, arr); \
			return var_ptr; \
		} \
	}; \
	NS_END

#define SHIB_REFLECTION_VAR_NO_COPY_WITH_BASE(ClassType, BaseType) \
	NS_REFLECTION \
	template <class T> \
	struct VarPtrTypeHelper<T, ClassType> final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			ClassType T::* ptr, \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(VarNoCopy< T, ClassType, BaseType>), allocator, ptr); \
			return var_ptr; \
		} \
	}; \
	template <class T, class Vec_Allocator> \
	struct VarPtrTypeHelper< T, Gaff::Vector<ClassType, Vec_Allocator> > final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			Gaff::Vector<ClassType, Vec_Allocator> T::* ptr, \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(VectorVarNoCopy<T, ClassType, BaseType, Vec_Allocator>), allocator, ptr); \
			return var_ptr; \
		} \
	}; \
	template <class T, size_t array_size> \
	struct VarPtrTypeHelper< T, ClassType (T::*)[array_size] > final \
	{ \
		static IVar<T>* Create( \
			eastl::u8string_view name, \
			ClassType (T::*arr)[array_size], \
			Shibboleth::ProxyAllocator& allocator, \
			Shibboleth::Vector< eastl::pair<Shibboleth::HashString32<>, IVar<T>*> >& /*extra_vars*/) \
		{ \
			static_assert(array_size > 0, "Cannot reflect a zero size array."); \
			GAFF_ASSERT_MSG(!name.empty(), "Name cannot be an empty string."); \
			IVar<T>* const var_ptr = SHIB_ALLOCT(GAFF_SINGLE_ARG(ArrayVarNoCopy<T, ClassType, BaseType, array_size>), allocator, arr); \
			return var_ptr; \
		} \
	}; \
	NS_END

#define SHIB_REFLECTION_VAR_NO_COPY(ClassType) SHIB_REFLECTION_VAR_NO_COPY_WITH_BASE(ClassType, ClassType)
