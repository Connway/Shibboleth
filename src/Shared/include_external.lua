external "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION)
	end

	kind "StaticLib"
	language "C++"
