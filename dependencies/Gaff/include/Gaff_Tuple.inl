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

// Tuple
template <class First, class... Rest>
Tuple<First, Rest...>::Tuple(const First& first, const Rest&... rest):
	Tuple<Rest...>(rest...), _value(first)
{
}

template <class First, class... Rest>
Tuple<First, Rest...>::Tuple(const Tuple<First, Rest...>& tuple):
	Tuple<Rest...>(tuple), _value(tuple._value)
{
}

template <class First, class... Rest>
Tuple<First, Rest...>::Tuple(First&& first, Rest&&... rest):
	Tuple<Rest...>(Move(rest)...), _value(Move(first))
{
}

template <class First, class... Rest>
Tuple<First, Rest...>::Tuple(Tuple<First, Rest...>&& tuple):
	Tuple<Rest...>(Move< Tuple<Rest...> >(tuple)), _value(Move(tuple._value))
{
}

template <class First, class... Rest>
Tuple<First, Rest...>::Tuple(void)
{
}

template <class First, class... Rest>
const Tuple<First, Rest...>& Tuple<First, Rest...>::operator=(const Tuple<First, Rest...>& rhs)
{
	_value = rhs._value;
	Tuple<Rest...>::operator=(rhs);
	return *this;
}

template <class First, class... Rest>
const Tuple<First, Rest...>& Tuple<First, Rest...>::operator=(Tuple<First, Rest...>&& rhs)
{
	_value = Move(rhs._value);
	Tuple<Rest...>::operator=(Move< Tuple<Rest...> >(rhs));
	return *this;
}

template <class First, class... Rest>
bool Tuple<First, Rest...>::operator==(const Tuple<First, Rest...>& rhs) const
{
	return _value == rhs._value && Tuple<Rest...>::operator==(rhs);
}

template <class First, class... Rest>
bool Tuple<First, Rest...>::operator!=(const Tuple<First, Rest...>& rhs) const
{
	return _value != rhs._value || Tuple<Rest...>::operator!=(rhs);
}

template <class First, class... Rest>
void Tuple<First, Rest...>::set(const First& first, Rest&&... rest)
{
	_value = first;
	Tuple<Rest...>::set(rest...);
}

template <class First, class... Rest>
void Tuple<First, Rest...>::moveSet(First&& first, Rest&&... rest)
{
	_value = Move(first);
	Tuple<Rest...>::moveSet(Move(rest)...);
}


// Tuple operation functions
template <class First, class... Rest>
class TupleElement< 0, Tuple<First, Rest...> >
{
public:
	typedef Tuple<First, Rest...> TupleType;
	typedef First ValueType;
};

template <unsigned int index, class First, class... Rest>
class TupleElement< index, Tuple<First, Rest...> > : public TupleElement< index - 1, Tuple<Rest...> >
{
};

template <unsigned int index, class... Args>
const typename TupleElement< index, Tuple<Args...> >::ValueType& TupleGet(const Tuple<Args...>& tuple)
{
	return ((const TupleElement< index, Tuple<Args...> >::TupleType&)tuple)._value;
}

template <unsigned int index, class... Args>
typename TupleElement< index, Tuple<Args...> >::ValueType& TupleGet(Tuple<Args...>& tuple)
{
	return ((TupleElement< index, Tuple<Args...> >::TupleType&)tuple)._value;
}
