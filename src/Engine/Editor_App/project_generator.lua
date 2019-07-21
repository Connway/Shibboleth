if _ACTION == "vs2017" then
	externalproject "Editor_App_2017"
		location "."
		uuid "57940020-8E99-AEB6-271F-61E0F7F6B73B"
		kind "WindowedApp"
		language "C#"

elseif _ACTION == "vs2019" then
	externalproject "Editor_App_2019"
		location "."
		uuid "68E9B1E5-2F14-4C53-9BC2-A4025EAC9E14"
		kind "WindowedApp"
		language "C#"
end
