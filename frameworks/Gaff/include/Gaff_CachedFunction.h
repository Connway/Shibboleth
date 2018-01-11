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

/*! \file */

#pragma once

#include "Gaff_Function.h"
#include <tuple>

NS_GAFF

template <class ReturnType, class... Args>
class CachedFunction : public IFunction<ReturnType>
{
public:
	static_assert(sizeof...(Args) > 0, "CachedFunction requires at least one function argument!");

	using Func = CachedFunction<ReturnType, Args...>;

	CachedFunction(const FunctionBinder<ReturnType, Args...>& function, const Args&... args);
	CachedFunction(const FunctionBinder<ReturnType, Args...>& function, Args&&... args);
	CachedFunction(const CachedFunction<ReturnType, Args...>& function);
	CachedFunction(CachedFunction<ReturnType, Args...>&& function);
	CachedFunction(void);
	~CachedFunction(void);

	const CachedFunction<ReturnType, Args...>& operator=(const CachedFunction<ReturnType, Args...>& rhs);
	const CachedFunction<ReturnType, Args...>& operator=(CachedFunction<ReturnType, Args...>&& rhs);

	void setFunction(const FunctionBinder<ReturnType, Args...>& function);
	void moveSetArguments(Args&&... args);
	void setArguments(Args&&... args);

	template <unsigned int index>
	const typename std::tuple_element< index, std::tuple<Args...> >::type& getArgument(void) const;

	template <unsigned int index>
	typename std::tuple_element< index, std::tuple<Args...> >::type& getArgument(void);

	template <unsigned int index>
	void setArg(const typename std::tuple_element< index, std::tuple<Args...> >::type& argument);

	template <unsigned int index>
	void setArg(typename std::tuple_element< index, std::tuple<Args...> >::type&& argument);

	ReturnType call(void);
	void copy(char* buffer) const;
	bool valid(void) const;

private:
	std::tuple<Args...> _arguments;
	FunctionBinder<ReturnType, Args...> _function;
};

template <class ReturnType, class... Args>
CachedFunction<ReturnType, Args...> BindCached(const FunctionBinder<ReturnType, Args...>& function, Args&&... args);

#include "Gaff_CachedFunction.inl"

NS_END
