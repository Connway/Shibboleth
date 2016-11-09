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

#include "Gaff_Hash.h"

NS_GAFF

template <class T>
class ReflectionVersion final
{
public:
	ReflectionVersion& baseClass(const char*, ReflectionHash interface_name, ptrdiff_t offset);

	template <class Base>
	ReflectionVersion& baseClass(void);

	template <class Var, size_t size>
	ReflectionVersion& var(const char(&name)[size], Var T::*ptr);

	template <class Ret, class Var, size_t size>
	ReflectionVersion& var(const char(&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var));

	void finish(void);

	Gaff::Hash64 getHash(void) const;

private:
	Gaff::Hash64 _hash = INIT_HASH64;
};

NS_END

#include "Gaff_ReflectionVersion.inl"
