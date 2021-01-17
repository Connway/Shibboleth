/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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
#include <Gleam_Quaternion.h>
#include <Gleam_Matrix3x3.h>

SHIB_REFLECTION_DEFINE_BEGIN(FlyCameraProcess)
	.BASE(Esprit::IProcess)
	.ctor<>()

	.var("angular_speed", &FlyCameraProcess::_angular_speed)
	.var("linear_speed", &FlyCameraProcess::_linear_speed)
SHIB_REFLECTION_DEFINE_END(FlyCameraProcess)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(FlyCameraProcess);

bool FlyCameraProcess::init(const Esprit::StateMachine& owner)
{
	const Esprit::VariableSet& variables = owner.getVariables();
	_entity_id_index = variables.getVariableIndex(HashStringView32<>("entity_id"), Esprit::VariableSet::VariableType::Integer);

	if (_entity_id_index < 0) {
		// $TODO: Log error.
		return false;
	}

	IApp& app = GetApp();

	_time_mgr = &app.getManagerTFast<GameTimeManager>();
	_input_mgr = &app.getManagerTFast<InputManager>();
	_ecs_mgr = &app.getManagerTFast<ECSManager>();

	_camera_vert_alias_index = _input_mgr->getAliasIndex("Camera_Vertical");
	_horiz_alias_index = _input_mgr->getAliasIndex("Horizontal");
	_vert_alias_index = _input_mgr->getAliasIndex("Vertical");

	_pitch_index = _input_mgr->getAliasIndex("Camera_Pitch");
	_yaw_index = _input_mgr->getAliasIndex("Camera_Yaw");

	if (_camera_vert_alias_index < 0) {
		// $TODO: Log error.
		return false;
	}

	if (_horiz_alias_index < 0) {
		// $TODO: Log error.
		return false;
	}

	if (_vert_alias_index < 0) {
		// $TODO: Log error.
		return false;
	}

	if (_pitch_index < 0) {
		// $TODO: Log error.
		return false;
	}

	if (_yaw_index < 0) {
		// $TODO: Log error.
		return false;
	}

	return true;
}

void FlyCameraProcess::update(const Esprit::StateMachine& /*owner*/, Esprit::VariableSet::Instance& variables)
{
	if (_camera_vert_alias_index < 0) {
		return;
	}

	if (_horiz_alias_index < 0) {
		return;
	}

	if (_vert_alias_index < 0) {
		return;
	}

	if (_entity_id_index < 0) {
		return;
	}

	if (_pitch_index < 0) {
		return;
	}

	if (_yaw_index < 0) {
		return;
	}

	const EntityID entity_id = static_cast<EntityID>(variables.integers[_entity_id_index]);
	
	if (entity_id == EntityID_None) {
		// $TODO: Log error periodic.
		return;
	}

	if (!_ecs_mgr->hasComponent<PlayerOwner>(entity_id)) {
		// $TODO: Log error periodic.
		return;
	}

	if (!_ecs_mgr->hasComponent<Position>(entity_id)) {
		// $TODO: Log error periodic.
		return;
	}

	if (!_ecs_mgr->hasComponent<Rotation>(entity_id)) {
		// $TODO: Log error periodic.
		return;
	}

	PlayerOwner player_owner = PlayerOwner::Get(*_ecs_mgr, entity_id);
	Position position = Position::Get(*_ecs_mgr, entity_id);
	Rotation rotation = Rotation::Get(*_ecs_mgr, entity_id);

	Gleam::Vec3 local_move_dir = glm::zero<Gleam::Vec3>();

	local_move_dir.x = _input_mgr->getAliasValue(_horiz_alias_index, player_owner.value);
	local_move_dir.y = _input_mgr->getAliasValue(_camera_vert_alias_index, player_owner.value);
	local_move_dir.z = _input_mgr->getAliasValue(_vert_alias_index, player_owner.value);

	if (local_move_dir != glm::zero<Gleam::Vec3>()) {
		local_move_dir = glm::normalize(local_move_dir);
	}

	// Update rotation.
	rotation.value += _angular_speed * Gleam::Vec3{
		_input_mgr->getAliasValue(_pitch_index, player_owner.value),
		_input_mgr->getAliasValue(_yaw_index, player_owner.value),
		0.0f
	};

	// Update position.
	const Gleam::Mat3x3 rot = glm::mat3_cast(Gleam::Quat(rotation.value * Gaff::TurnsToRad));
	Gleam::Vec3 camera_dir = rot * local_move_dir;

	if (camera_dir != glm::zero<Gleam::Vec3>()) {
		camera_dir = glm::normalize(camera_dir);
	}

	const float dt = _time_mgr->getRealTime().getDeltaFloat();
	position.value += camera_dir * _linear_speed * dt;

	Position::Set(*_ecs_mgr, entity_id, position);
	Rotation::Set(*_ecs_mgr, entity_id, rotation);
}

NS_END
