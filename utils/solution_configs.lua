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

table.foreachi(configs, AddConfigOption)
