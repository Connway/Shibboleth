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

#include "Shibboleth_Object.h"
#include <Shibboleth_IResource.h>

NS_SHIBBOLETH

class PrefabResource final : public IResource
{
public:
	~PrefabResource(void);

	const Object* getPrefab(void) const;

private:
	Object* _prefab = nullptr;

	bool loadJSON(IFile* file, const ProxyAllocator& allocator);
	bool loadMPack(IFile* file, const ProxyAllocator& allocator);
	void loadPrefab(IFile* file);

	SHIB_REFLECTION_CLASS_DECLARE(PrefabResource);
};

using PrefabResourcePtr = Gaff::RefPtr<PrefabResource>;

NS_END

SHIB_REFLECTION_DECLARE(PrefabResource)
