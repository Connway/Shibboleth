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

#include "Shibboleth_Reflection.h"
#include "Shibboleth_Utilities.h"

NS_REFLECTION

template <class T>
class ReflectionOfType final
{
public:
	static_assert(Reflection< std::decay_t<T> >::k_has_reflection || Hash::template ClassHashable< std::decay_t<T> >::k_is_hashable, "Type T does not have reflection or is not hashable.");

	template <class U>
	ReflectionOfType(const ReflectionDefinition<U>& ref_def):
		ReflectionOfType(static_cast<const IReflectionDefinition&>(ref_def))
	{
		static_assert(std::is_base_of_v<T, U>, "Type U is not derived from type T.");
	}

	ReflectionOfType(const IReflectionDefinition& ref_def):
		ReflectionOfType(&ref_def)
	{
	}

	ReflectionOfType(const IReflectionDefinition* ref_def):
		_ref_def(ref_def)
	{
		if constexpr (Reflection< std::decay_t<T> >::k_has_reflection) {
			GAFF_ASSERT_MSG(
				!ref_def || ref_def->template hasInterface< std::decay_t<T> >(),
				"ReflectionOfType: '%s' does not implement or derive from '%s'.",
				ref_def->getReflectionInstance().getName(),
				Reflection< std::decay_t<T> >::GetName()
			);

		} else {
			GAFF_ASSERT_MSG(
				!ref_def || ref_def->template hasInterface< std::decay_t<T> >(),
				"ReflectionOfType: '%s' does not implement or derive from '%s'.",
				ref_def->getReflectionInstance().getName(),
				Hash::template ClassHashable< std::decay_t<T> >::GetName().data.data()
			);
		}
	}

	template <class U>
	ReflectionOfType(const ReflectionOfType<U>& refl):
		ReflectionOfType(refl._ref_def)
	{
		static_assert(std::is_base_of_v<T, U>, "Type U is not derived from type T.");
	}

	template <class U>
	ReflectionOfType& operator=(const ReflectionOfType<U>& rhs)
	{
		static_assert(std::is_base_of_v<T, U>, "Type U is not derived from type T.");

		*this = rhs._ref_def;
		return *this;
	}

	ReflectionOfType& operator=(const IReflectionDefinition& ref_def)
	{
		*this = &ref_def;
		return *this;
	}

	ReflectionOfType& operator=(const IReflectionDefinition* ref_def)
	{
		if constexpr (Reflection< std::decay_t<T> >::k_has_reflection) {
			GAFF_ASSERT_MSG(
				!ref_def || ref_def->template hasInterface< std::decay_t<T> >(),
				"ReflectionOfType: '%s' does not implement or derive from '%s'.",
				ref_def->getReflectionInstance().getName(),
				Reflection< std::decay_t<T> >::GetName()
			);

		} else {
			GAFF_ASSERT_MSG(
				!ref_def || ref_def->template hasInterface< std::decay_t<T> >(),
				"ReflectionOfType: '%s' does not implement or derive from '%s'.",
				ref_def->getReflectionInstance().getName(),
				Hash::template ClassHashable< std::decay_t<T> >::GetName().data.data()
			);
		}

		_ref_def = ref_def;
		return *this;
	}

	template <class U>
	operator ReflectionOfType<U>(void) const
	{
		static_assert(std::is_base_of_v<U, T>, "Type T is not derived from type U.");
		return ReflectionOfType<U>{ _ref_def };
	}


	ReflectionOfType& operator=(const ReflectionOfType&) = default;
	ReflectionOfType(const ReflectionOfType&) = default;
	ReflectionOfType(void) = default;


	const IReflectionDefinition* get(void) const { return _ref_def; }

	template <class U>
	auto operator<=>(const ReflectionOfType<U>& rhs) const { return _ref_def <=> rhs._ref_def; }
	auto operator<=>(const IReflectionDefinition& rhs) const { return _ref_def <=> &rhs; }
	auto operator<=>(const IReflectionDefinition* rhs) const { return _ref_def <=> rhs; }

