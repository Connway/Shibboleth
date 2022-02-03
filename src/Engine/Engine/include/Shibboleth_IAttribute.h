/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_IReflectionObject.h"
#include <Gaff_Vector.h>
#include <Gaff_Hash.h>

NS_REFLECTION

class IEnumReflectionDefinition;
class IReflectionVar;

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
	void apply(Ret(T::* /*func*/)(Args...) const) {}
	template <class T, class Ret, class... Args>
	void apply(Ret(T::* /*func*/)(Args...)) {}

	// Attributes that are applied to static class functions need to implement this template function.
	template <class T, class Ret, class... Args>
	void apply(Ret(* /*func*/)(Args...)) {}

	// Attributes that are applied to variables need to implement these template functions,
	// or at least the ones they apply to.
	template <class T, class Var>
	void apply(IReflectionVar& /*ref_var*/, Var T::* /*var*/) {}
	template <class T, class Var, class Ret>
	void apply(IReflectionVar& /*ref_var*/, Ret(T::* /*getter*/)(void) const, void (T::* /*setter*/)(Var)) {}
	template <class T, class Var, class Vec_Allocator, size_t size>
	void apply(IReflectionVar& /*ref_var*/, Gaff::Vector<Var, Vec_Allocator> T::* /*vec*/) {}
	template <class T, class Var, size_t size>
	void apply(IReflectionVar& /*ref_var*/, Var(T::* /*arr*/)[size]) {}
};

NS_END
