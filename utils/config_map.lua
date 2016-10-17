if os.get() == "windows" then
	if _OPTIONS["gen-clang"] then
		configmap {
			["Debug_OpenGL"] = "Debug",
			["Release_OpenGL"] = "Release",
			["Debug_Direct3D"] = "Debug",
			["Release_Direct3D"] = "Release",
			["Debug_OpenGL_Clang"] = "Debug_Clang",
			["Release_OpenGL_Clang"] = "Release_Clang",
			["Debug_Direct3D_Clang"] = "Debug_Clang",
			["Release_Direct3D_Clang"] = "Release_Clang"
		}
	else
		configmap {
			["Debug_OpenGL"] = "Debug",
			["Release_OpenGL"] = "Release",
			["Debug_Direct3D"] = "Debug",
			["Release_Direct3D"] = "Release"
		}
	end
else
	configmap {
		["Debug_OpenGL"] = "Debug",
		["Release_OpenGL"] = "Release"
	}
end
