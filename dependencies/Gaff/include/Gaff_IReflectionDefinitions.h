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

#include "Gaff_String.h"
#include "Gaff_Pair.h"

NS_GAFF

class JSON;

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
	VT_CUSTOM,
	VT_SIZE
};

template <class Allocator>
class IEnumReflectionDefinition
{
public:
	IEnumReflectionDefinition(void) {}
	virtual ~IEnumReflectionDefinition(void) {}

	virtual const char* getNameGeneric(unsigned int value) const = 0;
	virtual unsigned int getValueGeneric(const char* name) const = 0;
	virtual bool getValueGeneric(const char* name, unsigned int& value) const = 0;

	virtual Pair<AString<Allocator>, unsigned int> getEntryGeneric(unsigned int index) const = 0;
	virtual size_t getNumEntries(void) const = 0;

	virtual const char* getEnumName(void) const = 0;
};

class IReflectionDefinition
{
public:
	class IValueContainer
	{
	public:
		IValueContainer(void) {}
		virtual ~IValueContainer(void) {}

		virtual void get(void* /*out*/, const void* /*object*/) const { GAFF_ASSERT_MSG(false, "GET: Value container is not a value/string/object!"); }
		virtual void get(void* /*out*/, size_t /*index*/, const void* /*object*/) const { GAFF_ASSERT_MSG(false, "GET: Value container it not an array!"); }
		virtual void set(const void* /*value*/, void* /*object*/) { GAFF_ASSERT_MSG(false, "SET: Value container is not a value/string/object!"); } // Set object
		virtual void set(const void* /*value*/, size_t /*index*/, void* /*object*/) { GAFF_ASSERT_MSG(false, "SET: Value container it not an array!"); } // Set array element

		// Array specific functionality
		virtual size_t size(const void* /*object*/) const { GAFF_ASSERT_MSG(false, "SIZE: Value container it not an array!"); return 0; }
		virtual void resize(size_t /*new_size*/, void* /*object*/) { GAFF_ASSERT_MSG(false, "RESIZE: Value container it not an array or is a static array!"); }
		virtual void move(size_t /*src_index*/, size_t /*dest_index*/, void* /*object*/) { GAFF_ASSERT_MSG(false, "MOVE: Value container it not an array!"); }

		virtual bool isFixedArray(void) const = 0;
		virtual bool isArray(void) const = 0;

		virtual ReflectionValueType getType(void) const = 0;
	};


	template <class T>
	T* getInterface(void* object) const
	{
		return reinterpret_cast<T*>(getInterface(T::gHash));
	}

	IReflectionDefinition(void) {}
	virtual ~IReflectionDefinition(void) {}

	/*!
		\brief Reads data from a JSON object and populates reflected variables.
		\param json The JSON object to read from.
		\param object The reflected object we are populating.
	*/
	virtual void read(const Gaff::JSON& json, void* object) = 0;

	/*!
		\brief Writes data from the reflected object to the JSON object.
		\param json The JSON object to write to.
		\param object The object to read from.
	*/
	virtual void write(Gaff::JSON& json, const void* object) const = 0;

	/*!
		\brief Gets the address offset of a type that this object implements or derives from.

		\param class_id The class ID whose interface we wish to retrieve.
		\param object The object we are performing the query on.

		\return
			The address where the interface of \a class_id begins at,
			otherwise nullptr if \a object does not implement\inherit that interface.
	*/
	virtual const void* getInterface(ReflectionHash class_id, const void* object) const = 0;
	virtual void* getInterface(ReflectionHash class_id, const void* object) = 0;

	/*!
		\brief Gets the name of the reflected type.
		\return The name of the reflected type.
	*/
	virtual const char* getName(void) const = 0;
};

NS_END
