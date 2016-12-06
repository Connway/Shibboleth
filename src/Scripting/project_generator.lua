group "Scripting"

project "Scripting"
	if _ACTION then
		location ("../../project/" .. _ACTION .. "/scripting")
	end

	kind "StaticLib"
	language "C++"
	rtti "On"

	files { "**.h", "**.cpp", "**.inl" }
	removefiles { "Shibboleth_ScriptModule.cpp" }

	includedirs
	{
		"include",
		"../Memory/include",
		"../Shared/include",
		"../../dependencies/EASTL/include",
		"../../dependencies/chaiscript",
		"../../frameworks/Gaff/include"
	}

	filter { "action:vs*" }
		buildoptions { "/bigobj" }

	filter {}
