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

#include "Shibboleth_SerializeablePtr.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::ISerializeablePtr)
	.serialize(Shibboleth::ISerializeablePtr::Load, Shibboleth::ISerializeablePtr::Save)
SHIB_REFLECTION_DEFINE_END(Shibboleth::ISerializeablePtr)

NS_SHIBBOLETH

bool ISerializeablePtr::Load(const ISerializeReader& reader, ISerializeablePtr& out)
{
	const char8_t* class_name = nullptr;

	{
		const auto guard = reader.enterElementGuard(u8"class");
		GAFF_ASSERT(reader.isNull() || reader.isString());

		if (reader.isNull()) {
			return true;
		}

		class_name = reader.readString();
	}

	if (class_name && class_name[0]) {
		const auto guard = reader.enterElementGuard(u8"data");

		const Refl::IReflectionDefinition* const ref_def = GetApp().getReflectionManager().getReflection(Gaff::FNV1aHash64String(class_name));

		if (ref_def) {
			out._ptr.reset(ref_def->CREATET(Refl::IReflectionObject, out._allocator));
			ref_def->load(reader, out._ptr.get());

		} else {
			// $TODO: Log error.
		}
	}

	reader.freeString(class_name);
	return true;
}

void ISerializeablePtr::Save(ISerializeWriter& writer, const ISerializeablePtr& value)
{
	if (value._ptr) {
		const Refl::IReflectionDefinition& ref_def = value._ptr->getReflectionDefinition();

		writer.writeString(u8"class", ref_def.getReflectionInstance().getName());
		writer.writeKey(u8"data");
		ref_def.save(writer, value._ptr.get());

	} else {
		writer.writeNull(u8"class");
	}
}

ISerializeablePtr::ISerializeablePtr(UniquePtr<Refl::IReflectionObject>&& ptr, const ProxyAllocator& allocator):
	_ptr(std::move(ptr)), _allocator(allocator)
{
}

ISerializeablePtr::ISerializeablePtr(Refl::IReflectionObject* ptr, const ProxyAllocator& allocator):
	_ptr(ptr), _allocator(allocator)
{
}

ISerializeablePtr::ISerializeablePtr(const ProxyAllocator& allocator):
	_ptr(nullptr), _allocator(allocator)
{
}

NS_END
