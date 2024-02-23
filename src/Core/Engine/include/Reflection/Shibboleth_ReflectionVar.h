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

#include "Shibboleth_SerializeInterfaces.h"
#include "Shibboleth_IReflection.h"
#include "Shibboleth_HashString.h"
#include <EASTL/string_view.h>

NS_REFLECTION

template <class T>
class IVar;

template <class T, class VarType>
class Var;



template <class T, class VarType>
struct VarTypeHelper final
{
	using ReflectionType = VarType;
	using VariableType = VarType;
	using Type = Var<T, VarType>;
	static constexpr bool k_can_copy = requires(T lhs, const T& rhs) { lhs = rhs; } || requires(const T& rhs) { T(rhs); };
};



template <class T>
class IVar : public IReflectionVar
{
public:
	using IReflectionVar::load;
	using IReflectionVar::save;

	template <class VarType>
	IVar(VarType T::*ptr);

	IVar(ptrdiff_t offset);
	IVar(void) = default;

	virtual ~IVar(void) {}

	const IReflection& getOwnerReflection(void) const override;

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

	void setOffset(ptrdiff_t offset);

	const IVar<T>* getParent(void) const;
	void setParent(IVar<T>* parent);

	const void* adjust(const void* object) const;
	void* adjust(void* object);

	template <class VarType>
	const VarType* get(const void* object) const;

	template <class VarType>
	VarType* get(void* object);

protected:
	IVar<T>* _parent = nullptr;
	ptrdiff_t _offset = 0;
};



template <class T, class VarType>
class Var : public IVar<T>
{
public:
	using ReflectionType = VarTypeHelper<T, VarType>::ReflectionType;

	static_assert(Reflection<ReflectionType>::HasReflection);
	static_assert(Reflection<T>::HasReflection);

	explicit Var(VarType T::*ptr);
	Var(void) = default;

	static const Reflection<ReflectionType>& GetReflection(void);
	const IReflection& getReflection(void) const override;

	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool load(const Shibboleth::ISerializeReader& reader, void* object) override;
	void save(Shibboleth::ISerializeWriter& writer, const void* object) override;

	bool load(const Shibboleth::ISerializeReader& reader, T& object) override;
	void save(Shibboleth::ISerializeWriter& writer, const T& object) override;
};

NS_END



#define SHIB_REFLECTION_TEMPLATE_VAR_WITH_BASE(ClassType, BaseType) \
	NS_REFLECTION \
	template <class T, class VarType> \
	struct VarTypeHelper< T, ClassType<VarType> > final \
	{ \
		using ReflectionType = BaseType; \
		using VariableType = ClassType<VarType>; \
		using Type = Var< T, ClassType<VarType> >; \
		static constexpr bool k_can_copy = true; \
	}; \
	NS_END

#define SHIB_REFLECTION_VAR_WITH_BASE(ClassType, BaseType) \
	NS_REFLECTION \
	template <class T> \
	struct VarTypeHelper<T, ClassType> final \
	{ \
		using ReflectionType = BaseType; \
		using VariableType = ClassType; \
		using Type = Var<T, ClassType>; \
		static constexpr bool k_can_copy = true; \
	}; \
	NS_END

#define SHIB_REFLECTION_TEMPLATE_VAR(ClassType) SHIB_REFLECTION_TEMPLATE_VAR_WITH_BASE(ClassType, ClassType<VarType>)
#define SHIB_REFLECTION_VAR(ClassType) SHIB_REFLECTION_VAR_WITH_BASE(ClassType, ClassType)



#define SHIB_REFLECTION_TEMPLATE_VAR_NO_COPY_WITH_BASE(ClassType, BaseType) \
	NS_REFLECTION \
	template <class T, class VarType> \
	struct VarTypeHelper< T, ClassType<VarType> > final \
	{ \
		using ReflectionType = BaseType; \
		using VariableType = ClassType<VarType>; \
		using Type = Var< T, ClassType<VarType> >; \
		static constexpr bool k_can_copy = false; \
	}; \
	NS_END

#define SHIB_REFLECTION_VAR_NO_COPY_WITH_BASE(ClassType, BaseType) \
	NS_REFLECTION \
	template <class T> \
	struct VarTypeHelper<T, ClassType> final \
	{ \
		using ReflectionType = BaseType; \
		using VariableType = ClassType; \
		using Type = Var<T, ClassType>; \
		static constexpr bool k_can_copy = false; \
	}; \
	NS_END

#define SHIB_REFLECTION_TEMPLATE_VAR_NO_COPY(ClassType) SHIB_REFLECTION_TEMPLATE_VAR_NO_COPY_WITH_BASE(ClassType, ClassType<VarType>)
#define SHIB_REFLECTION_VAR_NO_COPY(ClassType) SHIB_REFLECTION_VAR_NO_COPY_WITH_BASE(ClassType, ClassType)
