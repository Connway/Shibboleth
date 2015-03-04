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

#define MEMBER_FUNCTION MemberFunction<T, ReturnType, Args...>
#define FUNCTOR Functor<T, ReturnType, Args...>
#define FUNCTION Function<ReturnType, Args...>
#define STDCALLFUNCTION STDCallFunction<ReturnType, Args...>

////////////////////////////
//    STDCALL FUNCTION    //
////////////////////////////
#if defined(_WIN32) || defined(_WIN64)
template <class ReturnType, class... Args>
STDCallFunction<ReturnType, Args...>::STDCallFunction(FunctionType function):
	_function(function)
{
}

template <class ReturnType, class... Args>
STDCallFunction<ReturnType, Args...>::STDCallFunction(const Func& function):
	_function(function._function),
{
}

template <class ReturnType, class... Args>
const STDCALLFUNCTION& STDCallFunction<ReturnType, Args...>::operator=(const Func& rhs)
{
	_function = rhs._function;
	return *this;
}

template <class ReturnType, class... Args>
const STDCALLFUNCTION& STDCallFunction<ReturnType, Args...>::operator=(FunctionType rhs)
{
	_function = rhs;
	return *this;
}

template <class ReturnType, class... Args>
bool STDCallFunction<ReturnType, Args...>::operator==(const STDCALLFUNCTION& rhs) const
{
	return _function == rhs._function;
}

template <class ReturnType, class... Args>
bool STDCallFunction<ReturnType, Args...>::operator!=(const STDCALLFUNCTION& rhs) const
{
	return _function != rhs._function;
}

template <class ReturnType, class... Args>
bool STDCallFunction<ReturnType, Args...>::operator==(FunctionType rhs) const
{
	return _function == rhs;
}

template <class ReturnType, class... Args>
bool STDCallFunction<ReturnType, Args...>::operator!=(FunctionType rhs) const
{
	return _function != rhs;
}

template <class ReturnType, class... Args>
void STDCallFunction<ReturnType, Args...>::set(FunctionType function)
{
	_function = function;
}

template <class ReturnType, class... Args>
typename STDCALLFUNCTION::FunctionType STDCallFunction<ReturnType, Args...>::get(void) const
{
	return _function;
}

template <class ReturnType, class... Args>
ReturnType STDCallFunction<ReturnType, Args...>::call(Args... args) const
{
	return _function(args...);
}

template <class ReturnType, class... Args>
ReturnType STDCallFunction<ReturnType, Args...>::call(Args... args)
{
	return _function(args...);
}

template <class ReturnType, class... Args>
bool STDCallFunction<ReturnType, Args...>::valid(void) const
{
	return _function != nullptr;
}
#endif

////////////////////
//    FUNCTION    //
////////////////////
template <class ReturnType, class... Args>
Function<ReturnType, Args...>::Function(FunctionType function):
	_function(function)
{
}

template <class ReturnType, class... Args>
Function<ReturnType, Args...>::Function(const Func& function):
	_function(function._function)
{
}

template <class ReturnType, class... Args>
const FUNCTION& Function<ReturnType, Args...>::operator=(const Func& rhs)
{
	_function = rhs._function;
	return *this;
}

template <class ReturnType, class... Args>
const FUNCTION& Function<ReturnType, Args...>::operator=(FunctionType rhs)
{
	_function = rhs;
	return *this;
}

template <class ReturnType, class... Args>
bool Function<ReturnType, Args...>::operator==(const FUNCTION& rhs) const
{
	return _function == rhs._function;
}

template <class ReturnType, class... Args>
bool Function<ReturnType, Args...>::operator!=(const FUNCTION& rhs) const
{
	return _function != rhs._function;
}

template <class ReturnType, class... Args>
bool Function<ReturnType, Args...>::operator==(FunctionType rhs) const
{
	return _function == rhs;
}

template <class ReturnType, class... Args>
bool Function<ReturnType, Args...>::operator!=(FunctionType rhs) const
{
	return _function != rhs;
}

template <class ReturnType, class... Args>
void Function<ReturnType, Args...>::set(FunctionType function)
{
	_function = function;
}

template <class ReturnType, class... Args>
typename FUNCTION::FunctionType Function<ReturnType, Args...>::get(void) const
{
	return _function;
}

template <class ReturnType, class... Args>
ReturnType Function<ReturnType, Args...>::call(Args... args) const
{
	return _function(args...);
}

template <class ReturnType, class... Args>
ReturnType Function<ReturnType, Args...>::call(Args... args)
{
	return _function(args...);
}

template <class ReturnType, class... Args>
bool Function<ReturnType, Args...>::valid(void) const
{
	return _function != nullptr;
}

///////////////////////////
//    MEMBER FUNCTION    //
///////////////////////////
template <class T, class ReturnType, class... Args>
MemberFunction<T, ReturnType, Args...>::MemberFunction(T* object, FunctionType function):
	_function(function), _object(object)
{
}

template <class T, class ReturnType, class... Args>
MemberFunction<T, ReturnType, Args...>::MemberFunction(const Func& member_function):
	_function(member_function._function), _object(member_function._object)
{
}

