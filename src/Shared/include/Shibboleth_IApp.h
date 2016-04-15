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

#include "Shibboleth_MessageBroadcaster.h"
#include "Shibboleth_DynamicLoader.h"
#include "Shibboleth_LogManager.h"
#include "Shibboleth_HashString.h"
#include "Shibboleth_JobPool.h"
#include "Shibboleth_HashMap.h"
#include "Shibboleth_Array.h"

NS_SHIBBOLETH

class IFileSystem;
class IManager;

class IApp
{
public:
	template <class T>
	const T& getManagerT(const AHashString& name) const
	{
		return *reinterpret_cast<T*>(getManager(name));
	}

	template <class T>
	T& getManagerT(const AHashString& name)
	{
		return *reinterpret_cast<T*>(getManager(name));
	}

	template <class T>
	const T& getManagerT(const AString& name) const
	{
		return *reinterpret_cast<T*>(getManager(name));
	}

	template <class T>
	T& getManagerT(const AString& name)
	{
		return *reinterpret_cast<T*>(getManager(name));
	}

	template <class T>
	const T& getManagerT(const char* name) const
	{
		return *reinterpret_cast<T*>(getManager(name));
	}

	template <class T>
	T& getManagerT(const char* name)
	{
		return *reinterpret_cast<T*>(getManager(name));
	}

	template <class T>
	const T& getManagerT(void) const
	{
		return *reinterpret_cast<T*>(getManager(T::GetFriendlyName()));
	}

	template <class T>
	T& getManagerT(void)
	{
		return *reinterpret_cast<T*>(getManager(T::GetFriendlyName()));
	}

	IApp(void) {}
	virtual ~IApp(void) {}

	virtual const IManager* getManager(const AHashString& name) const = 0;
	virtual const IManager* getManager(const AString& name) const = 0;
	virtual const IManager* getManager(const char* name) const = 0;
	virtual IManager* getManager(const AHashString& name) = 0;
	virtual IManager* getManager(const AString& name) = 0;
	virtual IManager* getManager(const char* name) = 0;

	virtual MessageBroadcaster& getBroadcaster(void) = 0;

	virtual const Array<unsigned int>& getStateTransitions(unsigned int state_id) = 0;
	virtual unsigned int getStateID(const char* name) = 0;
	virtual void switchState(unsigned int state_id) = 0;

	virtual IFileSystem* getFileSystem(void) = 0;
	virtual const HashMap<AHashString, AString>& getCmdLine(void) const = 0;
	virtual HashMap<AHashString, AString>& getCmdLine(void) = 0;

	virtual JobPool& getJobPool(void) = 0;

	virtual void getWorkerThreadIDs(Array<unsigned int>& out) const = 0;
	virtual void helpUntilNoJobs(void) = 0;
	virtual void doAJob(void) = 0;

	virtual const char* getLogFileName(void) const = 0;
	virtual LogManager& getLogManager(void) = 0;

	virtual DynamicLoader::ModulePtr loadModule(const char* filename, const char* name) = 0;
	virtual size_t getSeed(void) const = 0;

	virtual bool isQuitting(void) const = 0;
	virtual void quit(void) = 0;
};

NS_END
