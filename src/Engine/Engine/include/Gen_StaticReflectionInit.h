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
	static bool LoadModulesStatic(Shibboleth::InitMode mode)
	{
		Shibboleth::IApp& app = Shibboleth::GetApp();

		GAFF_FAIL_RETURN(Graphics::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Input::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Debug::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Script::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Camera::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(ECS::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Entity::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Logic::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(MainLoop::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Physics::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Resource::Initialize(app, mode), false)
		GAFF_FAIL_RETURN(Scene::Initialize(app, mode), false)

		return true;
	}
}

#endif
