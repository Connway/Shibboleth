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

template <class ReturnType, class... Args>
CachedFunction<ReturnType, Args...>::CachedFunction(const FunctionBinder<ReturnType, Args...>& function, const Args&... args):
	_arguments(args...), _function(function)
{
}

template <class ReturnType, class... Args>
CachedFunction<ReturnType, Args...>::CachedFunction(const FunctionBinder<ReturnType, Args...>& function, Args&&... args):
	_arguments(std::forward<Args>(args)...), _function(function)
{
}

template <class ReturnType, class... Args>
CachedFunction<ReturnType, Args...>::CachedFunction(const CachedFunction<ReturnType, Args...>& function):
	_arguments(function._arguments), _function(function._function)
{
}

template <class ReturnType, class... Args>
CachedFunction<ReturnType, Args...>::CachedFunction(CachedFunction<ReturnType, Args...>&& function):
	_arguments(std::move(function._arguments)), _function(std::move(function._function))
{
}

template <class ReturnType, class... Args>
CachedFunction<ReturnType, Args...>::CachedFunction(void)
{
}

template <class ReturnType, class... Args>
CachedFunction<ReturnType, Args...>::~CachedFunction(void)
{
}

template <class ReturnType, class... Args>
const CachedFunction<ReturnType, Args...>& CachedFunction<ReturnType, Args...>::operator=(const CachedFunction<ReturnType, Args...>& rhs)
{
	_arguments = rhs._arguments;
	_function = rhs._function;
	return *this;
}

template <class ReturnType, class... Args>
const CachedFunction<ReturnType, Args...>& CachedFunction<ReturnType, Args...>::operator=(CachedFunction<ReturnType, Args...>&& rhs)
{
	_arguments = std::move(rhs._arguments);
	_function = std::move(rhs._function);
	return *this;
}

template <class ReturnType, class... Args>
void CachedFunction<ReturnType, Args...>::setFunction(const FunctionBinder<ReturnType, Args...>& function)
{
	_function = function;
}

template <class ReturnType, class... Args>
void CachedFunction<ReturnType, Args...>::moveSetArguments(Args&&... args)
{
	_arguments.moveSet(std::move(args)...);
}

template <class ReturnType, class... Args>
void CachedFunction<ReturnType, Args...>::setArguments(Args&&... args)
{
	_arguments.set(args...);
}

template <class ReturnType, class... Args>
template <unsigned int index>
const typename std::tuple_element< index, std::tuple<Args...> >::type& CachedFunction<ReturnType, Args...>::getArgument(void) const
{
	return std::get<index>(_arguments);
}

template <class ReturnType, class... Args>
template <unsigned int index>
typename std::tuple_element< index, std::tuple<Args...> >::type& CachedFunction<ReturnType, Args...>::getArgument(void)
{
	return std::get<index>(_arguments);
}

template <class ReturnType, class... Args>
template <unsigned int index>
void CachedFunction<ReturnType, Args...>::setArg(const typename std::tuple_element< index, std::tuple<Args...> >::type& argument)
{
	std::get<index>(_arguments) = argument;
}

template <class ReturnType, class... Args>
template <unsigned int index>
void CachedFunction<ReturnType, Args...>::setArg(typename std::tuple_element< index, std::tuple<Args...> >::type&& argument)
{
	std::get<index>(_arguments) = std::move(argument);
}

template <class ReturnType, class... Args>
ReturnType CachedFunction<ReturnType, Args...>::call(void) const
{
	return FunctionTupleUnwrapper<std::tuple<Args...>::_Mysize>::Call(_function.getInterface(), _arguments);
}

template <class ReturnType, class... Args>
ReturnType CachedFunction<ReturnType, Args...>::call(void)
{
	return FunctionTupleUnwrapper<std::tuple<Args...>::_Mysize>::Call(_function.getInterface(), _arguments);
}

template <class ReturnType, class... Args>
bool CachedFunction<ReturnType, Args...>::valid(void) const
{
	return _function.valid();
}

// Cached Function Binder
template <class ReturnType, class... Args>
CachedFunction<ReturnType, Args...> BindCached(const FunctionBinder<ReturnType, Args...>& function, Args&&... args)
{
	return CachedFunction<ReturnType, Args...>(function, std::forward(args)...);
}
