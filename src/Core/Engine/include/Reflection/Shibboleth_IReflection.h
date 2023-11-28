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

#include "Shibboleth_ReflectionDefines.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_Vector.h"
#include <Gaff_Assert.h>
#include <Gaff_Flags.h>
#include <Gaff_Hash.h>

NS_REFLECTION

class IEnumReflectionDefinition;
class IReflectionDefinition;

class IReflection
{
public:
	virtual ~IReflection(void) {}

	virtual void init(void) = 0;

	virtual bool isDefined(void) const = 0;
	virtual bool isEnum(void) const = 0;

	virtual const char8_t* getName(void) const = 0;
	virtual Gaff::Hash64 getNameHash(void) const = 0;
	virtual Gaff::Hash64 getVersion(void) const = 0;
	virtual int32_t size(void) const = 0;

	virtual const IEnumReflectionDefinition& getEnumReflectionDefinition(void) const = 0;
	virtual const IReflectionDefinition& getReflectionDefinition(void) const = 0;

	IReflection* next = nullptr;
};

template <class T>
class Reflection final : public IReflection
{
public:
	static constexpr bool HasReflection = false;

	static const IReflectionDefinition& GetReflectionDefinition(void)
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		const IReflectionDefinition* const ref_def = nullptr;
		return *ref_def;
	}

	static const Reflection<T>& GetInstance(void)
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		const Reflection<T>* const instance = nullptr;
		return *instance;
	}

	Reflection(void)
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}

	void init(void) override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}

	bool isDefined(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return false;
	}

	bool isEnum(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return false;
	}

	const char8_t* getName(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return u8"Unknown";
	}

	Gaff::Hash64 getNameHash(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return Gaff::Hash64(0);
	}

	Gaff::Hash64 getVersion(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return Gaff::Hash64(0);
	}

	int32_t size(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return 0;
	}

	const IEnumReflectionDefinition& getEnumReflectionDefinition(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		const IEnumReflectionDefinition* const ref_def = nullptr;
		return *ref_def;
	}

	const IReflectionDefinition& getReflectionDefinition(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return GetReflectionDefinition();
	}
};

class IReflectionVar
{
public:
	virtual ~IReflectionVar(void) {}

	template <class DataType>
	void setDataT(void* object, const DataType& data)
	{
		if (isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		setData(object, &data);
	}

	template <class DataType>
	void setDataMoveT(void* object, DataType&& data)
	{
		if (isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		setDataMove(object, &data);
	}

	template <class DataType>
	const DataType& getElementT(const void* object, int32_t index) const
	{
		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		return *reinterpret_cast<const DataType*>(getElement(object, index));
	}

	template <class DataType>
	void setElementT(void* object, int32_t index, const DataType& data)
	{
		if (isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		setElement(object, index, &data);
	}

	template <class DataType>
	void setElementMoveT(void* object, int32_t index, DataType&& data)
	{
		if (isReadOnly()) {
			// $TODO: Log error.
			return;
		}

		using Type = typename std::remove_reference<DataType>::type;

		const auto& other_refl = Reflection<Type>::GetInstance();
		const auto& refl = getReflection();

		GAFF_ASSERT(refl.isEnum() == other_refl.isEnum());

		if constexpr (std::is_enum<Type>::value) {
			GAFF_ASSERT(&refl.getEnumReflectionDefinition() == &other_refl.getEnumReflectionDefinition());
		} else {
			GAFF_ASSERT(&refl.getReflectionDefinition() == &other_refl.getReflectionDefinition());
		}

		GAFF_ASSERT((isFixedArray() || isVector()) && size(object) > index);
		setElementMove(object, index, &data);
	}

	virtual const IReflection& getReflection(void) const = 0;
	virtual const void* getData(const void* object) const = 0;
	virtual void* getData(void* object) = 0;
	virtual void setData(void* object, const void* data) = 0;
	virtual void setDataMove(void* object, void* data) = 0;

	virtual bool isFixedArray(void) const { return false; }
	virtual bool isVector(void) const { return false; }
	virtual bool isFlags(void) const { return false; }
	virtual bool isMap(void) const { return false; }

	virtual const IReflection& getReflectionKey(void) const
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not a vector map!");
		// To calm down Clang.
		IReflection* const ptr = nullptr;
		return *ptr;
	}

	virtual int32_t size(const void*) const
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
		return 0;
	}

	virtual const void* getElement(const void*, int32_t) const
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
		return nullptr;
	}

	virtual void* getElement(void*, int32_t)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
		return nullptr;
	}

	virtual void setElement(void*, int32_t, const void*)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
	}

	virtual void setElementMove(void*, int32_t, void*)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
	}

	virtual void swap(void*, int32_t, int32_t)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not an array or vector!");
	}

	virtual void resize(void*, size_t)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not a vector!");
	}

	virtual void remove(void*, int32_t)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not a vector!");
	}

	virtual void setFlagValue(void*, int32_t, bool)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not flags!");
	}

	virtual bool getFlagValue(const void*, int32_t) const
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not flags!");
		return false;
	}

	using SubVarData = eastl::pair<Shibboleth::HashString32<>, IReflectionVar*>;

	// Pointers should not be cached. Not guaranteed to remain valid.
	virtual const Shibboleth::Vector<SubVarData>& getSubVars(void)
	{
		static const Shibboleth::Vector<SubVarData> k_no_sub_vars;
		return k_no_sub_vars;
	}

	virtual void setSubVarBaseName(eastl::u8string_view /*base_name*/) {}


	void setReadOnly(bool read_only) { _flags.set(read_only, Flag::ReadOnly); }
	bool isReadOnly(void) const { return _flags.testAll(Flag::ReadOnly); }

	void setNoSerialize(bool no_serialize) { _flags.set(no_serialize, Flag::NoSerialize); }
	bool canSerialize(void) const { return !_flags.testAll(Flag::NoSerialize); }

	void setOptional(bool optional) { _flags.set(optional, Flag::Optional); }
	bool isOptional(void) const { return _flags.testAll(Flag::Optional); }

	void setNoCopy(bool optional) { _flags.set(optional, Flag::NoCopy); }
	bool isNoCopy(void) const { return _flags.testAll(Flag::NoCopy); }

private:
	enum class Flag
	{
		NoSerialize,
		ReadOnly,
		Optional,
		NoCopy,

		Count
	};

	Gaff::Flags<Flag> _flags;
};

NS_END
