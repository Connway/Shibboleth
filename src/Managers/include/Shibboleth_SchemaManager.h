/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_HashString.h>
#include <Shibboleth_HashMap.h>
#include <Shibboleth_IManager.h>
#include <Gaff_JSON.h>

NS_SHIBBOLETH

class IFile;

class SchemaManager : public IManager
{
public:
	SchemaManager(void);
	~SchemaManager(void);

	void* rawRequestInterface(unsigned int class_id) const;
	const char* getName(void) const;
	void allManagersCreated(void);

	INLINE const Gaff::JSON& getSchema(const char* schema_file) const;

private:
	HashMap<AHashString, Gaff::JSON> _schema_map;

	bool addSchema(const char* file_name, IFile* file);

	SHIB_REF_DEF(SchemaManager);
};

NS_END