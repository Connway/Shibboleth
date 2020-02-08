/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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

#include "Shibboleth_ECSArchetypeResource.h"
#include <Shibboleth_SerializeReaderWrapper.h>

NS_SHIBBOLETH

class ECSLayerResource final : public IResource
{
public:
	static constexpr bool Creatable = true;

	ECSLayerResource(void);
	~ECSLayerResource(void);

private:
	Vector<ECSArchetypeResourcePtr> _archetypes;
	Vector<ArchetypeReferencePtr> _archetype_refs;
	SerializeReaderWrapper _reader_wrapper;
	ResourceCallbackID _callback_id;

	bool loadOverrides(
		const Gaff::ISerializeReader& reader,
		ECSManager& ecs_mgr,
		const ECSArchetype& base_archetype,
		Gaff::Hash32 layer_name,
		Gaff::Hash64& outArchetype
	);

	void archetypeLoaded(const Vector<IResource*>&);
	void loadLayer(IFile* file);

	SHIB_REFLECTION_CLASS_DECLARE_NEW(ECSLayerResource);
};

using ECSLayerResourcePtr = Gaff::RefPtr<ECSLayerResource>;

NS_END

SHIB_REFLECTION_DECLARE_NEW(ECSLayerResource)
