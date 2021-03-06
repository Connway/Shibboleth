/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Shibboleth_DebugAttributes.h"
#include "Shibboleth_IDebugManager.h"
#include <Shibboleth_IAllocator.h>
#include <Shibboleth_Memory.h>

SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(DebugMenuClassAttribute, Gaff::IAttribute)
SHIB_REFLECTION_DEFINE_WITH_BASE_NO_INHERITANCE(DebugMenuItemAttribute, Gaff::IAttribute)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(DebugMenuClassAttribute)
SHIB_REFLECTION_CLASS_DEFINE(DebugMenuItemAttribute)

Gaff::IAttribute* DebugMenuClassAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(DebugMenuClassAttribute, allocator.getPoolIndex("Reflection"), allocator);
}

void DebugMenuClassAttribute::instantiated(void* object, const Gaff::IReflectionDefinition& ref_def)
{
	// If we are the DebugManager, then just call register on ourselves.
	// Otherwise, get the DebugManager from the manager registry.
	IDebugManager* const debug_mgr = (ref_def.getReflectionInstance().getHash() == CLASS_HASH(DebugManager)) ?
		reinterpret_cast<IDebugManager*>(object) :
		&GetApp().GETMANAGERT(IDebugManager, DebugManager);

	debug_mgr->registerDebugMenuItems(object, ref_def);
}



DebugMenuItemAttribute::DebugMenuItemAttribute(const char* path):
	_path(path, ProxyAllocator("Reflection"))
{
}

const U8String& DebugMenuItemAttribute::getPath(void) const
{
	return _path;
}

Gaff::IAttribute* DebugMenuItemAttribute::clone(void) const
{
	IAllocator& allocator = GetAllocator();
	return SHIB_ALLOCT_POOL(DebugMenuItemAttribute, allocator.getPoolIndex("Reflection"), allocator, _path.data());
}

void DebugMenuItemAttribute::finish(Gaff::IReflectionDefinition& ref_def)
{
	if (!ref_def.getClassAttr<DebugMenuClassAttribute>()) {
		DebugMenuClassAttribute attr;
		ref_def.addClassAttr(attr);
	}
}

NS_END