	operator const IReflectionDefinition*(void) const { return _ref_def; }
	operator bool(void) const { return _ref_def != nullptr; }

	const IReflectionDefinition& operator*(void) const
	{
		GAFF_ASSERT(_ref_def);
		return *_ref_def;
	}

private:
	const IReflectionDefinition* _ref_def = nullptr;

	template <class Derived, class Base>
	friend ReflectionOfType<Derived> ReflectionCast(const ReflectionOfType<Base>&);
};



template <class T>
class DeferredReflectionOfType final
{
public:
	static_assert(Reflection< std::decay_t<T> >::k_has_reflection || Hash::template ClassHashable< std::decay_t<T> >::k_is_hashable, "Type T does not have reflection or is not hashable.");

	const IReflectionDefinition* retrieve(void)
	{
		if (_ref_def) {
			return _ref_def;
		} else if (_type_name.empty()) {
			return nullptr;
		}

		const Shibboleth::ReflectionManager& ref_mgr = Shibboleth::GetApp().getReflectionManager();
		_ref_def = ref_mgr.getReflection(_type_name.getHash());

		if (!_ref_def) {
			// $TODO: Log error.
		}

		return _ref_def;
	}

	const IReflectionDefinition* get(void) const { return _ref_def; }


	template <class U>
	DeferredReflectionOfType(const ReflectionDefinition<U>& ref_def):
		_ref_def(ref_def)
	{
		static_assert(std::is_base_of_v<T, U>, "Type U is not derived from type T.");
	}

	DeferredReflectionOfType(const IReflectionDefinition& ref_def):
		DeferredReflectionOfType(&ref_def)
	{
	}

	DeferredReflectionOfType(const IReflectionDefinition* ref_def):
		_ref_def(ref_def)
	{
		if (ref_def) {
			_type_name = ref_def->getReflectionInstance().getName();
		}
	}

	template <class U>
	DeferredReflectionOfType(const DeferredReflectionOfType<U>& refl):
		DeferredReflectionOfType(refl._ref_def)
	{
		static_assert(std::is_base_of_v<T, U>, "Type U is not derived from type T.");
	}

	template <class U>
	DeferredReflectionOfType(const ReflectionOfType<U>& refl):
		DeferredReflectionOfType(refl.get())
	{
		static_assert(std::is_base_of_v<T, U>, "Type U is not derived from type T.");
	}

	DeferredReflectionOfType(const DeferredReflectionOfType&) = default;
	DeferredReflectionOfType(DeferredReflectionOfType&&) = default;

	DeferredReflectionOfType& operator=(const DeferredReflectionOfType&) = default;
	DeferredReflectionOfType& operator=(DeferredReflectionOfType&&) = default;


	template <class U>
	DeferredReflectionOfType& operator=(const ReflectionOfType<U>& rhs)
	{
		static_assert(std::is_base_of_v<T, U>, "Type U is not derived from type T.");

		*this = rhs.get();
		return *this;
	}

	DeferredReflectionOfType& operator=(const IReflectionDefinition& ref_def)
	{
		*this = &ref_def;
		return *this;
	}

	DeferredReflectionOfType& operator=(const IReflectionDefinition* ref_def)
	{
		_ref_def = ref_def;

		if (ref_def) {
			_type_name = ref_def->getReflectionInstance().getName();
		} else {
			_type_name.clear();
		}

		return *this;
	}

	template <class U>
	auto operator<=>(const DeferredReflectionOfType<U>& rhs) const { return _ref_def <=> rhs._ref_def; }

	template <class U>
	auto operator<=>(const ReflectionOfType<U>& rhs) const { return _ref_def <=> rhs; }

	auto operator<=>(const IReflectionDefinition& rhs) const { return _ref_def <=> rhs; }
	auto operator<=>(const IReflectionDefinition* rhs) const { return _ref_def <=> rhs; }

