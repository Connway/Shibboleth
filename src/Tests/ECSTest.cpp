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

#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_ECSManager.h>
#include <Shibboleth_ECSQuery.h>
#include <Shibboleth_App.h>
#include <catch_amalgamated.hpp>

TEST_CASE("shibboleth_ecs_archetype_hash")
{
	Shibboleth::ECSArchetype archetype1;
	Shibboleth::ECSArchetype archetype2;

	REQUIRE(archetype1.add<Shibboleth::Position>());
	REQUIRE(archetype1.add<Shibboleth::Rotation>());
	REQUIRE(archetype1.add<Shibboleth::Scale>());

	REQUIRE(archetype2.add<Shibboleth::Scale>());
	REQUIRE(archetype2.add<Shibboleth::Rotation>());
	REQUIRE(archetype2.add<Shibboleth::Position>());

	REQUIRE(archetype1.getHash() == archetype2.getHash());
}

TEST_CASE("shibboleth_ecs_create_destroy_entity")
{
	Shibboleth::ECSManager ecs_mgr;

	Shibboleth::ECSArchetype archetype;
	REQUIRE(archetype.add<Shibboleth::Position>());
	REQUIRE(archetype.add<Shibboleth::Rotation>());
	REQUIRE(archetype.add<Shibboleth::Scale>());

	REQUIRE(archetype.finalize());

	const Gaff::Hash64 archetype_hash = archetype.getHash();

	ecs_mgr.addArchetype(std::move(archetype));

	const Shibboleth::EntityID id1 = ecs_mgr.createEntity(archetype_hash);
	const Shibboleth::EntityID id2 = ecs_mgr.createEntity(archetype_hash);

	REQUIRE(id1 == 0);
	REQUIRE(id2 == 1);

	Shibboleth::Position::Set(ecs_mgr, id1, Shibboleth::Position(Gleam::Vec3(0.0f, 1.0f, 2.0f)));
	Shibboleth::Rotation::Set(ecs_mgr, id1, Shibboleth::Rotation(Gleam::Vec3(0.0f, 0.0f, 0.0f)));
	Shibboleth::Scale::Set(ecs_mgr, id1, Shibboleth::Scale(Gleam::Vec3(3.0f)));

	Shibboleth::Position::Set(ecs_mgr, id2, Shibboleth::Position(Gleam::Vec3(5.0f, 4.0f, 3.0f)));
	Shibboleth::Rotation::Set(ecs_mgr, id2, Shibboleth::Rotation(Gleam::Vec3(1.0f, 2.0f, 3.0f)));
	Shibboleth::Scale::Set(ecs_mgr, id2, Shibboleth::Scale(Gleam::Vec3(5.0f)));

	REQUIRE(Shibboleth::Position::Get(ecs_mgr, id1).value == Gleam::Vec3(0.0f, 1.0f, 2.0f));
	REQUIRE(Shibboleth::Rotation::Get(ecs_mgr, id1).value == Gleam::Vec3(0.0f, 0.0f, 0.0f));
	REQUIRE(Shibboleth::Scale::Get(ecs_mgr, id1).value == Gleam::Vec3(3.0f));

	REQUIRE(Shibboleth::Position::Get(ecs_mgr, id2).value == Gleam::Vec3(5.0f, 4.0f, 3.0f));
	REQUIRE(Shibboleth::Rotation::Get(ecs_mgr, id2).value == Gleam::Vec3(1.0f, 2.0f, 3.0f));
	REQUIRE(Shibboleth::Scale::Get(ecs_mgr, id2).value == Gleam::Vec3(5.0f));

	ecs_mgr.destroyEntity(id1);
	ecs_mgr.destroyEntity(id2);
}

TEST_CASE("shibboleth_ecs_add_remove_component")
{
	Shibboleth::ECSManager ecs_mgr;

	Shibboleth::ECSArchetype archetype;
	REQUIRE(archetype.add<Shibboleth::Position>());
	REQUIRE(archetype.finalize());

	const Gaff::Hash64 archetype_hash = archetype.getHash();
	ecs_mgr.addArchetype(std::move(archetype));

	const Shibboleth::EntityID id = ecs_mgr.createEntity(archetype_hash);
	REQUIRE(id == 0);

	Shibboleth::Position::Set(ecs_mgr, id, Shibboleth::Position((Gleam::Vec3(0.0f, 1.0f, 2.0f))));
	REQUIRE(Shibboleth::Position::Get(ecs_mgr, id).value == Gleam::Vec3(0.0f, 1.0f, 2.0f));

	ecs_mgr.addComponents<Shibboleth::Rotation, Shibboleth::Scale>(id);
	Shibboleth::Rotation::Set(ecs_mgr, id, Shibboleth::Rotation(Gleam::Vec3(0.0f, 0.0f, 0.0f)));
	Shibboleth::Scale::Set(ecs_mgr, id, Shibboleth::Scale(Gleam::Vec3(3.0f)));

	REQUIRE(Shibboleth::Position::Get(ecs_mgr, id).value == Gleam::Vec3(0.0f, 1.0f, 2.0f));
	REQUIRE(Shibboleth::Rotation::Get(ecs_mgr, id).value == Gleam::Vec3(0.0f, 0.0f, 0.0f));
	REQUIRE(Shibboleth::Scale::Get(ecs_mgr, id).value == Gleam::Vec3(3.0f));

	ecs_mgr.removeComponents<Shibboleth::Position, Shibboleth::Scale>(id);
	REQUIRE(Shibboleth::Rotation::Get(ecs_mgr, id).value == Gleam::Vec3(0.0f, 0.0f, 0.0f));

	ecs_mgr.destroyEntity(id);
}

