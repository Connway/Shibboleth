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

#include "Gaff_IReflectionDefinition.h"
#include "Gaff_VectorMap.h"
#include "Gaff_String.h"

NS_GAFF

template <class T, class Allocator>
class ReflectionDefinition : public IReflectionDefinition
{
public:
	//explicit ReflectionDefinition(const char* name, const Allocator& allocator = Allocator());
	//ReflectionDefinition(const Allocator& allocator = Allocator());
	//~ReflectionDefinition(void) = default;

	void load(ISerializeReader& reader, void* object) const override;
	void save(ISerializeWriter& writer, const void* object) const override;
	void load(ISerializeReader& reader, T& object) const;
	void save(ISerializeWriter& writer, const T& object) const;

	//const void* getInterface(ReflectionHash class_id, const void* object) const override;
	//void* getInterface(ReflectionHash class_id, void* object) const override;

	ReflectionDefinition& setAllocator(const Allocator& allocator);

private:


	//HashMap<HashString32<Allocator>, ValueContainerPtr, Allocator> _value_containers;
	//Array<Pair< ReflectionHash, FunctionBinder<void*, const void*> >, Allocator> _base_ids;
	//Array<IOnCompleteFunctor*, Allocator> _callback_references;

	U8String<Allocator> _name;
	Allocator _allocator;
	unsigned int _base_classes_remaining;
	bool _defined;
};

NS_END

#include "Gaff_ReflectionDefinition.inl"
