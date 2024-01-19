/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#include "Shibboleth_Utilities.h"
#include "Shibboleth_IApp.h"

NS_SHIBBOLETH

template <class T>
static T& GetManagerTFast(void)
{
	static_assert(std::is_base_of<IManager, T>::value, "Type T does not derive from IManager.");
	return *static_cast<T*>(GetApp().getManager(Refl::Reflection<T>::GetNameHash()));
}

template <class T>
static T& GetManagerT(Gaff::Hash64 manager_name, Gaff::Hash64 interface_name)
{
	IManager* const manager = GetApp().getManager(manager_name);
	GAFF_ASSERT(manager);

	return *Refl::ReflectionCast<T>(manager, interface_name);
}

template <class T>
static T& GetManagerT(void)
{
	static_assert(std::is_base_of<IManager, T>::value, "Type T does not derive from IManager.");

	IManager* const manager = GetApp().getManager(Refl::Reflection<T>::GetNameHash());
	GAFF_ASSERT(manager);

	return *Refl::ReflectionCast<T>(manager);
}

NS_END

#define GETMANAGERT(base_mgr_class, mgr_class) GetManagerT<base_mgr_class>(Gaff::FNV1aHash64Const(#mgr_class), Gaff::FNV1aHash64Const(#base_mgr_class))
