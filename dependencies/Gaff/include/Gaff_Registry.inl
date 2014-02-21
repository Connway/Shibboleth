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

template <class Allocator>
Registry<Allocator>::Registry(const Allocator& allocator):
	_registry(allocator)
{
}

template <class Allocator>
Registry<Allocator>::~Registry(void)
{
}

template <class Allocator>
void Registry<Allocator>::addItem(IRefCounted* manager, const AString<Allocator>& name)
{
	_registry.insert(name, manager);
}

template <class Allocator>
void Registry<Allocator>::addItem(IRefCounted* manager, const char* name)
{
	_registry.insert(name, manager);
}

template <class Allocator>
void Registry<Allocator>::addItem(const IRefPtr& manager, const AString<Allocator>& name)
{
	_registry.insert(name, manager);
}

template <class Allocator>
void Registry<Allocator>::addItem(const IRefPtr& manager, const char* name)
{
	_registry.insert(name, manager);
}

template <class Allocator>
void Registry<Allocator>::removeItem(const AString<Allocator>& name)
{
	_registry.erase(name);
}

template <class Allocator>
void Registry<Allocator>::removeItem(const char* name)
{
	_registry.erase(name);
}

template <class Allocator>
template <class T>
const T* Registry<Allocator>::getItem(const AString<Allocator>& name) const
{
	return (T*)getItemHelper(name);
}

template <class Allocator>
template <class T>
const T* Registry<Allocator>::getItem(const char* name) const
{
	return (T*)getItemHelper(name);
}

template <class Allocator>
template <class T>
T* Registry<Allocator>::getItem(const AString<Allocator>& name)
{
	return (T*)getItemHelper(name);
}

template <class Allocator>
template <class T>
T* Registry<Allocator>::getItem(const char* name)
{
	return (T*)getItemHelper(name);
}

template <class Allocator>
void* Registry<Allocator>::getItemHelper(const AString<Allocator>& name)
{
	// TODO: Convert to iterators
	int index = _registry.indexOf(name);

	if (index == -1) {
		return nullptr;
	}

	return _registry.valueAt(index);
}
