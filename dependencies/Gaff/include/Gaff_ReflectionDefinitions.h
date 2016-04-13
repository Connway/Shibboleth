/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Gaff_IReflectionDefinitions.h"
#include "Gaff_HashString.h"
#include "Gaff_HashMap.h"
#include "Gaff_String.h"
#include "Gaff_SharedPtr.h"
#include "Gaff_Function.h"
#include "Gaff_JSON.h"
#include "Gaff_Math.h"
#include "Gaff_Map.h"
#include <cstdlib>

#define VAR_CONTAINER(name, type) \
	class name : public ValueContainerBase \
	{ \
	public: \
		typedef type VarType; \
		typedef type (T::*Getter)(void) const; \
		typedef void (T::*Setter)(type); \
		name(const char* key, Getter getter, Setter setter, const Allocator& allocator); \
		name(const char* key, type T::* var, const Allocator& allocator); \
		void read(const JSON& json, T* object); \
		void write(JSON& json, const T* object) const; \
		ReflectionValueType getType(void) const; \
		void get(void* out, const void* object) const; \
		void get(void* out, const T* object) const; \
		void set(const void* value, void* object); \
		void set(const void* value, T* object); \
		bool isFixedArray(void) const { return false; } \
		bool isArray(void) const { return false; } \
	private: \
		type T::* _var; \
		Getter _getter; \
		Setter _setter; \
	}

#define VAR_CONTAINER_REF(name, type) \
	class name : public ValueContainerBase \
	{ \
	public: \
		typedef type VarType; \
		typedef const type& (T::*Getter)(void) const; \
		typedef void (T::*Setter)(const type&); \
		name(const char* key, Getter getter, Setter setter, const Allocator& allocator); \
		name(const char* key, type T::* var, const Allocator& allocator); \
		void read(const JSON& json, T* object); \
		void write(JSON& json, const T* object) const; \
		ReflectionValueType getType(void) const; \
		void get(void* out, const void* object) const; \
		void get(void* out, const T* object) const; \
		void set(const void* value, void* object); \
		void set(const void* value, T* object); \
		bool isFixedArray(void) const { return false; } \
		bool isArray(void) const { return false; } \
	private: \
		type T::* _var; \
		Getter _getter; \
		Setter _setter; \
	}

#define VAR_CONTAINER_CONSTRUCTOR(name) \
	template <class T, class Allocator> \
	ReflectionDefinition<T, Allocator>::name::name(const char* key, Getter getter, Setter setter, const Allocator& allocator): ValueContainerBase(key, allocator), _var(nullptr), _getter(getter), _setter(setter) {} \
	template <class T, class Allocator> \
	ReflectionDefinition<T, Allocator>::name::name(const char* key, VarType T::* var, const Allocator& allocator): ValueContainerBase(key, allocator), _var(var), _getter(nullptr), _setter(nullptr) {}

#define VAR_CONTAINER_READ(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::read(const JSON& json, T* object)

#define VAR_CONTAINER_WRITE(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::write(JSON& json, const T* object) const

#define VAR_CONTAINER_VAL_TYPE(name, type) \
	template <class T, class Allocator> \
	ReflectionValueType ReflectionDefinition<T, Allocator>::name::getType(void) const \
	{ \
		return type; \
	}

#define VAR_CONTAINER_SET(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::set(const void* value, void* object) \
	{ \
		set(value, reinterpret_cast<T*>(object)); \
	} \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::set(const void* value, T* object) \
	{ \
		if (_var) { \
			object->*_var = *reinterpret_cast<const VarType*>(value); \
		} else if (_setter) { \
			(object->*_setter)(*reinterpret_cast<const VarType*>(value)); \
		} \
	}

#define VAR_CONTAINER_GET(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::get(void* out, const void* object) const \
	{ \
		get(out, reinterpret_cast<const T*>(object)); \
	} \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::get(void* out, const T* object) const \
	{ \
		if (_var) { \
			*reinterpret_cast<VarType*>(out) = (object->*_var); \
		} else if (_getter) { \
			*reinterpret_cast<VarType*>(out) = (object->*_getter)(); \
		} \
	}


#define ARRAY_CONTAINER(name, type) \
	class name : public ValueContainerBase \
	{ \
	public: \
		typedef Array<type, Allocator> VarType; \
		typedef type ElementType; \
		typedef const Array<type, Allocator>& (T::*Getter)(void) const; \
		typedef void (T::*Setter)(const Array<type, Allocator>&); \
		name(const char* key, Getter getter, Setter setter, const Allocator& allocator); \
		name(const char* key, Array<type, Allocator> T::* var, const Allocator& allocator); \
		void read(const JSON& json, T* object); \
		void write(JSON& json, const T* object) const; \
		ReflectionValueType getType(void) const; \
		void get(void* out, size_t index, const void* object) const; \
		void get(void* out, size_t index, const T* object) const; \
		void get(void* out, const void* object) const; \
		void get(void* out, const T* object) const; \
		void set(const void* value, size_t index, void* object); \
		void set(const void* value, size_t index, T* object); \
		void set(const void* value, void* object); \
		void set(const void* value, T* object); \
		size_t size(const void* object) const; \
		size_t size(const T* object) const; \
		void resize(size_t new_size, void* object); \
		void resize(size_t new_size, T* object); \
		void move(size_t src_index, size_t dest_index, void* object); \
		void move(size_t src_index, size_t dest_index, T* object); \
		bool isFixedArray(void) const { return false; } \
		bool isArray(void) const { return true; } \
	private: \
		Array<type, Allocator> T::* _var; \
		Getter _getter; \
		Setter _setter; \
		Allocator _allocator; \
	}

