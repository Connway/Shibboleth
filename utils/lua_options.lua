newoption
{
	trigger = "no-luajit",
	description = "Use default Lua intepreter instead of LuaJIT."
}

newoption
{
	trigger = "renderer",
	description = "Which renderer the Graphics Module will use when built.",
	allowed =
	{
		{ "d3d11", "Direct3D 11" },
		{ "d3d12", "Direct3D 12" },
		{ "vulkan", "Vulkan" },
		{ "metal", "Metal" }
	}
}
