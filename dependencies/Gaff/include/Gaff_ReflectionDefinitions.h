/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#if defined(_WIN32) || defined(_WIN64)
	#ifndef snprintf
		#define snprintf
	#endif
#else
	#include <cstdio>
#endif

#define VAR_CONTAINER(name, type) \
	class name : public IValueContainer \
	{ \
	public: \
		typedef type (T::*Getter)(void) const; \
		typedef void (T::*Setter)(type); \
		name(const char* key, Getter getter, Setter setter); \
		name(const char* key, type T::* var); \
		void read(const JSON& json, T* object); \
		void write(JSON& json, T* object) const; \
		ReflectionValueType getType(void) const; \
		void set(const char* value, T* object); \
		void set(unsigned int value, T* object); \
		void set(int value, T* object); \
		void set(double value, T* object); \
	private: \
		type T::* _var; \
		Getter _getter; \
		Setter _setter; \
	}

#define VAR_CONTAINER_REF(name, type) \
	class name : public IValueContainer \
	{ \
	public: \
		typedef const type& (T::*Getter)(void) const; \
		typedef void (T::*Setter)(const type&); \
		name(const char* key, Getter getter, Setter setter); \
		name(const char* key, type T::* var); \
		void read(const JSON& json, T* object); \
		void write(JSON& json, T* object) const; \
		ReflectionValueType getType(void) const; \
		void set(const char* value, T* object); \
		void set(unsigned int value, T* object); \
		void set(int value, T* object); \
		void set(double value, T* object); \
	private: \
		type T::* _var; \
		Getter _getter; \
		Setter _setter; \
	}

#define VAR_CONTAINER_CONSTRUCTOR(name, type) \
	template <class T, class Allocator> \
	ReflectionDefinition<T, Allocator>::name::name(const char* key, Getter getter, Setter setter): IValueContainer(key), _getter(getter), _setter(setter), _var(nullptr) {} \
	template <class T, class Allocator> \
	ReflectionDefinition<T, Allocator>::name::name(const char* key, type T::* var): IValueContainer(key), _getter(nullptr), _setter(nullptr), _var(var) {}

#define VAR_CONTAINER_CONSTRUCTOR_GET_SET(name, type) \

#define VAR_CONTAINER_READ(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::read(const JSON& json, T* object)

#define VAR_CONTAINER_WRITE(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::write(JSON& json, T* object) const

#define VAR_CONTAINER_VAL_TYPE(name, type) \
	template <class T, class Allocator> \
	ReflectionValueType ReflectionDefinition<T, Allocator>::name::getType(void) const \
	{ \
		return type; \
	}

#define VAR_CONTAINER_SET_STRING(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::set(const char* value, T* object)

#define VAR_CONTAINER_SET_UINT(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::set(unsigned int value, T* object)

#define VAR_CONTAINER_SET_INT(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::set(int value, T* object)

#define VAR_CONTAINER_SET_DOUBLE(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::set(double value, T* object)

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

	Pair<AString<Allocator>, T> getEntry(unsigned int index) const;

	const char* getNameGeneric(unsigned int value) const;
	unsigned int getValueGeneric(const char* name) const;

	Pair<AString<Allocator>, unsigned int> getEntryGeneric(unsigned int index) const;
	unsigned int getNumEntries(void) const;

	const char* getEnumName(void) const;

	void setAllocator(const Allocator& allocator);
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

enum ReflectionValueType
{
	VT_DOUBLE = 0,
	VT_FLOAT,
	VT_UINT,
	VT_INT,
	VT_USHORT,
	VT_SHORT,
	VT_UCHAR,
	VT_CHAR,
	VT_BOOL,
	VT_ENUM,
	VT_STRING,
	VT_OBJECT,
	VT_ARRAY,
	VT_CUSTOM,
	VT_SIZE
};

template <class T, class Allocator>
class ReflectionDefinition : public IReflectionDefinition
{
public:
	class IValueContainer
	{
	public:
		IValueContainer(const char* key) : _key(key) {}
		virtual ~IValueContainer(void) {}

