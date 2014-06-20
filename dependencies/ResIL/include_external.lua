external "ResIL"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "SharedLib"
	language "C"

external "ResILU"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/dependencies")
	end

	kind "SharedLib"
	language "C"