#define ARRAY_CONTAINER_FIXED(name, type) \
	template <size_t array_size> \
	class name : public ValueContainerBase \
	{ \
	public: \
		typedef type VarType; \
		typedef const type* (T::*Getter)(void) const; \
		typedef void (T::*Setter)(const type*); \
		name(const char* key, Getter getter, Setter setter, const Allocator& allocator); \
		name(const char* key, type (T::*var)[array_size], const Allocator& allocator); \
		void read(const JSON& json, T* object); \
		void write(JSON& json, const T* object) const; \
		ReflectionValueType getType(void) const; \
		void get(void* out, size_t index, const void* object) const; \
		void get(void* out, size_t index, const T* object) const; \
		void get(void* out, const void* object) const; \
		void get(void* out, const T* object) const; \
		void set(const void* value, size_t index, void* object); \
		void set(const void* value, size_t index, T* object); \
		void set(const void* value, void* object); \
		void set(const void* value, T* object); \
		size_t size(const void*) const; \
		void move(size_t src_index, size_t dest_index, void* object); \
		void move(size_t src_index, size_t dest_index, T* object); \
		bool isFixedArray(void) const { return true; } \
		bool isArray(void) const { return true; } \
	private: \
		type (T::*_var)[array_size]; \
		Getter _getter; \
		Setter _setter; \
	}

#define ARRAY_CONTAINER_CONSTRUCTOR(name) \
	template <class T, class Allocator> \
	ReflectionDefinition<T, Allocator>::name::name(const char* key, Getter getter, Setter setter, const Allocator& allocator): ValueContainerBase(key, allocator), _var(nullptr), _getter(getter), _setter(setter), _allocator(allocator) {} \
	template <class T, class Allocator> \
	ReflectionDefinition<T, Allocator>::name::name(const char* key, VarType T::* var, const Allocator& allocator): ValueContainerBase(key, allocator), _var(var), _getter(nullptr), _setter(nullptr), _allocator(allocator) {}

#define ARRAY_CONTAINER_READ VAR_CONTAINER_READ
#define ARRAY_CONTAINER_WRITE VAR_CONTAINER_WRITE
#define ARRAY_CONTAINER_VAL_TYPE VAR_CONTAINER_VAL_TYPE
#define ARRAY_CONTAINER_SET VAR_CONTAINER_SET
#define ARRAY_CONTAINER_GET	VAR_CONTAINER_GET

#define ARRAY_CONTAINER_SET_INDEX(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::set(const void* value, size_t index, void* object) \
	{ \
		set(value, index, reinterpret_cast<T*>(object)); \
	} \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::set(const void* value, size_t index, T* object) \
	{ \
		if (_var) { \
			GAFF_ASSERT(index < (object->*_var).size()); \
			(object->*_var)[index] = *reinterpret_cast<const type*>(value); \
		} else if (_getter) { \
			VarType& array = const_cast<VarType&>((object->*_getter)()); \
			GAFF_ASSERT(index < array.size()); \
			array[index] = *reinterpret_cast<const type*>(value); \
		} \
	}

#define ARRAY_CONTAINER_GET_INDEX(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::get(void* out, size_t index, const void* object) const \
	{ \
		get(out, index, reinterpret_cast<const T*>(object)); \
	} \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::get(void* out, size_t index, const T* object) const \
	{ \
		if (_var) { \
			GAFF_ASSERT(index < (object->*_var).size()); \
			*reinterpret_cast<type*>(out) = (object->*_var)[index]; \
		} else if (_getter) { \
			const VarType& array = (object->*_getter)(); \
			GAFF_ASSERT(index < array.size()); \
			*reinterpret_cast<type*>(out) = array[index]; \
		} \
	}

#define ARRAY_CONTAINER_SIZE(name) \
	template <class T, class Allocator> \
	size_t ReflectionDefinition<T, Allocator>::name::size(const void* object) const \
	{ \
		return size(reinterpret_cast<const T*>(object)); \
	} \
	template <class T, class Allocator> \
	size_t ReflectionDefinition<T, Allocator>::name::size(const T* object) const \
	{ \
		if (_var) { \
			return (object->*_var).size(); \
		} else if (_getter) { \
			return (object->*_getter)().size(); \
		} else { \
			return 0; \
		} \
	}

#define ARRAY_CONTAINER_RESIZE(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::resize(size_t new_size, void* object) \
	{ \
		resize(new_size, reinterpret_cast<T*>(object)); \
	} \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::resize(size_t new_size, T* object) \
	{ \
		if (_var) { \
			(object->*_var).resize(new_size); \
		} else if (_getter) { \
			const_cast<VarType&>((object->*_getter)()).resize(new_size); \
		} \
	}

#define ARRAY_CONTAINER_MOVE(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::move(size_t src_index, size_t dest_index, void* object) \
	{ \
		move(src_index, dest_index, reinterpret_cast<T*>(object)); \
	} \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::move(size_t src_index, size_t dest_index, T* object) \
	{ \
		if (_var) { \
			GAFF_ASSERT(src_index < (object->*_var).size() && dest_index <= (object->*_var).size()); \
			ElementType temp = std::move((object->*_var)[src_index]); \
			(object->*_var).erase(src_index); \
			dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; /* If the dest index was above the source index, shift it down by one.*/ \
			(object->*_var).emplace(dest_index, std::move(temp)); \
		} else if (_getter) { \
			VarType& array = const_cast<VarType&>((object->*_getter)()); \
			GAFF_ASSERT(src_index < array.size() && dest_index <= array.size()); \
			ElementType temp = std::move(array[src_index]); \
			array.erase(src_index); \
			dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; /* If the dest index was above the source index, shift it down by one.*/ \
			array.emplace(dest_index, std::move(temp)); \
		} \
	}