		virtual void read(const JSON& json, T* object) = 0;
		virtual void write(JSON& json, T* object) const = 0;

		virtual void set(const char*, T*) {}
		virtual void set(unsigned int, T*) {}
		virtual void set(int, T*) {}
		virtual void set(double, T*) {}

		virtual ReflectionValueType getType(void) const = 0;

	protected:
		AString<Allocator> _key;
	};

	ReflectionDefinition(ReflectionDefinition<T, Allocator>&& ref_def);
	ReflectionDefinition(const char* name, const Allocator& allocator = Allocator());
	ReflectionDefinition(const Allocator& allocator = Allocator());
	~ReflectionDefinition(void);

	const ReflectionDefinition<T, Allocator>& operator=(ReflectionDefinition<T, Allocator>&& rhs);

	void read(const JSON& json, void* object);
	void write(JSON& json, void* object) const;

	void* getInterface(unsigned int class_id, const void* object) const;

	const char* getName(void) const;

	void read(const JSON& json, T* object);
	void write(JSON& json, T* object) const;

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addBaseClass(ReflectionDefinition<T2, Allocator>& base_ref_def, unsigned int class_id);

	// This function does not check if the base class is finished being defined. This is so you can add just casts.
	template <class T2>
	ReflectionDefinition<T, Allocator>&& addBaseClass(unsigned int class_id);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addBaseClass(void);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addBaseClassInterfaceOnly(void);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addObject(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def);

	template <class T2>
	ReflectionDefinition<T, Allocator>&& addEnum(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& ref_def);

	//template <class T2>
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<T2> T::* var, ReflectionDefinition<T2, Allocator> T2::* elem_ref_def);

	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<double> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<float> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<unsigned int> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<int> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<unsigned short> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<short> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<unsigned char> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<char> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<bool> T::* var);
	//ReflectionDefinition<T, Allocator>&& addArray(const char* key, Array<AString> T::* var);

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

	ReflectionDefinition<T, Allocator>&& addCustom(const char* key, IValueContainer* container);

	bool isDefined(void) const;
	void markDefined(void);

	ReflectionDefinition<T, Allocator>&& setAllocator(const Allocator& allocator);
	void clear(void);

	ReflectionDefinition<T, Allocator>&& macroFix(void);

