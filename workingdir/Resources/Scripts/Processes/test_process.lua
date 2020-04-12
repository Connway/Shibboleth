local TestProcess =
{
	test_vec = Vec3.new(1, 2, 3),
	test_var = 0
}

function TestProcess:init(owner)
	print("Test Process - Init")
	return true
end

function TestProcess:update(owner, variables)
	print("Test Process - Update ", self.test_var)
	self.test_var = self.test_var + 1

	if self.test_var == 1 then
		self.test_vec = Vec4.new(4, 3, 2, 1)
	elseif self.test_var == 2 then
		self.test_vec.w = 20
	end
end

return TestProcess