TEST_CASE("shibboleth_ecs_add_remove_shared_component")
{
	Shibboleth::ECSManager ecs_mgr;

	Shibboleth::ECSArchetype archetype;
	REQUIRE(archetype.add<Shibboleth::Position>());
	REQUIRE(archetype.addShared<Shibboleth::Rotation>());
	REQUIRE(archetype.finalize());

	ecs_mgr.addArchetype(std::move(archetype));

	const Gaff::Hash64 archetype_hash = archetype.getHash();
	Shibboleth::Rotation::SetShared(ecs_mgr, archetype_hash, Shibboleth::Rotation(Gleam::Vec3(0.0f, 0.0f, 0.0f)));

	const Shibboleth::EntityID id = ecs_mgr.createEntity(archetype_hash);
	REQUIRE(id == 0);

	Shibboleth::Position::Set(ecs_mgr, id, Shibboleth::Position(Gleam::Vec3(0.0f, 1.0f, 2.0f)));

	REQUIRE(Shibboleth::Position::Get(ecs_mgr, id).value == Gleam::Vec3(0.0f, 1.0f, 2.0f));
	REQUIRE(Shibboleth::Rotation::GetShared(ecs_mgr, archetype_hash).value == Gleam::Vec3(0.0f, 0.0f, 0.0f));

	ecs_mgr.addSharedComponents<Shibboleth::Scale>(id);

	Shibboleth::Rotation* rotation = ecs_mgr.getComponentShared<Shibboleth::Rotation>(id);
	Shibboleth::Scale* scale = ecs_mgr.getComponentShared<Shibboleth::Scale>(id);

	REQUIRE(rotation);
	REQUIRE(scale);

	scale->value = Gleam::Vec3(3.0f);

	REQUIRE(Shibboleth::Position::Get(ecs_mgr, id).value == Gleam::Vec3(0.0f, 1.0f, 2.0f));
	REQUIRE(rotation->value == Gleam::Vec3(0.0f, 0.0f, 0.0f));
	REQUIRE(scale->value == Gleam::Vec3(3.0f));

	ecs_mgr.destroyEntity(id);
}

TEST_CASE("shibboleth_ecs_query")
{
	Shibboleth::ECSManager ecs_mgr;

	Shibboleth::ECSArchetype archetype;
	REQUIRE(archetype.add<Shibboleth::Position>());
	REQUIRE(archetype.addShared<Shibboleth::Scale>());
	REQUIRE(archetype.finalize());

	ecs_mgr.addArchetype(std::move(archetype));

	const Gaff::Hash64 archetype_hash = archetype.getHash();
	const Shibboleth::EntityID id = ecs_mgr.createEntity(archetype_hash);
	REQUIRE(id == 0);

	Shibboleth::Position::Set(ecs_mgr, id, Shibboleth::Position(Gleam::Vec3(0.0f, 1.0f, 2.0f)));
	Shibboleth::Scale::SetShared(ecs_mgr, archetype_hash, Shibboleth::Scale(Gleam::Vec3(3.0f)));

	Shibboleth::Vector<const Shibboleth::Scale*> scale_output;
	Shibboleth::Vector<Shibboleth::ECSQueryResult> position_output;
	Shibboleth::ECSQuery query;

	query.addShared<Shibboleth::Scale>(scale_output);
	query.add<Shibboleth::Position>(position_output);

	ecs_mgr.registerQuery(std::move(query));

	REQUIRE(position_output.size() == 1);
	REQUIRE(scale_output.size() == 1);

	REQUIRE(ecs_mgr.getNumEntities(position_output[0]) == 1);

	Shibboleth::Position::Set(ecs_mgr, position_output[0], 0, Shibboleth::Position(Gleam::Vec3(0.0f, 1.0f, 2.0f)));

	REQUIRE(Shibboleth::Position::Get(ecs_mgr, position_output[0], 0).value == Gleam::Vec3(0.0f, 1.0f, 2.0f));
	REQUIRE(scale_output[0]->value == Gleam::Vec3(3.0f));
}
