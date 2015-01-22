external "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/shared")
	end

	kind "StaticLib"
	language "C++"
