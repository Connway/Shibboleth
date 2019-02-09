/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_ECSManager.h>
#include <Shibboleth_App.h>

Shibboleth::App g_app;

TEST_CASE("shibboleth_ecs_create_destroy_entity")
{
	Shibboleth::AllocatorThreadInit();

	//g_app.init(0, nullptr);
	Shibboleth::SetApp(g_app);

	Gaff::InitAttributeReflection();
	Gaff::InitClassReflection();

	Shibboleth::ECSManager ecs_mgr;

	Shibboleth::ECSArchetype archetype;
	REQUIRE(archetype.add<Shibboleth::Position>());
	REQUIRE(archetype.add<Shibboleth::Rotation>());
	REQUIRE(archetype.add<Shibboleth::Scale>());

	REQUIRE(archetype.finalize());

	const Gaff::Hash64 archetype_hash = archetype.getHash();

	ecs_mgr.addArchetype(std::move(archetype), "test_archetype");

	const Shibboleth::EntityID id1 = ecs_mgr.createEntity(archetype_hash);
	const Shibboleth::EntityID id2 = ecs_mgr.createEntity(archetype_hash);

	REQUIRE_EQ(id1, 0);
	REQUIRE_EQ(id2, 1);

	Shibboleth::Position::Set(ecs_mgr, id1, glm::vec3(0.0f, 1.0f, 2.0f));
	Shibboleth::Rotation::Set(ecs_mgr, id1, glm::quat(1.0f, glm::vec3(0.0f)));
	Shibboleth::Scale::Set(ecs_mgr, id1, glm::vec3(3.0f));

	Shibboleth::Position::Set(ecs_mgr, id2, glm::vec3(5.0f, 4.0f, 3.0f));
	Shibboleth::Rotation::Set(ecs_mgr, id2, glm::quat(2.0f, glm::vec3(4.0f)));
	Shibboleth::Scale::Set(ecs_mgr, id2, glm::vec3(5.0f));

	REQUIRE_EQ(Shibboleth::Position::Get(ecs_mgr, id1), glm::vec3(0.0f, 1.0f, 2.0f));
	REQUIRE_EQ(Shibboleth::Rotation::Get(ecs_mgr, id1), glm::quat(1.0f, glm::vec3(0.0f)));
	REQUIRE_EQ(Shibboleth::Scale::Get(ecs_mgr, id1), glm::vec3(3.0f));

	REQUIRE_EQ(Shibboleth::Position::Get(ecs_mgr, id2), glm::vec3(5.0f, 4.0f, 3.0f));
	REQUIRE_EQ(Shibboleth::Rotation::Get(ecs_mgr, id2), glm::quat(2.0f, glm::vec3(4.0f)));
	REQUIRE_EQ(Shibboleth::Scale::Get(ecs_mgr, id2), glm::vec3(5.0f));

	ecs_mgr.destroyEntity(id1);
	ecs_mgr.destroyEntity(id2);
}