#define ARRAY_CONTAINER_FIXED_CONSTRUCTOR(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	ReflectionDefinition<T, Allocator>::name<array_size>::name(const char* key, Getter getter, Setter setter, const Allocator& allocator): ValueContainerBase(key, allocator), _var(nullptr), _getter(getter), _setter(setter) {} \
	template <class T, class Allocator> \
	template <size_t array_size> \
	ReflectionDefinition<T, Allocator>::name<array_size>::name(const char* key, VarType (T::*var)[array_size], const Allocator& allocator): ValueContainerBase(key, allocator), _var(var), _getter(nullptr), _setter(nullptr) {}

#define ARRAY_CONTAINER_FIXED_READ(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::read(const JSON& json, T* object)

#define ARRAY_CONTAINER_FIXED_WRITE(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::write(JSON& json, const T* object) const

#define ARRAY_CONTAINER_FIXED_VAL_TYPE(name, type) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	ReflectionValueType ReflectionDefinition<T, Allocator>::name<array_size>::getType(void) const \
	{ \
		return type; \
	}

#define ARRAY_CONTAINER_FIXED_SET(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::set(const void* value, void* object) \
	{ \
		set(value, reinterpret_cast<T*>(object)); \
	}\
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::set(const void* value, T* object) \
	{ \
		const VarType* values = reinterpret_cast<const VarType*>(value); \
		VarType* array = nullptr; \
		if (_var) { \
			array = (object->*_var); \
		} else if (_getter) { \
			array = const_cast<VarType*>((object->*_getter)()); \
		} \
		if (array) { \
			for (size_t i = 0; i < array_size; ++i) { \
				array[i] = values[i]; \
			} \
		} \
	}

#define ARRAY_CONTAINER_FIXED_GET(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::get(void* out, const void* object) const \
	{ \
		get(out, reinterpret_cast<const T*>(object)); \
	} \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::get(void* out, const T* object) const \
	{ \
		VarType* values = reinterpret_cast<VarType*>(out); \
		const VarType* array = nullptr; \
		if (_var) { \
			array = (object->*_var); \
		} else if (_getter) { \
			array = (object->*_getter)(); \
		} \
		if (array) { \
			for (size_t i = 0; i < array_size; ++i) { \
				values[i] = array[i]; \
			} \
		} \
	}

#define ARRAY_CONTAINER_FIXED_GET_INDEX(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::get(void* out, size_t index, const void* object) const \
	{ \
		get(out, index, reinterpret_cast<const T*>(object)); \
	} \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::get(void* out, size_t index, const T* object) const \
	{ \
		GAFF_ASSERT(index < array_size); \
		const VarType* array = nullptr; \
		if (_var) { \
			array = (object->*_var); \
		} else if (_getter) { \
			array = (object->*_getter)(); \
		} \
		if (array) { \
			*reinterpret_cast<VarType*>(out) = array[index]; \
		} \
	}

#define ARRAY_CONTAINER_FIXED_SET_INDEX(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::set(const void* value, size_t index, void* object) \
	{ \
		set(value, index, reinterpret_cast<T*>(object)); \
	} \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::set(const void* value, size_t index, T* object) \
	{ \
		GAFF_ASSERT(index < array_size); \
		VarType* array = nullptr; \
		if (_var) { \
			array = (object->*_var); \
		} else if (_getter) { \
			array = const_cast<VarType*>((object->*_getter)()); \
		} \
		if (array) { \
			array[index] = *reinterpret_cast<const VarType*>(value); \
		} \
	}

#define ARRAY_CONTAINER_FIXED_SIZE(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	size_t ReflectionDefinition<T, Allocator>::name<array_size>::size(const void*) const \
	{ \
		return array_size; \
	}

#define ARRAY_CONTAINER_FIXED_MOVE(name) \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::move(size_t src_index, size_t dest_index, void* object) \
	{ \
		move(src_index, dest_index, reinterpret_cast<T*>(object)); \
	} \
	template <class T, class Allocator> \
	template <size_t array_size> \
	void ReflectionDefinition<T, Allocator>::name<array_size>::move(size_t src_index, size_t dest_index, T* object) \
	{ \
		GAFF_ASSERT(src_index < array_size && dest_index <= array_size); \
		VarType* array = nullptr; \
		if (_var) { \
			array = (object->*_var); \
		} else if (_getter) { \
			array = const_cast<VarType*>((object->*_getter)()); \
		} \
		if (array) { \
			VarType temp = std::move((object->*_var)[src_index]); \
			deconstruct(array + src_index); \
			for (size_t i = src_index; i  < array_size - 1; ++i) { \
				memcpy(array + i, array + i + 1, sizeof(VarType)); \
			} \
			dest_index = (dest_index > src_index) ? dest_index - 1 : dest_index; /* If the dest index was above the source index, shift it down by one.*/ \
			for (size_t i = array_size - 1; i > dest_index; --i) { \
				memcpy(array + i, array + i - 1, sizeof(VarType)); \
			} \
			construct(array + dest_index, std::move(temp)); \
		} \
	}


NS_GAFF

class IOnCompleteFunctor
{
public:
	IOnCompleteFunctor(void) {}
	virtual ~IOnCompleteFunctor(void) {}

	virtual void setRefDef(IReflectionDefinition* ref_def) = 0;
};


template <class Allocator = DefaultAllocator>
class BaseCallbackHelper
{
public:
	static BaseCallbackHelper<Allocator>& GetInstance(const Allocator& allocator = Allocator());

	// Takes in a FunctionBinder<> pointer so we can update the reflection definition later.
	void addBaseClassCallback(IReflectionDefinition* base_ref_def, FunctionBinder<void>* callback);
	void triggerBaseClassCallback(IReflectionDefinition* base_ref_def);

private:
	BaseCallbackHelper(const Allocator& allocator);
	~BaseCallbackHelper(void);

