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

#include "Attributes/Shibboleth_InstancedAttributes.h"
#include "Shibboleth_Utilities.h"
#include "Shibboleth_IApp.h"

NS_SHIBBOLETH

template <class T, class VarType>
class VarInstancedArray;

NS_END


NS_REFLECTION

template <class T, class VarType>
struct VarTypeHelper< T, Shibboleth::InstancedArray<VarType> > final
{
	using ReflectionType = VarTypeHelper<T, VarType>::ReflectionType;
	using VariableType = VarType;
	using Type = Shibboleth::VarInstancedArray<T, VarType>;
	static constexpr bool k_can_copy = false;
};

NS_END


NS_SHIBBOLETH

template <class T, class VarType>
class VarInstancedArray final : public Refl::IVar<T>
{
public:
	using ReflectionType = Refl::VarTypeHelper<T, VarType>::ReflectionType;

	VarInstancedArray(InstancedArray<VarType> T::* ptr);
	VarInstancedArray(void) = default;

	static const Refl::Reflection<ReflectionType>& GetReflection(void);
	const Refl::IReflection& getReflection(void) const override;

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

	bool load(const ISerializeReader& reader, void* object) override;
	void save(ISerializeWriter& writer, const void* object) override;

	bool load(const ISerializeReader& reader, T& object) override;
	void save(ISerializeWriter& writer, const T& object) override;

	const InstancedOptionalAttribute* getOptionalAttribute(void) const;

	const Vector<Refl::IReflectionVar::SubVarData>& getSubVars(void) override;
	void setSubVarBaseName(eastl::u8string_view base_name) override;
	void regenerateSubVars(const void* object, int32_t range_begin, int32_t range_end);

private:
	// $TODO: Make a var type that wraps this and overrides the adjust functions.
	using RefVarType = Refl::VarTypeHelper<T, VarType>::Type;

	class VarElementWrapper final : public RefVarType
	{
	public:
		const Refl::IReflection& getReflection(void) const override { GAFF_ASSERT(_reflection); return *_reflection; }
		void setReflection(const Refl::IReflection& reflection) { _reflection = &reflection; }

	private:
		const Refl::IReflection* _reflection = &Refl::Reflection<ReflectionType>::GetInstance();
	};

	Vector<Refl::IReflectionVar::SubVarData> _cached_element_vars{ ProxyAllocator("Reflection") };
	Vector<VarElementWrapper> _elements{ ProxyAllocator("Reflection") };
	eastl::u8string_view _base_name;
};

NS_END

#include "Shibboleth_InstancedArrayReflection.inl"