if os.get() == "windows" then
	configmap {
		["Debug_OpenGL"] = "Debug",
		["Release_OpenGL"] = "Release",
		["Debug_Direct3D"] = "Debug",
		["Release_Direct3D"] = "Release"
	}
else
	configmap {
		["Debug_OpenGL"] = "Debug",
		["Release_OpenGL"] = "Release"
	}
end
