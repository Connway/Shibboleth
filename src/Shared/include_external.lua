external "Shared"
	if _ACTION then
		location ("../../project/" .. _ACTION)
	end

	kind "StaticLib"
	language "C++"

external "Memory"
	if _ACTION then
		location ("../../project/" .. _ACTION)
	end

	kind "SharedLib"
	language "C++"
