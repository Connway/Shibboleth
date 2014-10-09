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

#include "Shibboleth_MessageBroadcaster.h"
#include "Shibboleth_DynamicLoader.h"
#include "Shibboleth_Logger.h"

NS_SHIBBOLETH

class IManager;

class IApp
{
public:
	template <class T>
	const T& getManagerT(const AHashString& name) const
	{
		return *(T*)getManager(name);
	}

	template <class T>
	T& getManagerT(const AHashString& name)
	{
		return *(T*)getManager(name);
	}

	template <class T>
	const T& getManagerT(const AString& name) const
	{
		return *(T*)getManager(name);
	}

	template <class T>
	T& getManagerT(const AString& name)
	{
		return *(T*)getManager(name);
	}

	template <class T>
	const T& getManagerT(const char* name) const
	{
		return *(T*)getManager(name);
	}

	template <class T>
	T& getManagerT(const char* name)
	{
		return *(T*)getManager(name);
	}

	IApp(void) {}
	virtual ~IApp(void) {}

	virtual const IManager* getManager(const AHashString& name) const = 0;
	virtual const IManager* getManager(const AString& name) const = 0;
	virtual const IManager* getManager(const char* name) const = 0;
	virtual IManager* getManager(const AHashString& name) = 0;
	virtual IManager* getManager(const AString& name) = 0;
	virtual IManager* getManager(const char* name) = 0;

	virtual void switchState(unsigned int state) = 0;

	virtual DynamicLoader::ModulePtr loadModule(const char* filename, const char* name) = 0;
	virtual Allocator& getAllocator(void) = 0;
	virtual void addTask(Gaff::TaskPtr<ProxyAllocator>& task) = 0;
	virtual LogManager::FileLockPair& getGameLogFile(void) = 0;
	virtual size_t getSeed(void) const = 0;
	virtual void quit(void) = 0;
};

NS_END
