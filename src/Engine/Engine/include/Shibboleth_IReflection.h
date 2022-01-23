/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

	virtual const char* getName(void) const = 0;
	virtual Gaff::Hash64 getHash(void) const = 0;
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

	Reflection(void)
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}

	void init(void) override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
	}

	bool isEnum(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return false;
	}

	const char* getName(void) const override
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		return "Unknown";
	}

	Gaff::Hash64 getHash(void) const override
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

	static const IReflectionDefinition& GetReflectionDefinition(void)
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		const IReflectionDefinition* const ref_def = nullptr;
		return *ref_def;
	}

	static const IReflection& GetInstance(void)
	{
		GAFF_ASSERT_MSG(false, "Unknown object type.");
		const IReflection* const instance = nullptr;
		return *instance;
	}
};


class IReflectionObject
{
public:
	virtual ~IReflectionObject(void) {}

	virtual const IReflectionDefinition& getReflectionDefinition(void) const = 0;

	virtual const void* getBasePointer(void) const = 0;
	virtual void* getBasePointer(void) = 0;
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

	virtual void setFlagValue(void*, int32_t, bool)
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not flags!");
	}

	virtual bool getFlagValue(void*, int32_t) const
	{
		GAFF_ASSERT_MSG(false, "Reflection variable is not flags!");
		return false;
	}


	void setReadOnly(bool read_only) { _flags.set(read_only, Flag::ReadOnly); }
	bool isReadOnly(void) const { return _flags.testAll(Flag::ReadOnly); }

	void setNoSerialize(bool no_serialize) { _flags.set(no_serialize, Flag::NoSerialize); }
	bool canSerialize(void) const { return !_flags.testAll(Flag::NoSerialize); }

private:
	enum class Flag
	{
		NoSerialize,
		ReadOnly,

		Count
	};

	Gaff::Flags<Flag> _flags;
};

class IAttribute : public IReflectionObject
{
public:
	virtual IAttribute* clone(void) const = 0;

	virtual Gaff::Hash64 applyVersioning(Gaff::Hash64 hash) const { return hash; }
	virtual bool canInherit(void) const { return true; }

	virtual void finish(IEnumReflectionDefinition& /*ref_def*/) {}
	virtual void finish(IReflectionDefinition& /*ref_def*/) {}

	virtual void instantiated(void* /*object*/, const IReflectionDefinition& /*ref_def*/) {}

	// The apply function corresponds directly to calls in reflection definition. Apply all that apply.

	// Attributes that are applied to functions need to implement these template functions.
	template <class T, class Ret, class... Args>
	void apply(Ret (T::* /*func*/)(Args...) const) {}
	template <class T, class Ret, class... Args>
	void apply(Ret (T::* /*func*/)(Args...)) {}

	// Attributes that are applied to static class functions need to implement this template function.
	template <class T, class Ret, class... Args>
	void apply(Ret (* /*func*/)(Args...)) {}

	// Attributes that are applied to variables need to implement these template functions,
	// or at least the ones they apply to.
	template <class T, class Var>
	void apply(IReflectionVar& /*ref_var*/, Var T::* /*var*/) {}
	template <class T, class Var, class Ret>
	void apply(IReflectionVar& /*ref_var*/, Ret (T::* /*getter*/)(void) const, void (T::* /*setter*/)(Var)) {}
	template <class T, class Var, class Vec_Allocator, size_t size>
	void apply(IReflectionVar& /*ref_var*/, Shibboleth::Vector<Var> T::* /*vec*/) {}
	template <class T, class Var, size_t size>
	void apply(IReflectionVar& /*ref_var*/, Var (T::* /*arr*/)[size]) {}
};

NS_END
