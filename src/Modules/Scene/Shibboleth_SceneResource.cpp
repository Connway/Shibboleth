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

#include "Shibboleth_SceneResource.h"
#include <Shibboleth_ResourceAttributesCommon.h>
// #include <Shibboleth_ResourceManager.h>
// #include <Shibboleth_ResourceLogging.h>
// #include <Shibboleth_Utilities.h>
// #include <Gaff_Function.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::SceneResource)
	.classAttrs(
		Shibboleth::ResourceExtensionAttribute(u8".scene.bin"),
		Shibboleth::ResourceExtensionAttribute(u8".scene")
	)

	.template base<Shibboleth::IResource>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::SceneResource)

NS_SHIBBOLETH

SceneResource::SceneResource(void)
{
}

SceneResource::~SceneResource(void)
{
}

void SceneResource::load(const ISerializeReader& reader, uintptr_t thread_id_int)
{

}

void SceneResource::save(ISerializeWriter& writer)
{

}

NS_END
