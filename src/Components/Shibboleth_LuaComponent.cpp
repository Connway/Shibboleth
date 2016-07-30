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

#include "Shibboleth_LuaComponent.h"
#include <Shibboleth_IResourceManager.h>
#include <Shibboleth_ISchemaManager.h>
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(push)
	#pragma warning(disable: 4100 4244 4267 4800)
#endif

#include <LuaState.h>

#ifdef PLATFORM_WINDOWS
	#pragma warning(pop)
#endif

NS_SHIBBOLETH

COMP_REF_DEF_SAVE(LuaComponent, g_ref_def);
REF_IMPL_REQ(LuaComponent);

SHIB_REF_IMPL(LuaComponent)
.addBaseClassInterfaceOnly<LuaComponent>()
.ADD_BASE_CLASS_INTERFACE_ONLY(ILuaComponent)
;

LuaComponent::LuaComponent(void)
{
}

LuaComponent::~LuaComponent(void)
{
}

const Gaff::JSON& LuaComponent::getSchema(void) const
{
	static const Gaff::JSON& schema = GetApp().getManagerT<ISchemaManager>().getSchema("ScriptComponent.schema");
	return schema;
}

bool LuaComponent::load(const Gaff::JSON& json)
{
	g_ref_def.read(json, this);

	IResourceManager& res_mgr = GetApp().getManagerT<IResourceManager>();

	Gaff::JSON script_file = json["Script File"];

	_script_res = res_mgr.requestResource(script_file.getString());
	_script_res.getResourcePtr()->addCallback(Gaff::Bind(this, &LuaComponent::scriptLoaded));

	return true;
}

void LuaComponent::allComponentsLoaded(void)
{
}

void LuaComponent::scriptLoaded(ResourceContainerBase*)
{
	if (_script_res.getResourcePtr()->isLoaded()) {
		cacheFunctions();
		// populate any global script data if necessary
	}
}

void LuaComponent::cacheFunctions(void)
{
	// cache script callback functions here
}

NS_END
