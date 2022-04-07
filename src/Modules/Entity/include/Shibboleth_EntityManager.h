/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

#include "Shibboleth_Entity.h"
#include <Shibboleth_IManager.h>
//#include <EAThread/eathread_mutex.h>

NS_SHIBBOLETH

class EntityManager final : public IManager
{
public:
	template <class T>
	T* createEntity(void)
	{
		static_assert(std::is_base_of<Entity, T>::value, "T must be derived from Entity.");

		Entity* const entity = createEntity(Refl::Reflection<T>::GetReflectionDefinition());
		return (entity) ? static_cast<T*>(entity) : nullptr;
	}

	//~EntityManager(void);

	//bool initAllModulesLoaded(void) override;

	Entity* createEntity(const Refl::IReflectionDefinition& ref_def);
	Entity* createEntity(void);

private:

	SHIB_REFLECTION_CLASS_DECLARE(EntityManager);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::EntityManager)
