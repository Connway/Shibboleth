local GenerateProject = function()
	local base_dir = GetModulesDirectory("Physics")

	project "Physics"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "StaticLib"
		language "C++"

		files { base_dir .. "**.h", base_dir .. "**.cpp", base_dir .. "**.inl" }
		defines { "SHIB_STATIC", "PX_PHYSX_STATIC_LIB" }

		ModuleGen("Physics")
		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			base_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			base_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/PhysX/physx/include",
			base_dir .. "../../Dependencies/PhysX/pxshared/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			base_dir .. "../../Modules/MainLoop/include",
			base_dir .. "../../Modules/Resource/include",
			base_dir .. "../../Modules/Debug/include",
			base_dir .. "../../Modules/ECS/include"
		}

	project "PhysicsModule"
		if _ACTION then
			location(GetModulesLocation())
		end

		kind "SharedLib"
		language "C++"

		files { base_dir .. "Shibboleth_PhysicsModule.cpp" }
		defines { "PX_PHYSX_STATIC_LIB" }

		ModuleCopy()

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Physics")
		NewDeleteLinkFix()
		SetupConfigMap()

		local deps =
		{
			"Gleam",
			"mpack",

			"LowLevel",
			"LowLevelAABB",
			"LowLevelDynamics",
			"PhysXCharacterKinematic",
			"PhysXFoundation",
			"PhysXExtensions",
			"PhysXVehicle",
			"PhysXCooking",
			"PhysXCommon",
			"PhysXPvdSDK",
			"PhysXTask",
			"PhysX",
			"SceneQuery",
			"SimulationController",

			"MainLoop",
			"Resource",
			"Debug",
			"ECS"
		}

		dependson(deps)
		links(deps)
end

local LinkDependencies = function()
	local deps = ModuleDependencies("Physics")
	table.insert(deps, "Gleam")
	table.insert(deps, "mpack")

	table.insert(deps, "LowLevel")
	table.insert(deps, "LowLevelAABB")
	table.insert(deps, "LowLevelDynamics")
	table.insert(deps, "PhysXCharacterKinematic")
	table.insert(deps, "PhysXFoundation")
	table.insert(deps, "PhysXExtensions")
	table.insert(deps, "PhysXVehicle")
	table.insert(deps, "PhysXCooking")
	table.insert(deps, "PhysXCommon")
	table.insert(deps, "PhysXPvdSDK")
	table.insert(deps, "PhysXTask")
	table.insert(deps, "PhysX")
	table.insert(deps, "SceneQuery")
	table.insert(deps, "SimulationController")

	table.insert(deps, "MainLoop")
	table.insert(deps, "Resource")
	table.insert(deps, "Debug")
	table.insert(deps, "ECS")

	dependson(deps)
	links(deps)
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
