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

#pragma once

#include "Gaff_RefPtr.h"
#include "Gaff_HashMap.h"

NS_GAFF

template <class Allocator>
class Registry
{
public:
	Registry(const Allocator& allocator = Allocator());
	~Registry(void);

	void addItem(IRefCounted* manager, const AString<Allocator>& name);
	void addItem(IRefCounted* manager, const char* name);
	void addItem(const IRefPtr& manager, const AString<Allocator>& name);
	void addItem(const IRefPtr& manager, const char* name);

	void removeItem(const AString<Allocator>& name);
	void removeItem(const char* name);

	template <class T>
	const T* getItem(const AString<Allocator>& name) const;

	template <class T>
	const T* getItem(const char* name) const;

	template <class T>
	T* getItem(const AString<Allocator>& name);

	template <class T>
	T* getItem(const char* name);

private:
	HashMap<AString<Allocator>, IRefPtr, Allocator> _registry;

	void* getItemHelper(const AString<Allocator>& name);
};

#include "Gaff_Registry.inl"

NS_END
