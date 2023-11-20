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

#include "Shibboleth_ResourcePtr.h"
#include "Shibboleth_ResourceManager.h"
#include <Shibboleth_AppUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::IResourcePtr)
	.serialize(Shibboleth::IResourcePtr::Load, Shibboleth::IResourcePtr::Save)
SHIB_REFLECTION_DEFINE_END(Shibboleth::IResourcePtr)


NS_SHIBBOLETH

bool IResourcePtr::Load(const ISerializeReader& reader, IResourcePtr& out)
{
	if (!reader.isString()) {
		return false;
	}

	const char8_t* const res_path = reader.readString();
	out = GetManagerTFast<ResourceManager>().requestResource(res_path);
	reader.freeString(res_path);

	return out._resource;
}

void IResourcePtr::Save(ISerializeWriter& writer, const IResourcePtr& value)
{
	writer.writeString(value._resource->getFilePath().getBuffer());
}

IResourcePtr::IResourcePtr(const IResourcePtr& res_ptr, const Refl::IReflectionDefinition& ref_def):
	_ref_def(&ref_def), _resource(res_ptr._resource)
{
	GAFF_ASSERT(_resource->getReflectionDefinition().hasInterface(ref_def.getReflectionInstance().getNameHash()));
}

IResourcePtr::IResourcePtr(IResourcePtr&& res_ptr, const Refl::IReflectionDefinition& ref_def):
	_ref_def(&ref_def), _resource(std::move(res_ptr._resource))
{
	GAFF_ASSERT(_resource->getReflectionDefinition().hasInterface(ref_def.getReflectionInstance().getNameHash()));
}

IResourcePtr::IResourcePtr(IResource* resource, const Refl::IReflectionDefinition& ref_def):
	_ref_def(&ref_def), _resource(resource)
{
	GAFF_ASSERT(resource->getReflectionDefinition().hasInterface(ref_def.getReflectionInstance().getNameHash()));
}

IResourcePtr::IResourcePtr(const Refl::IReflectionDefinition& ref_def):
	_ref_def(&ref_def), _resource(nullptr)
{
}

IResourcePtr& IResourcePtr::operator=(const IResourcePtr& rhs)
{
	GAFF_ASSERT(!rhs._resource || !_ref_def || rhs._resource->getReflectionDefinition().hasInterface(_ref_def->getReflectionInstance().getNameHash()));

	_resource = rhs._resource;

	if (!_ref_def && _resource) {
		_ref_def = &_resource->getReflectionDefinition();
	}

	return *this;
}

NS_END
