/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Shibboleth_ComponentManager.h"
#include "Shibboleth_Component.h"
#include <Shibboleth_LogManager.h>

SHIB_REFLECTION_DEFINE(ComponentManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ComponentManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ComponentManager)

ComponentManager::ComponentManager(void)
{
	GetApp().registerTypeBucket(Gaff::FNV1aHash64Const("Component"));
	GetApp().getLogManager().addChannel("Entity", "EntityLog");
}

void ComponentManager::allModulesLoaded(void)
{
	const Vector<Gaff::Hash64>* component_types = GetApp().getTypeBucket(Gaff::FNV1aHash64Const("Component"));
	GAFF_REF(component_types);
}

Component* ComponentManager::createComponent(Gaff::Hash64 class_hash)
{
	GAFF_REF(class_hash);
	return nullptr;
}

NS_END