	operator const IReflectionDefinition*(void) const { return _ref_def; }
	operator ReflectionOfType<T>(void) const { return _ref_def; }
	operator bool(void) const { return _ref_def; }

	const IReflectionDefinition& operator*(void) const
	{
		GAFF_ASSERT(_ref_def);
		return *_ref_def;
	}

private:
	Shibboleth::HashString64<> _type_name{ REFLECTION_ALLOCATOR };
	ReflectionOfType<T> _ref_def;

	template <class V, class W>
	friend class VarDeferredReflectionOfType;

	template <class Derived, class Base>
	friend DeferredReflectionOfType<Derived> ReflectionCast(const DeferredReflectionOfType<Base>&);

	template <class Derived, class Base>
	friend DeferredReflectionOfType<Derived> ReflectionCast(DeferredReflectionOfType<Base>&&);
};



template <class Derived, class Base>
DeferredReflectionOfType<Derived> ReflectionCast(DeferredReflectionOfType<Base>&& refl_type)
{
	if constexpr (std::is_same_v<Base, Derived>) {
		return DeferredReflectionOfType<Derived> { refl_type };

	} else {
		static_assert(Reflection< std::decay_t<Derived> >::k_has_reflection || Hash::template ClassHashable< std::decay_t<Derived> >::k_is_hashable, "Derived needs to either have reflection or be hashable.");
		static_assert(Reflection< std::decay_t<Base> >::k_has_reflection || Hash::template ClassHashable< std::decay_t<Base> >::k_is_hashable, "Base needs to either have reflection or be hashable.");

		if constexpr (Reflection< std::decay_t<Derived> >::k_has_reflection && Reflection< std::decay_t<Base> >::k_has_reflection) {
			GAFF_ASSERT_MSG(
				Reflection< std::decay_t<Derived> >::GetReflectionDefinition().template hasInterface<Base>(),
				"ReflectionCast<DeferredReflectionOfType>: '%s' does not implement or derive from '%s'.",
				Reflection< std::decay_t<Derived> >::GetName(),
				Reflection< std::decay_t<Base> >::GetName()
			);

		} else if constexpr (Reflection< std::decay_t<Derived> >::k_has_reflection && !Reflection< std::decay_t<Base> >::k_has_reflection) {
			GAFF_ASSERT_MSG(
				Reflection< std::decay_t<Derived> >::GetReflectionDefinition().hasInterface(Hash::template ClassHashable< std::decay_t<Base> >::GetHash()),
				"ReflectionCast<DeferredReflectionOfType>: '%s' does not implement or derive from '%s'.",
				Reflection< std::decay_t<Derived> >::GetName(),
				Hash::template ClassHashable< std::decay_t<Base> >::GetName().data.data()
			);

		} else if constexpr (!Reflection< std::decay_t<Derived> >::k_has_reflection && !Reflection< std::decay_t<Base> >::k_has_reflection) {
			static_assert(std::is_base_of_v<Base, Derived>, "Derived is not derived from Base.");

		} else {
			static_assert(false, "Derived must have reflection or both Derived and Base be hashable.");
		}

		DeferredReflectionOfType<Derived> value;
		value._type_name = std::move(refl_type._type_name);
		value._ref_def = refl_type._ref_def.get();

		return value;
	}
}