	Map< IReflectionDefinition*, Array<FunctionBinder<void>*, Allocator>, Allocator > _callbacks;
	Allocator _allocator;
};

template <class T, class Allocator = DefaultAllocator>
class EnumReflectionDefinition : public IEnumReflectionDefinition<Allocator>
{
public:
	EnumReflectionDefinition(EnumReflectionDefinition<T, Allocator>&& ref_def);

	EnumReflectionDefinition(const char* name, const Allocator& allocator = Allocator());
	EnumReflectionDefinition(const Allocator& allocator = Allocator());
	~EnumReflectionDefinition(void);

	const EnumReflectionDefinition<T, Allocator>& operator=(EnumReflectionDefinition<T, Allocator>&& rhs);

	EnumReflectionDefinition<T, Allocator>&& addValue(const char* name, T value);
	const char* getName(T value) const;
	T getValue(const char* name) const;
	bool getValue(const char* name, T& value) const;

	Pair<AString<Allocator>, T> getEntry(size_t index) const;

	const char* getNameGeneric(unsigned int value) const;
	unsigned int getValueGeneric(const char* name) const;
	bool getValueGeneric(const char* name, unsigned int& value) const;

	Pair<AString<Allocator>, unsigned int> getEntryGeneric(unsigned int index) const;
	size_t getNumEntries(void) const;

	const char* getEnumName(void) const;

	EnumReflectionDefinition<T, Allocator>&& setAllocator(const Allocator& allocator);
	void clear(void);

	bool isDefined(void) const;
	void markDefined(void);

	EnumReflectionDefinition<T, Allocator>&& macroFix(void);

private:
	HashMap<AHashString<Allocator>, T, Allocator> _values_map;
	AString<Allocator> _name;
	bool _defined;

	GAFF_NO_COPY(EnumReflectionDefinition);
};

template <class T, class Allocator>
class ReflectionDefinition : public IReflectionDefinition
{
public:
	class ValueContainerBase : public IValueContainer
	{
	public:
		ValueContainerBase(const char* key, const Allocator& allocator) : _key(key, allocator) {}
		virtual ~ValueContainerBase(void) {}

		virtual void read(const JSON& json, T* object) = 0;
		virtual void write(JSON& json, const T* object) const = 0;

	protected:
		AString<Allocator> _key;
	};

	ReflectionDefinition(ReflectionDefinition<T, Allocator>&& ref_def);
	ReflectionDefinition(const char* name, const Allocator& allocator = Allocator());
	ReflectionDefinition(const Allocator& allocator = Allocator());
	~ReflectionDefinition(void);

	const ReflectionDefinition<T, Allocator>& operator=(ReflectionDefinition<T, Allocator>&& rhs);

	void read(const JSON& json, void* object) override;
	void write(JSON& json, const void* object) const override;

	const void* getInterface(ReflectionHash class_id, const void* object) const override;
	void* getInterface(ReflectionHash class_id, const void* object) override;

	const char* getName(void) const override;

	void read(const JSON& json, T* object);
	void write(JSON& json, const T* object) const;

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addBaseClass(ReflectionDefinition<T2, Allocator>& base_ref_def, ReflectionHash class_id);

