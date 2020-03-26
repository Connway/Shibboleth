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

#include "Shibboleth_FlyCameraProcess.h"
#include <Shibboleth_ECSComponentCommon.h>
#include <Shibboleth_InputManager.h>
#include <Shibboleth_ECSManager.h>
#include <Shibboleth_GameTime.h>
#include <Esprit_StateMachine.h>

SHIB_REFLECTION_DEFINE_BEGIN(FlyCameraProcess)
	.BASE(Esprit::IProcess)
	.ctor<>()
SHIB_REFLECTION_DEFINE_END(FlyCameraProcess)

NS_SHIBBOLETH

bool FlyCameraProcess::init(const Esprit::StateMachine& owner)
{
	const Esprit::VariableSet* const variables = owner.getVariables();

	if (!variables) {
		// $TODO: Log error.
		return false;
	}

	_entity_id_index = variables->getVariableIndex(Esprit::HashStringTemp32<>("entity_id"), Esprit::VariableSet::VariableType::Integer);

	if (_entity_id_index < 0) {
		return false;
	}

	IApp& app = GetApp();

	_time_mgr = &app.getManagerTFast<GameTimeManager>();
	_input_mgr = &app.getManagerTFast<InputManager>();
	_ecs_mgr = &app.getManagerTFast<ECSManager>();

	_horiz_alias_index = _input_mgr->getAliasIndex("Horizontal");
	_vert_alias_index = _input_mgr->getAliasIndex("Vertical");

	if (_horiz_alias_index < 0) {
		// $TODO: Log error.
		return false;
	}

	if (_vert_alias_index < 0) {
		// $TODO: Log error.
		return false;
	}

	return true;
}

void FlyCameraProcess::update(const Esprit::StateMachine& /*owner*/, Esprit::VariableSet::VariableInstance* instance_data)
{
	if (_horiz_alias_index < 0) {
		return;
	}

	if (_vert_alias_index < 0) {
		return;
	}

	if (_entity_id_index < 0) {
		return;
	}

	if (!instance_data) {
		return;
	}

	const EntityID entity_id = static_cast<EntityID>(instance_data->integers[_entity_id_index]);
	
	if (entity_id == EntityID_None) {
		// $TODO: Log error periodic.
		return;
	}

	if (!_ecs_mgr->hasComponent<PlayerOwner>(entity_id)) {
		// $TODO: Log error periodic.
	}

	if (!_ecs_mgr->hasComponent<Position>(entity_id)) {
		// $TODO: Log error periodic.
	}

	if (!_ecs_mgr->hasComponent<Rotation>(entity_id)) {
		// $TODO: Log error periodic.
	}

	PlayerOwner player_owner = PlayerOwner::Get(*_ecs_mgr, entity_id);
	Position position = Position::Get(*_ecs_mgr, entity_id);
	Rotation rotation = Rotation::Get(*_ecs_mgr, entity_id);

	glm::vec3 local_move_dir = glm::zero<glm::vec3>();

	local_move_dir.x = _input_mgr->getAliasValue(_horiz_alias_index, player_owner.value);
	//local_move_dir.y = _input_mgr->getAliasValue(_vert_alias_index, player_owner.value);
	local_move_dir.z = _input_mgr->getAliasValue(_vert_alias_index, player_owner.value);

	local_move_dir = glm::normalize(local_move_dir);

	// update rotation

	const float dt = _time_mgr->getRealTime().getDeltaFloat();
	position.value += local_move_dir * dt;

	Position::Set(*_ecs_mgr, entity_id, position);
	Rotation::Set(*_ecs_mgr, entity_id, rotation);
}

NS_END
