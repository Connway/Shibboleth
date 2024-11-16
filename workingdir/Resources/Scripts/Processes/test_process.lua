local TestProcess =
{
	entity_index = -1,
	rot_speed_index = -1,
	game_time = nil,
	ecs_mgr = nil
}

function TestProcess:init(owner)
	local variables = owner:getVariables()
	self.entity_index = variables:getVariableIndex("entity", Esprit.VariableSet.VariableType.Reference)
	self.rot_speed_index = variables:getVariableIndex("rot_speed", Esprit.VariableSet.VariableType.Float)

	self.game_time = GetManager(Shibboleth.GameTimeManager).game_time
	self.ecs_mgr = GetManager(Shibboleth.ECSManager)

	return true
end

function TestProcess:update(owner, var_inst)
	local variables = owner:getVariables()
	local entity = variables:getReference(var_inst, self.entity_id, Shibboleth.Entity)
	local rot_speed = variables:getFloat(var_inst, self.rot_speed_index)

	if entity == nil then
		return
	end

	-- local pos = Shibboleth.Position.Get(self.ecs_mgr, entity_id)
	-- pos.value.x = pos.value.x + 0.5 * self.game_time.delta
	-- Shibboleth.Position.Set(self.ecs_mgr, entity_id, pos)

	local rot = Shibboleth.Rotation.Get(self.ecs_mgr, entity_id)
	rot.value.y = rot.value.y + rot_speed * self.game_time.delta
	Shibboleth.Rotation.Set(self.ecs_mgr, entity_id, rot)
end

return TestProcess