template <class T, class ReturnType, class... Args>
const MEMBER_FUNCTION& MemberFunction<T, ReturnType, Args...>::operator=(const MEMBER_FUNCTION& rhs)
{
	_function = rhs._function;
	_object = rhs._object;
	return *this;
}

template <class T, class ReturnType, class... Args>
bool MemberFunction<T, ReturnType, Args...>::operator==(const MEMBER_FUNCTION& rhs) const
{
	return _function == rhs._function && _object == rhs._object;
}

template <class T, class ReturnType, class... Args>
bool MemberFunction<T, ReturnType, Args...>::operator!=(const MEMBER_FUNCTION& rhs) const
{
	return _function != rhs._function && _object != rhs._object;
}

template <class T, class ReturnType, class... Args>
T* MemberFunction<T, ReturnType, Args...>::getObject(void) const
{
	return _object;
}

template <class T, class ReturnType, class... Args>
typename MEMBER_FUNCTION::FunctionType MemberFunction<T, ReturnType, Args...>::getFunction(void) const
{
	return _function;
}

template <class T, class ReturnType, class... Args>
void MemberFunction<T, ReturnType, Args...>::set(T* object, FunctionType function)
{
	_function = function;
	_object = object;
}

template <class T, class ReturnType, class... Args>
Pair<T*, typename MEMBER_FUNCTION::FunctionType> MemberFunction<T, ReturnType, Args...>::get(void) const
{
	return MakePair(_object, _function);
}

template <class T, class ReturnType, class... Args>
ReturnType MemberFunction<T, ReturnType, Args...>::call(Args... args) const
{
	return (_object->*_function)(args...);
}

template <class T, class ReturnType, class... Args>
ReturnType MemberFunction<T, ReturnType, Args...>::call(Args... args)
{
	return (_object->*_function)(args...);
}

template <class T, class ReturnType, class... Args>
bool MemberFunction<T, ReturnType, Args...>::valid(void) const
{
	return _function != nullptr && _object != nullptr;
}

///////////////////
//    FUNCTOR    //
///////////////////
template <class T, class ReturnType, class... Args>
Functor<T, ReturnType, Args...>::Functor(const T& functor):
	_functor(functor)
{
}

template <class T, class ReturnType, class... Args>
const FUNCTOR& Functor<T, ReturnType, Args...>::operator=(const FUNCTOR& rhs)
{
	_functor = rhs._functor;
	return *this;
}

template <class T, class ReturnType, class... Args>
const FUNCTOR& Functor<T, ReturnType, Args...>::operator=(const T& rhs)
{
	_functor = rhs;
	return *this;
}

template <class T, class ReturnType, class... Args>
bool Functor<T, ReturnType, Args...>::operator==(const FUNCTOR& rhs) const
{
	return _functor == rhs._functor;
}

template <class T, class ReturnType, class... Args>
bool Functor<T, ReturnType, Args...>::operator!=(const FUNCTOR& rhs) const
{
	return _functor != rhs._functor;
}

template <class T, class ReturnType, class... Args>
bool Functor<T, ReturnType, Args...>::operator==(const T& rhs) const
{
	return _functor == rhs;
}

template <class T, class ReturnType, class... Args>
bool Functor<T, ReturnType, Args...>::operator!=(const T& rhs) const
{
	return _functor != rhs;
}

template <class T, class ReturnType, class... Args>
const T& Functor<T, ReturnType, Args...>::getFunctor(void) const
{
	return _functor;
}

template <class T, class ReturnType, class... Args>
T& Functor<T, ReturnType, Args...>::getFunctor(void)
{
	return _functor;
}

template <class T, class ReturnType, class... Args>
ReturnType Functor<T, ReturnType, Args...>::call(Args... args) const
{
	return _functor(args...);
}

template <class T, class ReturnType, class... Args>
ReturnType Functor<T, ReturnType, Args...>::call(Args... args)
{
	return _functor(args...);
}

template <class T, class ReturnType, class... Args>
bool Functor<T, ReturnType, Args...>::valid(void) const
{
	return true;
}

////////////////////////////
//    Function Binders    //
////////////////////////////
template <class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...>::FunctionBinder(void* object, size_t size):
	_object_size(size)
{
	assert(size <= FUNCTION_BUFFER_SIZE);
	memcpy(_function_buffer, object, size);
}

template <class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...>::FunctionBinder(const Binder& rhs):
	_object_size(rhs._object_size)
{
	memcpy(_function_buffer, rhs._function_buffer, _object_size);
}

template <class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...>::FunctionBinder(void):
	_object_size(0)
{
}

template <class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...>::~FunctionBinder(void)
{
	if (valid()) {
		IFunc* function = (IFunc*)_function_buffer;
		function->~IFunc();
	}
}

template <class ReturnType, class... Args>
ReturnType FunctionBinder<ReturnType, Args...>::operator()(Args... args) const
{
	assert(valid());
	const IFunc* function = reinterpret_cast<const IFunc*>(_function_buffer);
	return function->call(args...);
}