private:
	typedef SharedPtr<IValueContainer, Allocator> ValueContainerPtr;

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

	template <class T2>
	class ObjectContainer : public IValueContainer
	{
	public:
		typedef const T2& (T::*Getter)(void) const;
		typedef void (T::*Setter)(const T2& value);

		ObjectContainer(const char* key, ReflectionDefinition<T2, Allocator>& var_ref_def, Getter getter, Setter setter);
		ObjectContainer(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def);

		void read(const JSON& json, T* object);
		void write(JSON& json, T* object) const;

		void set(const T2& value, T* object);

		ReflectionValueType getType(void) const;

	private:
		Getter _getter;
		Setter _setter;
		ReflectionDefinition<T2, Allocator>& _var_ref_def;
		T2 T::* _var;
	};

	template <class T2>
	class EnumContainer : public IValueContainer
	{
	public:
		typedef T2 (T::*Getter)(void) const;
		typedef void (T::*Setter)(T2 value);

		EnumContainer(const char* key, const EnumReflectionDefinition<T2, Allocator>& ref_def, Getter getter, Setter setter);
		EnumContainer(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& var_ref_def);

		void read(const JSON& json, T* object);
		void write(JSON& json, T* object) const;

		void set(const char* value, T* object);
		void set(unsigned int value, T* object);
		void set(int value, T* object);
		void set(double value, T* object);

		ReflectionValueType getType(void) const;

	private:
		Getter _getter;
		Setter _setter;
		const EnumReflectionDefinition<T2, Allocator>& _var_ref_def;
		T2 T::* _var;

		GAFF_NO_COPY(EnumContainer);
	};

	template <class T2>
	class BaseValueContainer : public IValueContainer
	{
	public:
		BaseValueContainer(const char* key, const typename ReflectionDefinition<T2, Allocator>::ValueContainerPtr& value_ptr);

		void read(const JSON& json, T* object);
		void write(JSON& json, T* object) const;

		void set(const char* value, T* object);
		void set(unsigned int value, T* object);
		void set(int value, T* object);
		void set(double value, T* object);

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
	Array<Pair< unsigned int, FunctionBinder<void*, const void*> >, Allocator> _base_ids;
	Array<IOnCompleteFunctor*> _callback_references;

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
	static unsigned int GetReflectionHash(void); \
private: \
	static unsigned int g_Hash; \
	static Gaff::ReflectionDefinition<ClassName, Allocator> g_Ref_Def

#define CLASS_HASH(ClassName) Gaff::FNV1aHash32(#ClassName, strlen(#ClassName))

#define REF_IMPL(ClassName, Allocator) \
unsigned int ClassName::GetReflectionHash(void) { return g_Hash; } \
Gaff::ReflectionDefinition<ClassName, Allocator>& ClassName::GetReflectionDefinition(void) { return g_Ref_Def; } \
unsigned int ClassName::g_Hash = CLASS_HASH(ClassName); \
Gaff::ReflectionDefinition<ClassName, Allocator> ClassName::g_Ref_Def

// uses default allocator
#define REF_IMPL_ASSIGN_DEFAULT(ClassName, Allocator) \
unsigned int ClassName::GetReflectionHash(void) { return g_Hash; } \
Gaff::ReflectionDefinition<ClassName, Allocator>& ClassName::GetReflectionDefinition(void) { return g_Ref_Def; } \
unsigned int ClassName::g_Hash = CLASS_HASH(ClassName); \
Gaff::ReflectionDefinition<ClassName, Allocator> ClassName::g_Ref_Def = Gaff::RefDef<ClassName, Allocator>(#ClassName).macroFix()

#define REF_IMPL_ASSIGN(ClassName, Allocator, allocator_instance) \
unsigned int ClassName::GetReflectionHash(void) { return g_Hash; } \
Gaff::ReflectionDefinition<ClassName, Allocator>& ClassName::GetReflectionDefinition(void) { return g_Ref_Def; } \
unsigned int ClassName::g_Hash = CLASS_HASH(ClassName); \
Gaff::ReflectionDefinition<ClassName, Allocator> ClassName::g_Ref_Def = Gaff::RefDef<ClassName, Allocator>(#ClassName, allocator_instance).macroFix()

#define ADD_BASE_CLASS_INTERFACE_ONLY(ClassName) addBaseClass<ClassName>(CLASS_HASH(ClassName))

#define REF_IMPL_REQ(ClassName) \
void* ClassName::rawRequestInterface(unsigned int class_id) const \
{ \
	return g_Ref_Def.getInterface(class_id, this); \
}

#define ENUM_REF_DEF(EnumName, Allocator) extern Gaff::EnumReflectionDefinition<EnumName, Allocator> g_##EnumName##_Ref_Def
#define ENUM_REF_IMPL(EnumName, Allocator) Gaff::EnumReflectionDefinition<EnumName, Allocator> g_##EnumName##_Ref_Def
#define ENUM_REF_IMPL_ASSIGN(EnumName, Allocator, allocator_instance) Gaff::EnumReflectionDefinition<EnumName, Allocator> g_##EnumName##_Ref_Def = Gaff::EnumRefDef<EnumName, Allocator>(#EnumName, allocator_instance).macroFix()
#define ENUM_REF_IMPL_ASSIGN_DEFAULT(EnumName, Allocator, allocator_instance) Gaff::EnumReflectionDefinition<EnumName, Allocator> g_##EnumName##_Ref_Def = Gaff::EnumRefDef<EnumName, Allocator>(#EnumName).macroFix()


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

#include "Gaff_ReflectionDefinitionsContainers.inl"
#include "Gaff_ReflectionDefinitions.inl"

NS_END
