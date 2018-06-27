/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Shibboleth_ECSManager.h"

SHIB_REFLECTION_DEFINE(ECSManager)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE_BEGIN(ECSManager)
	.BASE(IManager)
	.ctor<>()
SHIB_REFLECTION_CLASS_DEFINE_END(ECSManager)

bool ECSManager::init(void)
{
	GetApp().getReflectionManager().registerTypeBucket(Gaff::FNV1aHash64Const("IECSComponent"));

	// Load all archetypes from config directory
	// Create component buckets
	return true;
}

void ECSManager::addArchetype(const ECSArchetype& archetype, const char* name)
{
	_archetypes.emplace(Gaff::FNV1aHash64String(name), archetype);
}

void ECSManager::addArchetype(ECSArchetype&& archetype, const char* name)
{
	_archetypes.emplace(Gaff::FNV1aHash64String(name), std::move(archetype));
}

const ECSArchetype& ECSManager::getArchetype(Gaff::Hash64 name) const
{
	const VectorMap<Gaff::Hash64, ECSArchetype>::const_iterator it = _archetypes.find(name);
	GAFF_ASSERT(it && it != _archetypes.end());
	return it->second;
}

const ECSArchetype& ECSManager::getArchetype(const char* name) const
{
	return getArchetype(Gaff::FNV1aHash64String(name));
}

NS_END