	// This function does not check if the base class is finished being defined. This is so you can add just casts.
	template <class T2>
	ReflectionDefinition<T, Allocator>&& addBaseClass(ReflectionHash class_id);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addBaseClass(void);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addBaseClassInterfaceOnly(void);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addObject(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addEnum(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& ref_def);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<T2, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<T2, Allocator>&), ReflectionDefinition<T2, Allocator>& obj_ref_def);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<T2, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<T2, Allocator>&));

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<T2, Allocator> T::* var, ReflectionDefinition<T2, Allocator>& obj_ref_def);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<T2, Allocator> T::* var);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<T2, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<T2, Allocator>&), const EnumReflectionDefinition<T2, Allocator>& enum_ref_def);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<T2, Allocator> T::* var, const EnumReflectionDefinition<T2, Allocator>& enum_ref_def);

	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<double, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<double, Allocator>&));
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<float, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<float, Allocator>&));
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<unsigned int, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<unsigned int, Allocator>&));
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<int, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<int, Allocator>&));
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<unsigned short, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<unsigned short, Allocator>&));
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<short, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<short, Allocator>&));
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<unsigned char, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<unsigned char, Allocator>&));
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<char, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<char, Allocator>&));
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const Array<AString<Allocator>, Allocator>& (T::*getter)(void) const, void (T::*setter)(const Array<AString<Allocator>, Allocator>&));

	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<double, Allocator> T::* var);
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<float, Allocator> T::* var);
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<unsigned int, Allocator> T::* var);
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<int, Allocator> T::* var);
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<unsigned short, Allocator> T::* var);
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<short, Allocator> T::* var);
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<unsigned char, Allocator> T::* var);
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<char, Allocator> T::* var);
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<AString<Allocator>, Allocator> T::* var);

	template <unsigned int array_size, class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const T2* (T::*getter)(void) const, void (T::*setter)(const T2*), const EnumReflectionDefinition<T2, Allocator>& enum_ref_def);

	template <unsigned int array_size, class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const T2* (T::*getter)(void) const, void (T::*setter)(const T2*), ReflectionDefinition<T2, Allocator>& obj_ref_def);

	template <unsigned int array_size, class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const T2* (T::*getter)(void) const, void (T::*setter)(const T2*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const double* (T::*getter)(void) const, void (T::*setter)(const double*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const float* (T::*getter)(void) const, void (T::*setter)(const float*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const unsigned int* (T::*getter)(void) const, void (T::*setter)(const unsigned int*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const int* (T::*getter)(void) const, void (T::*setter)(const int*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const unsigned short* (T::*getter)(void) const, void (T::*setter)(const unsigned short*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const short* (T::*getter)(void) const, void (T::*setter)(const short*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const unsigned char* (T::*getter)(void) const, void (T::*setter)(const unsigned char*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const char* (T::*getter)(void) const, void (T::*setter)(const char*));

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, const AString<Allocator>* (T::*getter)(void) const, void (T::*setter)(const AString<Allocator>*));

	template <unsigned int array_size, class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, T2 (T::*var)[array_size], const EnumReflectionDefinition<T2, Allocator>& enum_ref_def);

	template <unsigned int array_size, class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, T2 (T::*var)[array_size], ReflectionDefinition<T2, Allocator>& obj_ref_def);

	template <unsigned int array_size, class T2>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, T2 (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, double (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, float (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, unsigned int (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, int (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, unsigned short (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, short (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, unsigned char (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, char (T::*var)[array_size]);

	template <unsigned int array_size>
	ReflectionDefinition<T, Allocator>&& addArray(const char* key, AString<Allocator> (T::*var)[array_size]);

	ReflectionDefinition<T, Allocator>&& addDouble(const char* key, double T::* var);
	ReflectionDefinition<T, Allocator>&& addFloat(const char* key, float T::* var);
	ReflectionDefinition<T, Allocator>&& addUInt(const char* key, unsigned int T::* var);
	ReflectionDefinition<T, Allocator>&& addInt(const char* key, int T::* var);
	ReflectionDefinition<T, Allocator>&& addUShort(const char* key, unsigned short T::* var);
	ReflectionDefinition<T, Allocator>&& addShort(const char* key, short T::* var);
	ReflectionDefinition<T, Allocator>&& addUChar(const char* key, unsigned char T::* var);
	ReflectionDefinition<T, Allocator>&& addChar(const char* key, char T::* var);
	ReflectionDefinition<T, Allocator>&& addBool(const char* key, bool T::* var);
	ReflectionDefinition<T, Allocator>&& addString(const char* key, AString<Allocator> T::* var);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addEnum(const char* key, const EnumReflectionDefinition<T2, Allocator>& ref_def, T2 (T::*getter)(void) const, void (T::*setter)(T2 value));

	ReflectionDefinition<T, Allocator>&& addDouble(const char* key, double (T::*getter)(void) const, void (T::*Setter)(double value));
	ReflectionDefinition<T, Allocator>&& addFloat(const char* key, float (T::*getter)(void) const, void (T::*setter)(float value));
	ReflectionDefinition<T, Allocator>&& addUInt(const char* key, unsigned int (T::*getter)(void) const, void (T::*setter)(unsigned int value));
	ReflectionDefinition<T, Allocator>&& addInt(const char* key, int (T::*getter)(void) const, void (T::*setter)(int value));
	ReflectionDefinition<T, Allocator>&& addUShort(const char* key, unsigned short (T::*getter)(void) const, void (T::*setter)(unsigned short value));
	ReflectionDefinition<T, Allocator>&& addShort(const char* key, short (T::*getter)(void) const, void (T::*setter)(short value));
	ReflectionDefinition<T, Allocator>&& addUChar(const char* key, unsigned char (T::*getter)(void) const, void (T::*setter)(unsigned char value));
	ReflectionDefinition<T, Allocator>&& addChar(const char* key, char (T::*getter)(void) const, void (T::*setter)(char value));
	ReflectionDefinition<T, Allocator>&& addBool(const char* key, bool (T::*getter)(void) const, void (T::*setter)(bool value));
	ReflectionDefinition<T, Allocator>&& addString(const char* key, const AString<Allocator>& (T::*getter)(void) const, void (T::*setter)(const AString<Allocator>& value));

	ReflectionDefinition<T, Allocator>&& addCustom(const char* key, ValueContainerBase* container);

	bool isDefined(void) const;
	void markDefined(void);

	ReflectionDefinition<T, Allocator>&& setAllocator(const Allocator& allocator);
	void clear(void);

	ReflectionDefinition<T, Allocator>&& macroFix(void);

private:
	typedef SharedPtr<ValueContainerBase, Allocator> ValueContainerPtr;

	VAR_CONTAINER(DoubleContainer, double);
	VAR_CONTAINER(FloatContainer, float);
	VAR_CONTAINER(UIntContainer, unsigned int);
	VAR_CONTAINER(IntContainer, int);
	VAR_CONTAINER(UShortContainer, unsigned short);
	VAR_CONTAINER(ShortContainer, short);
	VAR_CONTAINER(UCharContainer, unsigned char);
	VAR_CONTAINER(CharContainer, char);
	VAR_CONTAINER(BoolContainer, bool);
	VAR_CONTAINER_REF(StringContainer, AString<Allocator>);

	ARRAY_CONTAINER(ArrayDoubleContainer, double);
	ARRAY_CONTAINER(ArrayFloatContainer, float);
	ARRAY_CONTAINER(ArrayUIntContainer, unsigned int);
	ARRAY_CONTAINER(ArrayIntContainer, int);
	ARRAY_CONTAINER(ArrayUShortContainer, unsigned short);
	ARRAY_CONTAINER(ArrayShortContainer, short);
	ARRAY_CONTAINER(ArrayUCharContainer, unsigned char);
	ARRAY_CONTAINER(ArrayCharContainer, char);
	//ARRAY_CONTAINER(ArrayBoolContainer, bool); // Maybe not support this and support BitArray instead?
	ARRAY_CONTAINER(ArrayStringContainer, AString<Allocator>);

	ARRAY_CONTAINER_FIXED(ArrayFixedDoubleContainer, double);
	ARRAY_CONTAINER_FIXED(ArrayFixedFloatContainer, float);
	ARRAY_CONTAINER_FIXED(ArrayFixedUIntContainer, unsigned int);
	ARRAY_CONTAINER_FIXED(ArrayFixedIntContainer, int);
	ARRAY_CONTAINER_FIXED(ArrayFixedUShortContainer, unsigned short);
	ARRAY_CONTAINER_FIXED(ArrayFixedShortContainer, short);
	ARRAY_CONTAINER_FIXED(ArrayFixedUCharContainer, unsigned char);
	ARRAY_CONTAINER_FIXED(ArrayFixedCharContainer, char);
	ARRAY_CONTAINER_FIXED(ArrayFixedStringContainer, AString<Allocator>);

	template <size_t array_size, class T2>
	class ArrayFixedEnumContainer : public ValueContainerBase
	{
	public:
		typedef const T2* (T::*Getter)(void) const;
		typedef void (T::*Setter)(const T2* value);

		ArrayFixedEnumContainer(const char* key, const EnumReflectionDefinition<T2, Allocator>& enum_ref_def, Getter getter, Setter setter, const Allocator& allocator);
		ArrayFixedEnumContainer(const char* key, T2 (T::*var)[array_size], const EnumReflectionDefinition<T2, Allocator>& enum_ref_def, const Allocator& allocator);

		void read(const JSON& json, T* object);
		void write(JSON& json, const T* object) const;

		void get(void* out, const void* object) const;
		void get(void* out, const T* object) const;

		void get(void* out, size_t index, const void* object) const;
		void get(void* out, size_t index, const T* object) const;

		void set(const void* value, void* object);
		void set(const void* value, T* object);

		void set(const void* value, size_t index, void* object);
		void set(const void* value, size_t index, T* object);

		size_t size(const void*) const;

		void move(size_t src_index, size_t dest_index, void* object);
		void move(size_t src_index, size_t dest_index, T* object);

		bool isFixedArray(void) const;
		bool isArray(void) const;

		ReflectionValueType getType(void) const;

	private:
		const EnumReflectionDefinition<T2, Allocator>& _enum_ref_def;
		Getter _getter;
		Setter _setter;
		T2 (T::*_var)[array_size];
	};

	template <size_t array_size, class T2>
	class ArrayFixedObjectContainer : public ValueContainerBase
	{
	public:
		typedef const T2* (T::*Getter)(void) const;
		typedef void (T::*Setter)(const T2* value);

		ArrayFixedObjectContainer(const char* key, ReflectionDefinition<T2, Allocator>& obj_ref_def, Getter getter, Setter setter, const Allocator& allocator);
		ArrayFixedObjectContainer(const char* key, T2 (T::*var)[array_size], ReflectionDefinition<T2, Allocator>& obj_ref_def, const Allocator& allocator);

		void read(const JSON& json, T* object);
		void write(JSON& json, const T* object) const;

		void get(void* out, const void* object) const;
		void get(void* out, const T* object) const;

		void get(void* out, size_t index, const void* object) const;
		void get(void* out, size_t index, const T* object) const;

		void set(const void* value, void* object);
		void set(const void* value, T* object);

		void set(const void* value, size_t index, void* object);
		void set(const void* value, size_t index, T* object);

		size_t size(const void*) const;

		void move(size_t src_index, size_t dest_index, void* object);
		void move(size_t src_index, size_t dest_index, T* object);

		bool isFixedArray(void) const;
		bool isArray(void) const;

		ReflectionValueType getType(void) const;

	private:
		ReflectionDefinition<T2, Allocator>& _obj_ref_def;
		Getter _getter;
		Setter _setter;
		T2 (T::*_var)[array_size];
	};

	template <class T2>
	class ArrayObjectContainer : public ValueContainerBase
	{
	public:
		typedef const Array<T2, Allocator>& (T::*Getter)(void) const;
		typedef void (T::*Setter)(const Array<T2, Allocator>& value);

		ArrayObjectContainer(const char* key, ReflectionDefinition<T2, Allocator>& obj_ref_def, Getter getter, Setter setter, const Allocator& allocator);
		ArrayObjectContainer(const char* key, Array<T2, Allocator> T::* var, ReflectionDefinition<T2, Allocator>& obj_ref_def, const Allocator& allocator);

		void read(const JSON& json, T* object);
		void write(JSON& json, const T* object) const;

		void get(void* out, const void* object) const;
		void get(void* out, const T* object) const;

		void get(void* out, size_t index, const void* object) const;
		void get(void* out, size_t index, const T* object) const;

		void set(const void* value, void* object);
		void set(const void* value, T* object);

		void set(const void* value, size_t index, void* object);
		void set(const void* value, size_t index, T* object);

		size_t size(const void* object) const;
		size_t size(const T* object) const;

		void resize(size_t new_size, void* object);
		void resize(size_t new_size, T* object);

		void move(size_t src_index, size_t dest_index, void* object);
		void move(size_t src_index, size_t dest_index, T* object);

		bool isFixedArray(void) const;
		bool isArray(void) const;

		ReflectionValueType getType(void) const;

	private:
		ReflectionDefinition<T2, Allocator>& _obj_ref_def;
		Getter _getter;
		Setter _setter;
		Array<T2, Allocator> T::* _var;
	};

	template <class T2>
	class ArrayEnumContainer : public ValueContainerBase
	{
	public:
		typedef const Array<T2, Allocator>& (T::*Getter)(void) const;
		typedef void (T::*Setter)(const Array<T2, Allocator>& value);

		ArrayEnumContainer(const char* key, const EnumReflectionDefinition<T2, Allocator>& enum_ref_def, Getter getter, Setter setter, const Allocator& allocator);
		ArrayEnumContainer(const char* key, Array<T2, Allocator> T::* var, const EnumReflectionDefinition<T2, Allocator>& enum_ref_def, const Allocator& allocator);

		void read(const JSON& json, T* object);
		void write(JSON& json, const T* object) const;

		void get(void* out, const void* object) const;
		void get(void* out, const T* object) const;

		void get(void* out, size_t index, const void* object) const;
		void get(void* out, size_t index, const T* object) const;

		void set(const void* value, void* object);
		void set(const void* value, T* object);

		void set(const void* value, size_t index, void* object);
		void set(const void* value, size_t index, T* object);

		size_t size(const void* object) const;
		size_t size(const T* object) const;

		void resize(size_t new_size, void* object);
		void resize(size_t new_size, T* object);

		void move(size_t src_index, size_t dest_index, void* object);
		void move(size_t src_index, size_t dest_index, T* object);

		bool isFixedArray(void) const;
		bool isArray(void) const;

		ReflectionValueType getType(void) const;

	private:
		const EnumReflectionDefinition<T2, Allocator>& _enum_ref_def;
		Getter _getter;
		Setter _setter;
		Array<T2, Allocator> T::* _var;
	};

	template <class T2>
	class ObjectContainer : public ValueContainerBase
	{
	public:
		typedef const T2& (T::*Getter)(void) const;
		typedef void (T::*Setter)(const T2& value);

		ObjectContainer(const char* key, ReflectionDefinition<T2, Allocator>& var_ref_def, Getter getter, Setter setter, const Allocator& allocator);
		ObjectContainer(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def, const Allocator& allocator);

		void read(const JSON& json, T* object);
		void write(JSON& json, const T* object) const;

		void get(void* out, const void* object) const;
		void get(void* out, const T* object) const;

		void set(const void* value, void* object);
		void set(const void* value, T* object);

		bool isFixedArray(void) const;
		bool isArray(void) const;

		ReflectionValueType getType(void) const;

	private:
		Getter _getter;
		Setter _setter;
		ReflectionDefinition<T2, Allocator>& _var_ref_def;
		T2 T::* _var;
	};

	template <class T2>
	class EnumContainer : public ValueContainerBase
	{
	public:
		typedef T2 (T::*Getter)(void) const;
		typedef void (T::*Setter)(T2 value);

		EnumContainer(const char* key, const EnumReflectionDefinition<T2, Allocator>& ref_def, Getter getter, Setter setter, const Allocator& allocator);
		EnumContainer(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& var_ref_def, const Allocator& allocator);

		void read(const JSON& json, T* object);
		void write(JSON& json, const T* object) const;

		void get(void* out, const void* object) const;
		void get(void* out, const T* object) const;

		void set(const void* value, void* object);
		void set(const void* value, T* object);

		bool isFixedArray(void) const;
		bool isArray(void) const;

		ReflectionValueType getType(void) const;

	private:
		Getter _getter;
		Setter _setter;
		const EnumReflectionDefinition<T2, Allocator>& _var_ref_def;
		T2 T::* _var;

		GAFF_NO_COPY(EnumContainer);
	};

	template <class T2>
	class BaseValueContainer : public ValueContainerBase
	{
	public:
		BaseValueContainer(const char* key, const typename ReflectionDefinition<T2, Allocator>::ValueContainerPtr& value_ptr, const Allocator& allocator);

		void read(const JSON& json, T* object);
		void write(JSON& json, const T* object) const;

		void get(void* out, const void* object) const;
		void get(void* out, const T* object) const;

		void set(const void* value, void* object);
		void set(const void* value, T* object);

		bool isFixedArray(void) const;
		bool isArray(void) const;

		ReflectionValueType getType(void) const;

	private:
		typename ReflectionDefinition<T2, Allocator>::ValueContainerPtr _value_ptr;
	};

	template <class T2>
	class OnCompleteFunctor : public IOnCompleteFunctor
	{
	public:
		OnCompleteFunctor(ReflectionDefinition<T, Allocator>* my_ref_def, bool interface_only);

		void operator()(void) const;
		void setRefDef(IReflectionDefinition* ref_def);

	private:
		ReflectionDefinition<T, Allocator>* _my_ref_def;
		bool _interface_only;
	};

	template <class T2>
	static void* BaseClassCast(const void* object)
	{
		// Holy balls batman. That's a lot of casts.
		return static_cast<T2*>(reinterpret_cast<T*>(const_cast<void*>(object)));
	}

	HashMap<AHashString<Allocator>, ValueContainerPtr, Allocator> _value_containers;
	Array<Pair< ReflectionHash, FunctionBinder<void*, const void*> >, Allocator> _base_ids;
	Array<IOnCompleteFunctor*, Allocator> _callback_references;

	AString<Allocator> _name;
	Allocator _allocator;
	unsigned int _base_classes_remaining;
	bool _defined;

	template <class T2, class Allocator> friend class ReflectionDefinition;
	template <class T2> friend class OnCompleteFunctor;
};

#define REF_DEF(ClassName, Allocator) \
public: \
	static Gaff::ReflectionDefinition<ClassName, Allocator>& GetReflectionDefinition(void); \
	static Gaff::ReflectionHash GetReflectionHash(void); \
private: \
	static Gaff::ReflectionHash gHash; \
	static Gaff::ReflectionDefinition<ClassName, Allocator> gRefDef

#define CLASS_HASH(ClassName) Gaff::FNV1aHash32(#ClassName, static_cast<Gaff::ReflectionHash>((strlen(#ClassName))))

#define REF_IMPL(ClassName, Allocator) \
	Gaff::ReflectionHash ClassName::GetReflectionHash(void) { return gHash; } \
	Gaff::ReflectionDefinition<ClassName, Allocator>& ClassName::GetReflectionDefinition(void) { return gRefDef; } \
	Gaff::ReflectionHash ClassName::gHash = CLASS_HASH(ClassName); \
	Gaff::ReflectionDefinition<ClassName, Allocator> ClassName::gRefDef

// uses default allocator
#define REF_IMPL_ASSIGN_DEFAULT(ClassName, Allocator) \
	Gaff::ReflectionHash ClassName::GetReflectionHash(void) { return gHash; } \
	Gaff::ReflectionDefinition<ClassName, Allocator>& ClassName::GetReflectionDefinition(void) { return gRefDef; } \
	Gaff::ReflectionHash ClassName::gHash = CLASS_HASH(ClassName); \
	Gaff::ReflectionDefinition<ClassName, Allocator> ClassName::gRefDef = Gaff::RefDef<ClassName, Allocator>(#ClassName).macroFix()

#define REF_IMPL_ASSIGN(ClassName, Allocator, allocator_instance) \
	Gaff::ReflectionHash ClassName::GetReflectionHash(void) { return gHash; } \
	Gaff::ReflectionDefinition<ClassName, Allocator>& ClassName::GetReflectionDefinition(void) { return gRefDef; } \
	Gaff::ReflectionHash ClassName::gHash = CLASS_HASH(ClassName); \
	Gaff::ReflectionDefinition<ClassName, Allocator> ClassName::gRefDef = Gaff::RefDef<ClassName, Allocator>(#ClassName, allocator_instance).macroFix()

#define ADD_BASE_CLASS_INTERFACE_ONLY(ClassName) addBaseClass<ClassName>(CLASS_HASH(ClassName))

#define REF_DEF_REQ \
	public: \
		const void* rawRequestInterface(Gaff::ReflectionHash class_id) const override; \
		void* rawRequestInterface(Gaff::ReflectionHash class_id) override

#define REF_IMPL_REQ(ClassName) \
const void* ClassName::rawRequestInterface(Gaff::ReflectionHash class_id) const \
{ \
	return gRefDef.getInterface(class_id, this); \
} \
void* ClassName::rawRequestInterface(Gaff::ReflectionHash class_id) \
{ \
	return gRefDef.getInterface(class_id, this); \
}

#define ENUM_REF_DEF_EMBEDDED(EnumName, Type, Allocator) extern Gaff::EnumReflectionDefinition<Type, Allocator> g##EnumName##RefDef
#define ENUM_REF_IMPL_EMBEDDED(EnumName, Type, Allocator) Gaff::EnumReflectionDefinition<Type, Allocator> g##EnumName##RefDef
#define ENUM_REFL_IMPL_ASSIGN_EMBEDDED(EnumName, Type, Allocator, allocator_instance) ENUM_REF_IMPL_EMBEDDED(EnumName, Type, Allocator) = Gaff::EnumRefDef<Type, Allocator>(#Type, allocator_instance).macroFix()
#define ENUM_REFL_IMPL_ASSIGN_DEFAULT_EMBEDDED(EnumName, Type, Allocator, allocator_instance) ENUM_REF_IMPL_EMBEDDED(EnumName, Type, Allocator) = Gaff::EnumRefDef<Type, Allocator>(#Type).macroFix()

#define ENUM_REF_DEF(EnumName, Allocator) extern Gaff::EnumReflectionDefinition<EnumName, Allocator> g##EnumName##RefDef
#define ENUM_REF_IMPL(EnumName, Allocator) Gaff::EnumReflectionDefinition<EnumName, Allocator> g##EnumName##RefDef
#define ENUM_REF_IMPL_ASSIGN(EnumName, Allocator, allocator_instance) ENUM_REF_IMPL(EnumName, Allocator) = Gaff::EnumRefDef<EnumName, Allocator>(#EnumName, allocator_instance).macroFix()
#define ENUM_REF_IMPL_ASSIGN_DEFAULT(EnumName, Allocator) ENUM_REF_IMPL(EnumName, Allocator) = Gaff::EnumRefDef<EnumName, Allocator>(#EnumName).macroFix()

/*
	This function is useful for when you want to do something like this:

	// In .h file
	class SomeClass
	{
		static ReflectionDefinition<SomeClass> _ref_def;

		int _a;
	};

	// In .cpp file
	ReflectionDefinition<SomeClass> SomeClass::_ref_def = RefDef<SomeClass>()
	.addInt("a", &SomeClass::_a)
	.markDefined()
	;

	BE CAREFUL WITH THIS! If whatever allocator you are using is not initialized or
	available during process/DLL global init, this will crash hard!
*/
template <class T, class Allocator = DefaultAllocator>
ReflectionDefinition<T, Allocator> RefDef(const char* name, const Allocator& allocator = Allocator())
{
	return ReflectionDefinition<T, Allocator>(name, allocator);
}

template <class T, class Allocator = DefaultAllocator>
ReflectionDefinition<T, Allocator> RefDef(const Allocator& allocator = Allocator())
{
	return ReflectionDefinition<T, Allocator>(allocator);
}

template <class T, class Allocator = DefaultAllocator>
EnumReflectionDefinition<T, Allocator> EnumRefDef(const char* name, const Allocator& allocator = Allocator())
{
	return EnumReflectionDefinition<T, Allocator>(name, allocator);
}

template <class T, class Allocator = DefaultAllocator>
EnumReflectionDefinition<T, Allocator> EnumRefDef(const Allocator& allocator = Allocator())
{
	return EnumReflectionDefinition<T, Allocator>(allocator);
}

#include "Gaff_ReflectionDefinitionsContainers.inl"
#include "Gaff_ReflectionDefinitions.inl"

NS_END
