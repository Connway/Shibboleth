/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

/*! \file */

#pragma once

#include "Gaff_String.h"

NS_GAFF

/*!
	\brief A class that represents a dynamically loaded module. (*.so, *.dll, *.dylib)
*/
class DynamicModule
{
public:
	DynamicModule(void);
	~DynamicModule(void);

	/*!
		\brief Retrieves a function from the dynamic module.

		\tparam Func The function pointer type the address will be cast to.
		\param name The name of the symbol to retrieve from the dynamic module.

		\return The function with the symbol \a name, otherwise nullptr.
	*/
	template <class Func>
	Func getFunc(const char* name) const
	{
		return reinterpret_cast<Func>(getAddress(name));
	}

	/*!
		\brief Retrieves a variable from the dynamic module.

		\tparam Func The type the address will be cast to.
		\param name The name of the symbol to retrieve from the dynamic module.

		\return The variable with the symbol \a name, otherwise nullptr.
	*/
	template <class T>
	T* getVariable(const char* name) const
	{
		return reinterpret_cast<T*>(getAddress(name));
	}

	/*!
		\brief Loads the dynamic module found at \a filename from the disk.
		\param filename The name of the file to load off the disk.
		\return Whether the dynamic module was successfully loaded.
	*/
	INLINE bool load(const char* filename);
	INLINE bool destroy(void);

	/*!
		\brief Retrieves the address of the requested symbol \a name.
		\param name The name of the symbol to retrieve from the dynamic module.
		\return The address of the symbol \a name, otherwise nullptr.
	*/
	INLINE void* getAddress(const char* name) const;

	INLINE static const char* GetErrorString(void);

private:
	void* _module;

	GAFF_NO_COPY(DynamicModule);
	GAFF_NO_MOVE(DynamicModule);
};

NS_END
