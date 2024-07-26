newoption
{
	trigger = "console-app",
	description = "Generates the 'App' project to be a ConsoleApp instead of a WindowApp"
}

newoption
{
	trigger = "no-editor",
	description = "Do not generate projects related to ShibEd."
}

newoption
{
	trigger = "wayland",
	description = "On Linux, use Wayland instead of X11 window system."
}

newoption
{
	trigger = "generate-preproc",
	description = "Generate the preprocessor project."
}

newoption
{
	trigger = "preproc-pipeline",
	description = "Generate project using the preprocessor pipeline. WARNING: Copy post-build operations will copy to incorrect directories."
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

newoption
{
	trigger = "home-path",
	description = "The absolute path to resolve ~ to."
}
