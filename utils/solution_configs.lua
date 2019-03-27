config_options = {}
configs = {}

function AddConfigOption(config)
	table.insert(config_options, { config, config })
end

if _ACTION == "vs2017" then
	if _OPTIONS["gen-clang"] then
		configs =
		{
			"Debug", "Release",
			"Debug_Clang", "Release_Clang",
			"Profile", "Profile_Clang",
			"Optimized_Debug", "Optimized_Debug_Clang"
			-- add static versions here
		}
	else
		configs =
		{
			"Debug", "Release",
			"Profile", "Optimized_Debug",
			"Static_Debug_D3D11",
			-- "Static_Debug_Vulkan",
			"Static_Release_D3D11",
			-- "Static_Release_Vulkan"
		}
	end
else
	configs =
	{
		"Debug", "Release",
		"Debug_Clang", "Release_Clang",
		"Profile", "Profile_Clang",
		"Optimized_Debug", "Optimized_Debug_Clang"
		-- add static versions here
	}
end

table.foreachi(configs, AddConfigOption)
