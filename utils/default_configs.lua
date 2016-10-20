if _OPTIONS["gen-clang"] and os.get() == "windows" then
	configurations
	{
		"Debug", "Release", "Analyze",
		"Debug_Clang", "Release_Clang",
		"Profile", "Profile_Clang"
	}
else
	configurations
	{
		"Debug", "Release", "Analyze",
		"Profile"
	}
end
