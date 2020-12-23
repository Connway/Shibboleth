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

#include <Shibboleth_Reflection.h>
#include <Shibboleth_ECSEntity.h>
#include <Esprit_IProcess.h>
#include <Gleam_Vec3.h>

NS_SHIBBOLETH

class GameTimeManager;
class InputManager;
class ECSManager;

class FlyCameraProcess final : public Esprit::IProcess, public Gaff::IReflectionObject
{
public:
	bool init(const Esprit::StateMachine& owner) override;
	void update(const Esprit::StateMachine& /*owner*/, Esprit::VariableSet::Instance& variables) override;

private:
	GameTimeManager* _time_mgr = nullptr;
	InputManager* _input_mgr = nullptr;
	ECSManager* _ecs_mgr = nullptr;

	Gleam::Vec3 _angular_speed = Gleam::Vec3{ 1.0f, 1.0f, 1.0f };
	float _linear_speed = 1.0f;

	int32_t _entity_id_index = -1;

	int32_t _camera_vert_alias_index = -1;
	int32_t _horiz_alias_index = -1;
	int32_t _vert_alias_index = -1;

	int32_t _pitch_index = -1;
	int32_t _yaw_index = -1;

	SHIB_REFLECTION_CLASS_DECLARE(FlyCameraProcess);
};

NS_END

SHIB_REFLECTION_DECLARE(FlyCameraProcess)
