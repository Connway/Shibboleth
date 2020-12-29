(def TestProcess
	@{
		:entity-id-index -1
		:rot-speed-index -1
		:game-time nil
		:ecs-mgr nil

		:init (fn [self owner]
		(
			(def variables (:getVariables owner))
			(put self :entity-id-index (:getVariableIndex variables "entity_id" Esprit/VariableSet/VariableType/Integer))
			(put self :rot-speed-index (:getVariableIndex variables "rot_speed" Esprit/VariableSet/VariableType/Float))

			(put self :game-time ((GetManager GameTimeManager) :game_time))
			(put self :ecs-mgr (GetManager ECSManager))
		))

		:update (fn [self owner var-inst]
		(
			(def variables (:getVariables owner))
			(def entity-id (:getInteger variables (self :var-inst) (self :entity-id-index)))
			(def rot-speed (:getFloat variables (self :var-inst) (self :rot-speed-index)))

			(def rot ((Rotation/Get (self :ecs-mgr) entity-id)))

			# rot.value.y = rot.value.y + rot-speed * self.game-time.delta
			(put (rot :value) :y (+ ((rot :value) :y) (* rot-speed ((self :game-time) :delta))))
			(Rotation/Set (self :ecs-mgr) entity-id rot)
		))
	}
)

#local TestProcess =
#{
#	entity_id_index = -1,
#	rot_speed_index = -1,
#	game_time = nil,
#	ecs_mgr = nil
#}

#function TestProcess:init(owner)
#	local variables = owner:getVariables()
#	self.entity_id_index = variables:getVariableIndex("entity_id", Esprit.VariableSet.VariableType.Integer)
#	self.rot_speed_index = variables:getVariableIndex("rot_speed", Esprit.VariableSet.VariableType.Float)

#	self.game_time = GetManager(GameTimeManager).game_time
#	self.ecs_mgr = GetManager(ECSManager)

#	return true
#end

#function TestProcess:update(owner, var_inst)
#	local variables = owner:getVariables()
#	local entity_id = variables:getInteger(var_inst, self.entity_id_index)
#	local rot_speed = variables:getFloat(var_inst, self.rot_speed_index)

#	-- local pos = Position.Get(self.ecs_mgr, entity_id)
#	-- pos.value.x = pos.value.x + 0.5 * self.game_time.delta
#	-- Position.Set(self.ecs_mgr, entity_id, pos)

#	local rot = Rotation.Get(self.ecs_mgr, entity_id)
#	rot.value.y = rot.value.y + rot_speed * self.game_time.delta
#	Rotation.Set(self.ecs_mgr, entity_id, rot)
#end

return TestProcess
