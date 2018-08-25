config_options = {}
configs = {}

function AddConfigOption(config)
	table.insert(config_options, { config, config })
end

if _OPTIONS["gen-clang"] then
	if _ACTION == "vs2017" then
		configs =
		{
			"Debug", "Release", "Analyze",
			"Debug_Clang", "Release_Clang",
			"Profile", "Profile_Clang",
			"Optimized_Debug", "Optimized_Debug_Clang"
		}
	else
		configs =
		{
			"Debug", "Release",
			"Debug_Clang", "Release_Clang",
			"Profile", "Profile_Clang",
			"Optimized_Debug", "Optimized_Debug_Clang"
		}
	end
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
