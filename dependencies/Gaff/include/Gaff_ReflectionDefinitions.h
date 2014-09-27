/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include "Gaff_IReflectionDefinition.h"
#include "Gaff_HashString.h"
#include "Gaff_HashMap.h"
#include "Gaff_String.h"
#include "Gaff_Array.h"
#include "Gaff_SharedPtr.h"
#include "Gaff_Function.h"
#include "Gaff_JSON.h"
#include "Gaff_Pair.h"
#include "Gaff_Math.h"
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
		name(const char* key, type T::* var); \
		void read(const Gaff::JSON& json, T* object); \
		void write(Gaff::JSON& json, T* object) const; \
		ValueType getType(void) const; \
		void set(const char* value, T* object); \
		void set(unsigned int value, T* object); \
		void set(int value, T* object); \
		void set(double value, T* object); \
	private: \
		type T::* _var; \
	}

#define VAR_CONTAINER_CONSTRUCTOR(name, type) \
	template <class T, class Allocator> \
	ReflectionDefinition<T, Allocator>::name::name(const char* key, type T::* var): IValueContainer(key), _var(var) {}

#define VAR_CONTAINER_READ(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::read(const Gaff::JSON& json, T* object)

#define VAR_CONTAINER_WRITE(name) \
	template <class T, class Allocator> \
	void ReflectionDefinition<T, Allocator>::name::write(Gaff::JSON& json, T* object) const

#define VAR_CONTAINER_VAL_TYPE(name, type) \
	template <class T, class Allocator> \
	typename ReflectionDefinition<T, Allocator>::IValueContainer::ValueType ReflectionDefinition<T, Allocator>::name::getType(void) const \
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

template <class T, class Allocator = DefaultAllocator>
class EnumReflectionDefinition
{
public:
	EnumReflectionDefinition(void);
	~EnumReflectionDefinition(void);

	EnumReflectionDefinition<T, Allocator>& addValue(const char* name, T value);
	const char* getName(T value) const;
	T getValue(const char* name) const;

	void setAllocator(const Allocator& allocator);

	bool isDefined(void) const;
	void markDefined(void);

private:
	HashMap<AHashString<Allocator>, T, Allocator> _values_map;
	bool _defined;

	GAFF_NO_COPY(EnumReflectionDefinition);
	GAFF_NO_MOVE(EnumReflectionDefinition);
};

template <class T, class Allocator>
class ReflectionDefinition : public IReflectionDefinition
{
public:
	class IValueContainer
	{
	public:
		enum ValueType
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

		IValueContainer(const char* key) : _key(key) {}
		virtual ~IValueContainer(void) {}

		virtual void read(const Gaff::JSON& json, T* object) = 0;
		virtual void write(Gaff::JSON& json, T* object) const = 0;

		virtual void set(const char*, T*) {}
		virtual void set(unsigned int, T*) {}
		virtual void set(int, T*) {}
		virtual void set(double, T*) {}

		virtual ValueType getType(void) const = 0;

	protected:
		AString<Allocator> _key;
	};

	ReflectionDefinition(void);
	~ReflectionDefinition(void);

	void read(const Gaff::JSON& json, void* object);
	void write(Gaff::JSON& json, void* object) const;

	void* getInterface(unsigned int class_id, const void* object) const;

	void read(const Gaff::JSON& json, T* object);
	void write(Gaff::JSON& json, T* object) const;

	template <class T2>
	ReflectionDefinition<T, Allocator>& addBaseClass(const ReflectionDefinition<T2, Allocator>& base_ref_def, unsigned int class_id);

	template <class T2>
	ReflectionDefinition<T, Allocator>& addBaseClass(unsigned int class_id);

	template <class T2>
	ReflectionDefinition<T, Allocator>& addBaseClass(void);

	template <class T2>
	ReflectionDefinition<T, Allocator>& addBaseClassInterfaceOnly(void);

	template <class T2>
	ReflectionDefinition<T, Allocator>& addObject(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def);

	template <class T2>
	ReflectionDefinition<T, Allocator>& addEnum(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& ref_def);

