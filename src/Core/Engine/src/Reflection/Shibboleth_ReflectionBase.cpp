/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Reflection/Shibboleth_ReflectionBase.h"

namespace
{
	static Refl::IReflection* g_enum_head = nullptr;
	static Refl::IReflection* g_attr_head = nullptr;
	static Refl::IReflection* g_head = nullptr;

	static void AddToChain(Refl::IReflection*& head, Refl::IReflection* reflection)
	{
		reflection->next = head;
		head = reflection;
	}

	static void InitChain(Refl::IReflection* head)
	{
		while (head) {
			head->init();
			head = head->next;
		}
	}
}

NS_REFLECTION

void AddToAttributeReflectionChain(IReflection* reflection)
{
	AddToChain(g_attr_head, reflection);
}

IReflection* GetAttributeReflectionChainHead(void)
{
	return g_attr_head;
}

void AddToEnumReflectionChain(IReflection* reflection)
{
	AddToChain(g_enum_head, reflection);
}

IReflection* GetEnumReflectionChainHead(void)
{
	return g_enum_head;
}

void AddToReflectionChain(IReflection* reflection)
{
	AddToChain(g_head, reflection);
}

IReflection* GetReflectionChainHead(void)
{
	return g_head;
}

void InitAttributeReflection(void)
{
	InitChain(g_attr_head);
}

void InitClassReflection(void)
{
	InitChain(g_head);
}

void InitEnumReflection(void)
{
	InitChain(g_enum_head);
}

NS_END
