/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_DynamicLoaderFwd.h"
#include "Shibboleth_JobPoolFwd.h"
#include "Shibboleth_Reflection.h"

NS_GAFF
	class IEnumReflectionDefinition;
	class IReflectionDefinition;
	class JSON;
NS_END

NS_SHIBBOLETH

class IFileSystem;
class Broadcaster;
class LogManager;
class IManager;

class IApp
{
public:
	template <class T>
	const T& getManagerTFast(void) const
	{
		return *static_cast<T*>(getManager(Reflection<T>::GetHash()));
	}

	template <class T>
	T& getManagerTFast(void)
	{
		return *static_cast<T*>(getManager(Reflection<T>::GetHash()));
	}

	template <class T>
	const T& getManagerT(Gaff::Hash64 manager_name, Gaff::Hash64 interface_name) const
	{
		const IManager* const manager = getManager(manager_name);
		return *Gaff::InterfaceCast<T>(*manager, interface_name);
	}

	template <class T>
	T& getManagerT(Gaff::Hash64 manager_name, Gaff::Hash64 interface_name)
	{
		IManager* const manager = getManager(manager_name);
		return *Gaff::InterfaceCast<T>(*manager, interface_name);
	}

	template <class T>
	const T& getManagerT(void) const
	{
		const IManager* const manager = getManager(Reflection<T>::GetHash());
		return *Gaff::ReflectionCast<T>(*manager);
	}

	template <class T>
	T& getManagerT(void)
	{
		IManager* const manager = getManager(Reflection<T>::GetHash());
		return *Gaff::ReflectionCast<T>(*manager);
	}

	IApp(void) {}
	virtual ~IApp(void) {}

	virtual const IManager* getManager(Gaff::Hash64 name) const = 0;
	virtual IManager* getManager(Gaff::Hash64 name) = 0;

	virtual IFileSystem& getFileSystem(void) = 0;
	virtual const Gaff::JSON& getConfigs(void) const = 0;
	virtual const U8String& getProjectDirectory(void) const = 0;

	virtual const ReflectionManager& getReflectionManager(void) const = 0;
	virtual ReflectionManager& getReflectionManager(void) = 0;
	virtual Broadcaster& getBroadcaster(void) = 0;
	virtual LogManager& getLogManager(void) = 0;
	virtual JobPool& getJobPool(void) = 0;

	virtual DynamicLoader& getDynamicLoader(void) = 0;

	virtual bool isQuitting(void) const = 0;
	virtual void quit(void) = 0;

	virtual bool inEditorMode(void) const = 0;
};

NS_END

#define GETMANAGERT(base_mgr_class, mgr_class) getManagerT<base_mgr_class>(Gaff::FNV1aHash64Const(#mgr_class), Gaff::FNV1aHash64Const(#base_mgr_class))