	//template <class T2>
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<T2> T::* var, ReflectionDefinition<T2, Allocator> T2::* elem_ref_def);

	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<double> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<float> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<unsigned int> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<int> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<unsigned short> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<short> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<unsigned char> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<char> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<bool> T::* var);
	//ReflectionDefinition<T, Allocator>& addArray(const char* key, Array<AString> T::* var);

	ReflectionDefinition<T, Allocator>& addDouble(const char* key, double T::* var);
	ReflectionDefinition<T, Allocator>& addFloat(const char* key, float T::* var);
	ReflectionDefinition<T, Allocator>& addUInt(const char* key, unsigned int T::* var);
	ReflectionDefinition<T, Allocator>& addInt(const char* key, int T::* var);
	ReflectionDefinition<T, Allocator>& addUShort(const char* key, unsigned short T::* var);
	ReflectionDefinition<T, Allocator>& addShort(const char* key, short T::* var);
	ReflectionDefinition<T, Allocator>& addUChar(const char* key, unsigned char T::* var);
	ReflectionDefinition<T, Allocator>& addChar(const char* key, char T::* var);
	ReflectionDefinition<T, Allocator>& addBool(const char* key, bool T::* var);
	ReflectionDefinition<T, Allocator>& addString(const char* key, AString<Allocator> T::* var);

	ReflectionDefinition<T, Allocator>& addCustom(const char* key, IValueContainer* container);

	bool isDefined(void) const;
	void markDefined(void);

	void setAllocator(const Allocator& allocator);

private:
	typedef Gaff::SharedPtr<IValueContainer, Allocator> ValueContainerPtr;

	VAR_CONTAINER(DoubleContainer, double);
	VAR_CONTAINER(FloatContainer, float);
	VAR_CONTAINER(UIntContainer, unsigned int);
	VAR_CONTAINER(IntContainer, int);
	VAR_CONTAINER(UShortContainer, unsigned short);
	VAR_CONTAINER(ShortContainer, short);
	VAR_CONTAINER(UCharContainer, unsigned char);
	VAR_CONTAINER(CharContainer, char);
	VAR_CONTAINER(BoolContainer, bool);
	VAR_CONTAINER(StringContainer, AString<Allocator>);

	template <class T2>
	class ObjectContainer : public IValueContainer
	{
	public:
		ObjectContainer(const char* key, T2 T::* var, ReflectionDefinition<T2, Allocator>& var_ref_def);

		void read(const Gaff::JSON& json, T* object);
		void write(Gaff::JSON& json, T* object) const;

		ValueType getType(void) const;

	private:
		ReflectionDefinition<T2, Allocator>& _var_ref_def;
		T2 T::* _var;
	};

	template <class T2>
	class EnumContainer : public IValueContainer
	{
	public:
		EnumContainer(const char* key, T2 T::* var, const EnumReflectionDefinition<T2, Allocator>& var_ref_def);

		void read(const Gaff::JSON& json, T* object);
		void write(Gaff::JSON& json, T* object) const;

		void set(const char* value, T* object);
		void set(unsigned int value, T* object);
		void set(int value, T* object);
		void set(double value, T* object);

		ValueType getType(void) const;

	private:
		const EnumReflectionDefinition<T2, Allocator>& _var_ref_def;
		T2 T::* _var;

		GAFF_NO_COPY(EnumContainer);
	};

	template <class T2>
	class BaseValueContainer : public IValueContainer
	{
	public:
		BaseValueContainer(const char* key, typename const ReflectionDefinition<T2, Allocator>::ValueContainerPtr& value_ptr);

		void read(const Gaff::JSON& json, T* object);
		void write(Gaff::JSON& json, T* object) const;

		void set(const char* value, T* object);
		void set(unsigned int value, T* object);
		void set(int value, T* object);
		void set(double value, T* object);

		ValueType getType(void) const;

	private:
		typename ReflectionDefinition<T2, Allocator>::ValueContainerPtr _value_ptr;
	};

	template <class T2>
	static void* BaseClassCast(const void* object)
	{
		// Holy balls batman. That's a lot of casts.
		return static_cast<T2*>(reinterpret_cast<T*>(const_cast<void*>(object)));
	}

	HashMap<AHashString<Allocator>, ValueContainerPtr, Allocator> _value_containers;
	Array<Gaff::Pair< unsigned int, Gaff::FunctionBinder<void*, const void*> >, Allocator> _base_ids;

	Allocator _allocator;
	bool _defined;

	GAFF_NO_COPY(ReflectionDefinition);
	GAFF_NO_MOVE(ReflectionDefinition);
};

#define REF_DEF(ClassName, Allocator) \
public: \
	static unsigned int g_Hash; \
	static Gaff::ReflectionDefinition<ClassName, Allocator> g_Ref_Def

#define REF_IMPL(ClassName, Allocator) \
unsigned int ClassName::g_Hash = Gaff::FNV1aHash32(#ClassName, strlen(#ClassName)); \
Gaff::ReflectionDefinition<ClassName, Allocator> ClassName::g_Ref_Def

#define REF_IMPL_REQ(ClassName) \
void* ClassName::rawRequestInterface(unsigned int class_id) const \
{ \
	return g_Ref_Def.getInterface(class_id, this); \
}


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
template <class T, class Allocator>
ReflectionDefinition<T, Allocator> RefDef(void)
{
	return ReflectionDefinition<T, Allocator>();
}

#include "Gaff_ReflectionDefinitionsContainers.inl"
#include "Gaff_ReflectionDefinitions.inl"

NS_END