template <class ReturnType, class... Args>
ReturnType FunctionBinder<ReturnType, Args...>::operator()(Args... args)
{
	assert(valid());
	IFunc* function = reinterpret_cast<IFunc*>(_function_buffer);
	return function->call(args...);
}

template <class ReturnType, class... Args>
const FunctionBinder<ReturnType, Args...>& FunctionBinder<ReturnType, Args...>::operator=(const FunctionBinder<ReturnType, Args...>& rhs)
{
	memcpy(_function_buffer, rhs._function_buffer, rhs._object_size);
	_object_size = rhs._object_size;
	return *this;
}

template <class ReturnType, class... Args>
bool FunctionBinder<ReturnType, Args...>::operator==(const Binder& rhs) const
{
	return (_object_size == rhs._object_size) && (strncmp(_function_buffer, rhs._function_buffer, _object_size) == 0);
}

template <class ReturnType, class... Args>
bool FunctionBinder<ReturnType, Args...>::operator!=(const Binder& rhs) const
{
	return (_object_size != rhs._object_size) || (strncmp(_function_buffer, rhs._function_buffer, _object_size) != 0);
}

template <class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...>::operator bool(void) const
{
	return valid();
}

template <class ReturnType, class... Args>
bool FunctionBinder<ReturnType, Args...>::valid(void) const
{
	return _object_size != 0;
}

template <class ReturnType, class... Args>
const IFunction<ReturnType, Args...>& FunctionBinder<ReturnType, Args...>::getInterface(void) const
{
	assert(valid());
	return *((const IFunc*)_function_buffer);
}

template <class ReturnType, class... Args>
IFunction<ReturnType, Args...>& FunctionBinder<ReturnType, Args...>::getInterface(void)
{
	assert(valid());
	return *((IFunc*)_function_buffer);
}

//////////////////////////
//    Bind Functions    //
//////////////////////////
template <class T, class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...> Bind(T* object, ReturnType (T::*function)(Args...))
{
	MemberFunction<T, ReturnType, Args...> member_function(object, function);
	return FunctionBinder<ReturnType, Args...>(&member_function, sizeof(member_function));
}

template <class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...> Bind(ReturnType (*function)(Args...))
{
	Function<ReturnType, Args...> func(function);
	return FunctionBinder<ReturnType, Args...>(&func, sizeof(func));
}

#if defined(_WIN32) || defined(_WIN64)
template <class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...> BindSTDCall(ReturnType (__stdcall *function)(Args...))
{
	STDCallFunction<ReturnType, Args...> func(function);
	return FunctionBinder<ReturnType, Args...>(&func, sizeof(func));
}
#endif

template <class T, class ReturnType, class... Args>
FunctionBinder<ReturnType, Args...> Bind(const T& functor)
{
	Functor<T, ReturnType, Args...> ftor(functor);
	return FunctionBinder<ReturnType, Args...>(&ftor, sizeof(ftor));
}
#

////////////////////////////////////
//    Function-Tuple Unwrapper    //
////////////////////////////////////
template <unsigned int index>
class FunctionTupleUnwrapper;

template <>
class FunctionTupleUnwrapper<0>
{
public:
	template <class ReturnType, class... FunctionArgs, class... CurrentArgs>
	static ReturnType Call(const IFunction<ReturnType, FunctionArgs...>& function, const Tuple<FunctionArgs...>&, CurrentArgs&&... args)
	{
		return function(args...);
	}

	template <class ReturnType, class... FunctionArgs, class... CurrentArgs>
	static ReturnType Call(IFunction<ReturnType, FunctionArgs...>& function, Tuple<FunctionArgs...>&, CurrentArgs&&... args)
	{
		return function(args...);
	}
};

/*!
	\brief Class holder for a static function. Unwraps a \a Tuple's contents and passes them to a function.

	\tparam index Used in the function \a Call. When using this class, this value should be the size of the \a Tuple.
*/
template <unsigned int index>
class FunctionTupleUnwrapper
{
public:
	template <class ReturnType, class... FunctionArgs, class... CurrentArgs>
	static ReturnType Call(const IFunction<ReturnType, FunctionArgs...>& function, const Tuple<FunctionArgs...>& tuple, CurrentArgs&&... args)
	{
		return FunctionTupleUnwrapper<index - 1>::Call(function, tuple, TupleGet<index - 1>(tuple), args...);
	}

	/*!
		\brief Does the work of unwrapping \a tuple's contents and passing them to \a function.

		\tparam The function return type.
		\tparam FunctionArgs The arguments the function takes.
		\tparam CurrentArgs The current arguments we have unwrapped.

		\return The value returned by \a function.
	*/
	template <class ReturnType, class... FunctionArgs, class... CurrentArgs>
	static ReturnType Call(IFunction<ReturnType, FunctionArgs...>& function, Tuple<FunctionArgs...>& tuple, CurrentArgs&&... args)
	{
		return FunctionTupleUnwrapper<index - 1>::Call(function, tuple, TupleGet<index - 1>(tuple), args...);
	}
};
