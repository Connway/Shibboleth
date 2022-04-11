// This file is generated! Any modifications will be lost in subsequent builds!

#pragma once

#ifdef SHIB_STATIC
	#undef SHIB_STATIC
	#include <../../Modules/Graphics/include/Gen_ReflectionInit.h>
	#include <../../Modules/Input/include/Gen_ReflectionInit.h>
	#include <../../Modules/Debug/include/Gen_ReflectionInit.h>
	#include <../../Modules/Script/include/Gen_ReflectionInit.h>
	#include <../../Modules/Camera/include/Gen_ReflectionInit.h>
	#include <../../Modules/ECS/include/Gen_ReflectionInit.h>
	#include <../../Modules/Entity/include/Gen_ReflectionInit.h>
	#include <../../Modules/Logic/include/Gen_ReflectionInit.h>
	#include <../../Modules/MainLoop/include/Gen_ReflectionInit.h>
	#include <../../Modules/Physics/include/Gen_ReflectionInit.h>
	#include <../../Modules/Resource/include/Gen_ReflectionInit.h>
	#include <../../Modules/Scene/include/Gen_ReflectionInit.h>
	#define SHIB_STATIC

namespace Gen::Engine
{
	static bool LoadModulesStatic(Shibboleth::App& app)
	{
		GAFF_FAIL_RETURN(app.createModule(Graphics::CreateModule, u8"Graphics"), false)
		GAFF_FAIL_RETURN(app.createModule(Input::CreateModule, u8"Input"), false)
		GAFF_FAIL_RETURN(app.createModule(Debug::CreateModule, u8"Debug"), false)
		GAFF_FAIL_RETURN(app.createModule(Script::CreateModule, u8"Script"), false)
		GAFF_FAIL_RETURN(app.createModule(Camera::CreateModule, u8"Camera"), false)
		GAFF_FAIL_RETURN(app.createModule(ECS::CreateModule, u8"ECS"), false)
		GAFF_FAIL_RETURN(app.createModule(Entity::CreateModule, u8"Entity"), false)
		GAFF_FAIL_RETURN(app.createModule(Logic::CreateModule, u8"Logic"), false)
		GAFF_FAIL_RETURN(app.createModule(MainLoop::CreateModule, u8"MainLoop"), false)
		GAFF_FAIL_RETURN(app.createModule(Physics::CreateModule, u8"Physics"), false)
		GAFF_FAIL_RETURN(app.createModule(Resource::CreateModule, u8"Resource"), false)
		GAFF_FAIL_RETURN(app.createModule(Scene::CreateModule, u8"Scene"), false)

		return true;
	}
}

#endif
