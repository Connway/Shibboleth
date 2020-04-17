local TestProcess =
{
	test_vec = glm.vec3.new(1, 2, 3),
	test_var = 0,
	test_index = -1
}

function TestProcess:init(owner)
	print("Test Process - Init")

	self.test_index = owner:getVariables():getVariableIndex("test", Esprit.VariableSet.VariableType.Integer)

	print("Test Index: ", self.test_index)

	return true
end

function TestProcess:update(owner, variables)
	print("Test Process - Update ", self.test_var)
	self.test_var = self.test_var + 1

	if self.test_var == 1 then
		self.test_vec = glm.vec4.new(4, 3, 2, 1)
	elseif self.test_var == 2 then
		self.test_vec.w = 20
	end
end

return TestProcess
