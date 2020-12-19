local TestProcess =
{
	entity_id_index = -1,
	rot_speed_index = -1,
	game_time = nil,
	ecs_mgr = nil
}

function TestProcess:init(owner)
	local variables = owner:getVariables()
	self.entity_id_index = variables:getVariableIndex("entity_id", Esprit.VariableSet.VariableType.Integer)
	self.rot_speed_index = variables:getVariableIndex("rot_speed", Esprit.VariableSet.VariableType.Float)

	self.game_time = GetManager(GameTimeManager).game_time
	self.ecs_mgr = GetManager(ECSManager)

	return true
end

function TestProcess:update(owner, var_inst)
	local variables = owner:getVariables()
	local entity_id = variables:getInteger(var_inst, self.entity_id_index)
	local rot_speed = variables:getFloat(var_inst, self.rot_speed_index)

	-- local pos = Position.Get(self.ecs_mgr, entity_id)
	-- pos.value.x = pos.value.x + 0.5 * self.game_time.delta
	-- Position.Set(self.ecs_mgr, entity_id, pos)

	local rot = Rotation.Get(self.ecs_mgr, entity_id)
	rot.value.y = rot.value.y + rot_speed * self.game_time.delta
	Rotation.Set(self.ecs_mgr, entity_id, rot)
end

return TestProcess
