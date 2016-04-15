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

#include "Shibboleth_SchemaManager.h"
#include <Shibboleth_IFileSystem.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

NS_SHIBBOLETH

REF_IMPL_REQ(SchemaManager);
SHIB_REF_IMPL(SchemaManager)
.addBaseClassInterfaceOnly<SchemaManager>()
;

const char* SchemaManager::GetFriendlyName(void)
{
	return "Schema Manager";
}

SchemaManager::SchemaManager(void)
{
}

SchemaManager::~SchemaManager(void)
{
}

const char* SchemaManager::getName(void) const
{
	return GetFriendlyName();
}

void SchemaManager::allManagersCreated(void)
{
	auto callback = Gaff::Bind(this, &SchemaManager::addSchema);
	GetApp().getFileSystem()->forEachFile("Resources/Schemas", callback);
}

const Gaff::JSON& SchemaManager::getSchema(const char* schema_file) const
{
	return _schema_map[schema_file];
}

bool SchemaManager::addSchema(const char* file_name, IFile* file)
{
	if (!_schema_map[file_name].parse(file->getBuffer())) {
		// log error
		GetApp().quit();
		return true;
	}

	return false;
}

NS_END
