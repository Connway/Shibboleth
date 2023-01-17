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

#pragma once

#include "Shibboleth_RuntimeVarManagerFwd.h"
#include "Shibboleth_DynamicLoaderFwd.h"
#include "Shibboleth_ReflectionUtils.h"
#include "Shibboleth_JobPoolFwd.h"
#include "Shibboleth_String.h"

NS_GAFF
	class JSON;

	//template <class Allocator>
	//class FileWatcherManager;
NS_END

NS_SHIBBOLETH

//using FileWatcherManager = Gaff::FileWatcherManager<ProxyAllocator>;

class ReflectionManager;
class IFileSystem;
class LogManager;
class IManager;

class IApp
{
public:
	IApp(void) {}
	virtual ~IApp(void) {}

	virtual const IManager* getManager(Gaff::Hash64 name) const = 0;
	virtual IManager* getManager(Gaff::Hash64 name) = 0;

	virtual IFileSystem& getFileSystem(void) = 0;
	virtual const Gaff::JSON& getConfigs(void) const = 0;
	virtual Gaff::JSON& getConfigs(void) = 0;
	virtual U8String getProjectDirectory(void) const = 0;

	virtual const ReflectionManager& getReflectionManager(void) const = 0;
	virtual ReflectionManager& getReflectionManager(void) = 0;
	virtual LogManager& getLogManager(void) = 0;
	virtual JobPool& getJobPool(void) = 0;

	//virtual FileWatcherManager& getFileWatcherManager(void) = 0;
	virtual DynamicLoader& getDynamicLoader(void) = 0;

	virtual bool isQuitting(void) const = 0;
	virtual void quit(void) = 0;

#ifdef SHIB_RUNTIME_VAR_ENABLED
	virtual RuntimeVarManager& getRuntimeVarManager(void) = 0;
#endif
};

NS_END
