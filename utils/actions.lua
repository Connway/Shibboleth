newaction
{
	trigger = "gen_vs",
	description = "Generate supported Visual Studio solutions. (VS2015/VS2017)",
	execute = function()
		os.execute("premake5 vs2015")
		os.execute("premake5 vs2017")
	end
}
