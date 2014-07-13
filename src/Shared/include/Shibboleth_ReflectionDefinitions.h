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

#include "Shibboleth_HashString.h"
#include "Shibboleth_HashMap.h"
#include "Shibboleth_String.h"
#include <Gaff_SharedPtr.h>
#include <Gaff_JSON.h>
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
	template <class T> \
	ReflectionDefinition<T>::name::name(const char* key, type T::* var): IValueContainer(key), _var(var) {}

#define VAR_CONTAINER_READ(name) \
	template <class T> \
	void ReflectionDefinition<T>::name::read(const Gaff::JSON& json, T* object)

#define VAR_CONTAINER_WRITE(name) \
	template <class T> \
	void ReflectionDefinition<T>::name::write(Gaff::JSON& json, T* object) const

#define VAR_CONTAINER_VAL_TYPE(name, type) \
	template <class T> \
	typename ReflectionDefinition<T>::IValueContainer::ValueType ReflectionDefinition<T>::name::getType(void) const \
	{ \
		return type; \
	}

#define VAR_CONTAINER_SET_STRING(name) \
	template <class T> \
	void ReflectionDefinition<T>::name::set(const char* value, T* object)

#define VAR_CONTAINER_SET_UINT(name) \
	template <class T> \
	void ReflectionDefinition<T>::name::set(unsigned int value, T* object)

#define VAR_CONTAINER_SET_INT(name) \
	template <class T> \
	void ReflectionDefinition<T>::name::set(int value, T* object)

#define VAR_CONTAINER_SET_DOUBLE(name) \
	template <class T> \
	void ReflectionDefinition<T>::name::set(double value, T* object)

NS_GAFF
	class JSON;
NS_END

NS_SHIBBOLETH

template <class T>
class EnumReflectionDefinition
{
public:
	EnumReflectionDefinition(void);
	~EnumReflectionDefinition(void);

	EnumReflectionDefinition<T>& addValue(const char* name, T value);
	const char* getName(T value) const;
	T getValue(const char* name) const;

	void setAllocator(const ProxyAllocator& allocator);

	bool isDefined(void) const;
	void markDefined(void);

private:
	HashMap<AHashString, T> _values_map;
	bool _defined;
};

template <class T>
class ReflectionDefinition
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
			VT_ASTRING,
			VT_OBJECT,
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
		AString _key;
	};

	ReflectionDefinition(void);
	~ReflectionDefinition(void);

	void read(const Gaff::JSON& json, T* object);
	void write(Gaff::JSON& json, T* object) const;

	template <class T2>
	ReflectionDefinition<T>& addBaseClass(const ReflectionDefinition<T2>& base_ref_def);

	template <class T2>
	ReflectionDefinition<T>& addObject(const char* key, T2 T::* var, ReflectionDefinition<T2> T2::* var_ref_def);

	template <class T2>
	ReflectionDefinition<T>& addEnum(const char* key, T2 T::* var, const EnumReflectionDefinition<T2>& ref_def);

	ReflectionDefinition<T>& addDouble(const char* key, double T::* var);
	ReflectionDefinition<T>& addFloat(const char* key, float T::* var);
	ReflectionDefinition<T>& addUInt(const char* key, unsigned int T::* var);
	ReflectionDefinition<T>& addInt(const char* key, int T::* var);
	ReflectionDefinition<T>& addUShort(const char* key, unsigned short T::* var);
	ReflectionDefinition<T>& addShort(const char* key, short T::* var);
	ReflectionDefinition<T>& addUChar(const char* key, unsigned char T::* var);
	ReflectionDefinition<T>& addChar(const char* key, char T::* var);
	ReflectionDefinition<T>& addBool(const char* key, bool T::* var);
	ReflectionDefinition<T>& addANSIString(const char* key, AString T::* var);

	ReflectionDefinition<T>& addCustom(const char* key, IValueContainer* container);

	bool isDefined(void) const;
	void markDefined(void);

	void setAllocator(const ProxyAllocator& allocator);

private:
	typedef Gaff::SharedPtr<IValueContainer, ProxyAllocator> ValueContainerPtr;

	VAR_CONTAINER(DoubleContainer, double);
	VAR_CONTAINER(FloatContainer, float);
	VAR_CONTAINER(UIntContainer, unsigned int);
	VAR_CONTAINER(IntContainer, int);
	VAR_CONTAINER(UShortContainer, unsigned short);
	VAR_CONTAINER(ShortContainer, short);
	VAR_CONTAINER(UCharContainer, unsigned char);
	VAR_CONTAINER(CharContainer, char);
	VAR_CONTAINER(BoolContainer, bool);
	VAR_CONTAINER(ANSIStringContainer, AString);

	template <class T2>
	class ObjectContainer : public IValueContainer
	{
	public:
		ObjectContainer(const char* key, T2 T::* var, ReflectionDefinition<T2> T2::* var_ref_def);

		void read(const Gaff::JSON& json, T* object);
		void write(Gaff::JSON& json, T* object) const;

		ValueType getType(void) const;

	private:
		ReflectionDefinition<T2> T2::* _var_ref_def;
		T2 T::* _var;
	};

	template <class T2>
	class EnumContainer : public IValueContainer
	{
	public:
		EnumContainer(const char* key, T2 T::* var, const EnumReflectionDefinition<T2>& var_ref_def);

		void read(const Gaff::JSON& json, T* object);
		void write(Gaff::JSON& json, T* object) const;

		void set(const char* value, T* object);
		void set(unsigned int value, T* object);
		void set(int value, T* object);
		void set(double value, T* object);

		ValueType getType(void) const;

	private:
		const EnumReflectionDefinition<T2>& _var_ref_def;
		T2 T::* _var;

		GAFF_NO_COPY(EnumContainer);
	};

	template <class T2>
	class BaseValueContainer : public IValueContainer
	{
	public:
		BaseValueContainer(const char* key, typename const ReflectionDefinition<T2>::ValueContainerPtr& value_ptr);

		void read(const Gaff::JSON& json, T* object);
		void write(Gaff::JSON& json, T* object) const;

		void set(const char* value, T* object);
		void set(unsigned int value, T* object);
		void set(int value, T* object);
		void set(double value, T* object);

		ValueType getType(void) const;

	private:
		typename ReflectionDefinition<T2>::ValueContainerPtr _value_ptr;
	};

	HashMap<AHashString, ValueContainerPtr> _value_containers;
	bool _defined;
};

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
// Commented out until I genericise the allocator for this and move it to Gaff.
//template <class T>
//ReflectionDefinition<T> RefDef(void)
//{
//	return ReflectionDefinition<T>();
//}

#include "Shibboleth_ReflectionDefinitionsContainers.inl"
#include "Shibboleth_ReflectionDefinitions.inl"

NS_END
