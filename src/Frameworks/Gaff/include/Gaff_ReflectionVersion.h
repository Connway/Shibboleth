/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gaff_Assert.h"
#include "Gaff_Hash.h"

NS_GAFF

template <class T>
class ReflectionVersion final
{
public:
	template <class Base>
	ReflectionVersion& base(const char* name);

	template <class Base>
	ReflectionVersion& base(void);

	template <class... Args>
	ReflectionVersion& ctor(void);

	template <class Var, size_t size>
	ReflectionVersion& var(const char(&name)[size], Var T::*ptr, bool read_only = false);

	template <class Ret, class Var, size_t size>
	ReflectionVersion& var(const char(&name)[size], Ret (T::*getter)(void) const, void (T::*setter)(Var));

	template <size_t size, class Ret, class... Args>
	ReflectionVersion& func(const char (&name)[size], Ret (T::*ptr)(Args...) const);

	template <size_t size, class Ret, class... Args>
	ReflectionVersion& func(const char (&name)[size], Ret (T::*ptr)(Args...));

	template <class... Args>
	ReflectionVersion& classAttrs(const Args&...);

	template <size_t size, class... Args>
	ReflectionVersion& varAttrs(const char(&name)[size], const Args&...);

	template <size_t size, class... Args>
	ReflectionVersion& funcAttrs(const char(&name)[size], const Args&...);

	ReflectionVersion& version(uint32_t version);

	Hash64 getHash(void) const;

	void finish(void);

private:
	Hash64 _hash = INIT_HASH64;
};

NS_END

#include "Gaff_ReflectionVersion.inl"