if not _OPTIONS["physx"] then

	dofile("findOpenCL.lua")

	local project_generators = os.matchfiles("**/premake4.lua")

	group "Dependencies/Bullet"

	for i = 1, table.getn(project_generators) do
		dofile(project_generators[i])
		defines { "_CRT_SECURE_NO_WARNINGS" }
	end

	group "Dependencies"

end
