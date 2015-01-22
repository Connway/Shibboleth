external "Memory"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/memory")
	end

	kind "SharedLib"
	language "C++"
