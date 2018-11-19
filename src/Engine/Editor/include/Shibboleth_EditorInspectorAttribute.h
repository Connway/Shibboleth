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

#include <Shibboleth_Reflection.h>

NS_SHIBBOLETH

class IEditorInspectorAttribute : public Gaff::IAttribute
{
public:
	IEditorInspectorAttribute(void) {}
	virtual ~IEditorInspectorAttribute(void) {}

	virtual const Gaff::IReflection& getType(void) const = 0;
};

template <class T>
class EditorInspectorAttribute final : public IEditorInspectorAttribute
{
public:
	IAttribute* clone(void) const override
	{
		IAllocator& allocator = GetAllocator();
		return SHIB_ALLOCT_POOL(EditorInspectorAttribute, allocator.getPoolIndex("Reflection"), allocator);
	}

	const Gaff::IReflection& getType(void) const override
	{
		return Reflection<T>::GetInstance();
	}

private:
	SHIB_TEMPLATE_REFLECTION_CLASS_DECLARE(EditorInspectorAttribute, T);
};

SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_BEGIN(EditorInspectorAttribute, T)
	.BASE(Gaff::IAttribute)
SHIB_TEMPLATE_REFLECTION_CLASS_DEFINE_END(EditorInspectorAttribute, T)

NS_END

SHIB_TEMPLATE_REFLECTION_DECLARE(EditorInspectorAttribute, T)
SHIB_TEMPLATE_REFLECTION_DEFINE(EditorInspectorAttribute, T)
