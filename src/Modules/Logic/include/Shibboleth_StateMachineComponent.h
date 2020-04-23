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

#include "Shibboleth_StateMachineResource.h"
#include <Shibboleth_ResourceComponent.h>

NS_SHIBBOLETH

struct StateMachineView final
{
	StateMachineResource* resource;
	Esprit::StateMachine::Instance* instance;
};

NS_END

NS_HASHABLE
	GAFF_CLASS_HASHABLE(StateMachineView)
NS_END

NS_SHIBBOLETH

class StateMachine final : public ECSComponentBaseBoth<StateMachine, StateMachineView>
{
public:
	static void CopyInternal(const void* old_begin, int32_t old_index, void* new_begin, int32_t new_index);
	static void SetInternal(void* component, int32_t entity_index, const StateMachine& value);
	static void SetInternal(void* component, int32_t entity_index, const StateMachineView& value);
	static StateMachineView GetInternal(const void* component, int32_t entity_index);

	static void Constructor(EntityID, void* component, int32_t entity_index);
	static void Destructor(EntityID, void* component, int32_t entity_index);

	static bool Load(ECSManager& ecs_mgr, EntityID id, const Gaff::ISerializeReader& reader);
	static bool Load(const Gaff::ISerializeReader& reader, StateMachine& out);

	StateMachine& operator=(const StateMachine& rhs);
	StateMachine& operator=(StateMachine&& rhs);

	StateMachineResourcePtr resource;
	UniquePtr<Esprit::StateMachine::Instance> instance;
};

NS_END

SHIB_REFLECTION_DECLARE(StateMachine)
