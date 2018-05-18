config_options = {}
configs = {}

function AddConfigOption(config)
	table.insert(config_options, { config, config })
end

if _OPTIONS["gen-clang"] and _ACTION == "vs2017" then
	configs =
	{
		"Debug", "Release",
		"Debug_Clang", "Release_Clang",
		"Analyze",
		"Profile", "Profile_Clang",
		"Optimized_Debug", "Optimized_Debug_Clang"
	}
elseif _ACTION == "vs2017" then
	configs =
	{
		"Debug", "Release", "Analyze",
		"Profile", "Optimized_Debug"
	}
else
	configs =
	{
		"Debug", "Release",
		"Profile", "Optimized_Debug"
	}
end

table.foreachi(configs, AddConfigOption)
