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

NS_SHIBBOLETH

template <class T, class VarType>
class VarDeferredResourcePtr;

NS_END



NS_REFLECTION

template <class T, class VarType>
struct VarTypeHelper< T, Shibboleth::DeferredResourcePtr<VarType> > final
{
	using ReflectionType = VarTypeHelper<T, VarType>::ReflectionType;
	using VariableType = VarType;
	using Type = Shibboleth::VarDeferredResourcePtr<T, VarType>;

	static constexpr bool k_can_copy = false;
	static constexpr bool k_can_move = false;
};

NS_END



NS_SHIBBOLETH

template <class T, class VarType>
class VarDeferredResourcePtr final : public Refl::IVar<T>
{
public:
	using ReflectionType = Refl::VarTypeHelper<T, VarType>::ReflectionType;

	VarDeferredResourcePtr(DeferredResourcePtr<VarType> T::* ptr);
	VarDeferredResourcePtr(void) = default;

	static const Refl::Reflection<ReflectionType>& GetReflection(void);
	const Refl::IReflection& getReflection(void) const override;

	const void* getData(const void* object) const override;
	void* getData(void* object) override;
	void setData(void* object, const void* data) override;
	void setDataMove(void* object, void* data) override;

	bool load(const ISerializeReader& reader, void* object) override;
	void save(ISerializeWriter& writer, const void* object) override;

	bool load(const ISerializeReader& reader, T& object) override;
	void save(ISerializeWriter& writer, const T& object) override;
};

NS_END

#include "Shibboleth_DeferredResourcePtrReflection.inl"
