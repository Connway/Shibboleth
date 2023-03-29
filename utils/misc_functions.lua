function GetSharedLibExtension()
	if os.target() == "windows" then
		return ".dll"
	elseif os.target() == "linux" then
		return ".so"
	elseif os.target() == "macosx" then
		return ".dylib"
	end
end
