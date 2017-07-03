if _ACTION then
	location ("../project/" .. _ACTION)
end

if _OPTIONS["gen-clang"] and (_ACTION == "vs2015" or _ACTION == "vs2017") then
	configurations
	{
		"Debug", "Release",
		"Debug_Clang", "Release_Clang",
		"Analyze",
		"Profile", "Profile_Clang",
		"Optimized_Debug", "Optimized_Debug_Clang"
	}
elseif _ACTION == "vs2015" or _ACTION == "vs2017" then
	configurations
	{
		"Debug", "Release", "Analyze",
		"Profile", "Optimized_Debug"
	}
else
	configurations
	{
		"Debug", "Release",
		"Profile", "Optimized_Debug"
	}
end
