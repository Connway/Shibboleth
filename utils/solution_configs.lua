config_options = {}
configs = {}

function AddConfigOption(config)
	table.insert(config_options, { config, config })
end

configs =
{
	"Debug",
	"Release",
	"Profile",
	"Optimized_Debug",

	-- Add static versions here.
	"Static_Debug_D3D11",
	-- "Static_Debug_D3D12",
	-- "Static_Debug_Vulkan",
	"Static_Release_D3D11",
	-- "Static_Release_D3D12",
	-- "Static_Release_Vulkan"
}

if ((_ACTION == "vs2019" or _ACTION == "vs2022") and _OPTIONS["gen-clang"]) then
	table.insert(configs, "Debug_Clang")
	table.insert(configs, "Release_Clang")
	table.insert(configs, "Profile_Clang")
	table.insert(configs, "Optimized_Debug_Clang")

	table.insert(configs, "Static_Debug_D3D11_Clang")
	table.insert(configs, "Static_Release_D3D11_Clang")
	-- table.insert(configs, "Static_Debug_D3D12_Clang")
	-- table.insert(configs, "Static_Release_D3D12_Clang")
	-- table.insert(configs, "Static_Debug_Vulkan_Clang")
	-- table.insert(configs, "Static_Release_Vulkan_Clang")
end

table.foreachi(configs, AddConfigOption)
