local GenerateProject = function()
	local source_dir = GetModulesSourceDirectory("Physics")
	local base_dir = GetModulesDirectory("Physics")

	GenProject "Physics"
		location(GetModulesLocation())

		language "C++"

		files { source_dir .. "**.h", source_dir .. "**.cpp", source_dir .. "**.inl" }
		defines { "SHIB_STATIC", "PX_PHYSX_STATIC_LIB" }

		SetupConfigMap()

		flags { "FatalWarnings" }

		includedirs
		{
			source_dir .. "include",
			base_dir .. "../../Engine/Memory/include",
			source_dir .. "../../Engine/Engine/include",
			base_dir .. "../../Dependencies/EASTL/include",
			base_dir .. "../../Dependencies/glm",
			base_dir .. "../../Dependencies/mpack",
			base_dir .. "../../Dependencies/PhysX/physx/include",
			base_dir .. "../../Dependencies/PhysX/pxshared/include",
			base_dir .. "../../Dependencies/rapidjson",
			base_dir .. "../../Dependencies/tracy",
			base_dir .. "../../Frameworks/Gaff/include",
			base_dir .. "../../Frameworks/Gleam/include",
			source_dir .. "../../Modules/MainLoop/include",
			source_dir .. "../../Modules/Resource/include",
			source_dir .. "../../Modules/DevDebug/include",
			source_dir .. "../../Modules/ECS/include"
		}

	GenProject("PhysicsModule", "SharedLib")
		location(GetModulesLocation())

		language "C++"

		files { source_dir .. "Shibboleth_PhysicsModule.cpp" }
		defines { "PX_PHYSX_STATIC_LIB" }

		flags { "FatalWarnings" }

		ModuleIncludesAndLinks("Physics")
		NewDeleteLinkFix()
		SetupConfigMap()
		ModuleCopy()

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
			"DevDebug",
			"ECS"
		}

		dependson(deps)
		links(deps)

	filter { "configurations:*Debug* or *Profile*" }
		dependson({ "TracyClient" })
		links({ "TracyClient" })

	filter {}
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
	table.insert(deps, "DevDebug")
	table.insert(deps, "ECS")

	dependson(deps)
	links(deps)

	filter { "configurations:*Debug* or *Profile*" }
		dependson({ "TracyClient" })
		links({ "TracyClient" })

	filter {}
end

return { GenerateProject = GenerateProject, LinkDependencies = LinkDependencies }
