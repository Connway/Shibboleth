-- if os.get() == "windows" then
-- 	os.execute("qmake -spec win64-msvc2013 -tp vc")
-- end

-- external "App"
-- 	if _ACTION then
-- 		location ("../../project/" .. _ACTION .. "/app")
-- 	end

-- 	if _OPTIONS["console_app"] then
-- 		kind "ConsoleApp"
-- 	else
-- 		kind "WindowedApp"
-- 	end

-- 	language "C++"