template <class Derived, class Base>
DeferredReflectionOfType<Derived> ReflectionCast(const DeferredReflectionOfType<Base>& refl_type)
{
	if constexpr (std::is_same_v<Base, Derived>) {
		return DeferredReflectionOfType<Derived> { refl_type };

	} else {
		static_assert(Reflection< std::decay_t<Derived> >::k_has_reflection || Hash::template ClassHashable< std::decay_t<Derived> >::k_is_hashable, "Derived needs to either have reflection or be hashable.");
		static_assert(Reflection< std::decay_t<Base> >::k_has_reflection || Hash::template ClassHashable< std::decay_t<Base> >::k_is_hashable, "Base needs to either have reflection or be hashable.");

		if constexpr (Reflection< std::decay_t<Derived> >::k_has_reflection && Reflection< std::decay_t<Base> >::k_has_reflection) {
			GAFF_ASSERT_MSG(
				Reflection< std::decay_t<Derived> >::GetReflectionDefinition().template hasInterface<Base>(),
				"ReflectionCast<DeferredReflectionOfType>: '%s' does not implement or derive from '%s'.",
				Reflection< std::decay_t<Derived> >::GetName(),
				Reflection< std::decay_t<Base> >::GetName()
			);

		} else if constexpr (Reflection< std::decay_t<Derived> >::k_has_reflection && !Reflection< std::decay_t<Base> >::k_has_reflection) {
			GAFF_ASSERT_MSG(
				Reflection< std::decay_t<Derived> >::GetReflectionDefinition().hasInterface(Hash::template ClassHashable< std::decay_t<Base> >::GetHash()),
				"ReflectionCast<DeferredReflectionOfType>: '%s' does not implement or derive from '%s'.",
				Reflection< std::decay_t<Derived> >::GetName(),
				Hash::template ClassHashable< std::decay_t<Base> >::GetName().data.data()
			);

		} else if constexpr (!Reflection< std::decay_t<Derived> >::k_has_reflection && !Reflection< std::decay_t<Base> >::k_has_reflection) {
			static_assert(std::is_base_of_v<Base, Derived>, "Derived is not derived from Base.");

		} else {
			static_assert(false, "Derived must have reflection or both Derived and Base be hashable.");
		}

		DeferredReflectionOfType<Derived> value;
		value._type_name = refl_type._type_name;
		value._ref_def = refl_type._ref_def.get();

		return value;
	}
}

template <class Derived, class Base>
ReflectionOfType<Derived> ReflectionCast(const ReflectionOfType<Base>& refl_type)
{
	if constexpr (std::is_same_v<Base, Derived>) {
		return ReflectionOfType<Derived> { refl_type };

	} else {
		static_assert(Reflection< std::decay_t<Derived> >::k_has_reflection || Hash::template ClassHashable< std::decay_t<Derived> >::k_is_hashable, "Derived needs to either have reflection or be hashable.");
		static_assert(Reflection< std::decay_t<Base> >::k_has_reflection || Hash::template ClassHashable< std::decay_t<Base> >::k_is_hashable, "Base needs to either have reflection or be hashable.");

		if constexpr (Reflection< std::decay_t<Derived> >::k_has_reflection && Reflection< std::decay_t<Base> >::k_has_reflection) {
			GAFF_ASSERT_MSG(
				Reflection< std::decay_t<Derived> >::GetReflectionDefinition().template hasInterface<Base>(),
				"ReflectionCast<ReflectionOfType>: '%s' does not implement or derive from '%s'.",
				Reflection< std::decay_t<Derived> >::GetName(),
				Reflection< std::decay_t<Base> >::GetName()
			);

		} else if constexpr (Reflection< std::decay_t<Derived> >::k_has_reflection && !Reflection< std::decay_t<Base> >::k_has_reflection) {
			GAFF_ASSERT_MSG(
				Reflection< std::decay_t<Derived> >::GetReflectionDefinition().hasInterface(Hash::template ClassHashable< std::decay_t<Base> >::GetHash()),
				"ReflectionCast<ReflectionOfType>: '%s' does not implement or derive from '%s'.",
				Reflection< std::decay_t<Derived> >::GetName(),
				Hash::template ClassHashable< std::decay_t<Base> >::GetName().data.data()
			);

		} else if constexpr (!Reflection< std::decay_t<Derived> >::k_has_reflection && !Reflection< std::decay_t<Base> >::k_has_reflection) {
			static_assert(std::is_base_of_v<Base, Derived>, "Derived is not derived from Base.");

		} else {
			static_assert(false, "Derived must have reflection or both Derived and Base be hashable.");
		}

		ReflectionOfType<Derived> value;
		value._ref_def = refl_type.get();

		return value;
	}
}

NS_END
