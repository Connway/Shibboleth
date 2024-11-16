/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#define SHIB_REFL_IMPL
#include "Config/Shibboleth_EngineConfig.h"
#include "Attributes/Shibboleth_EngineAttributesCommon.h"

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EngineConfig::Flag)
	.entry(u8"EditorMode", Shibboleth::EngineConfig::Flag::EditorMode)
	.entry(u8"NoManagers", Shibboleth::EngineConfig::Flag::NoManagers)
	.entry(u8"NoLoadModules", Shibboleth::EngineConfig::Flag::NoLoadModules)
	.entry(u8"NoMainLoop", Shibboleth::EngineConfig::Flag::NoMainLoop)
SHIB_REFLECTION_DEFINE_END(Shibboleth::EngineConfig::Flag)

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::EngineConfig)
	.classAttrs(
		Shibboleth::GlobalConfigAttribute{}
	)

	.template base<Shibboleth::Config>()
	.template ctor<>()

	// $TODO: Allocator attribute.
	.var(u8"module_unload_order", &Shibboleth::EngineConfig::module_unload_order, Shibboleth::OptionalAttribute{})
	.var(u8"module_load_order", &Shibboleth::EngineConfig::module_load_order, Shibboleth::OptionalAttribute{})
	.var(u8"module_directories", &Shibboleth::EngineConfig::module_directories, Shibboleth::OptionalAttribute{})

	.var(u8"working_dir", &Shibboleth::EngineConfig::working_dir, Shibboleth::OptionalAttribute{})
	.var(u8"log_dir", &Shibboleth::EngineConfig::log_dir, Shibboleth::OptionalAttribute{})

	.var(u8"file_system", &Shibboleth::EngineConfig::file_system, Shibboleth::OptionalAttribute{})
	.var(u8"main_loop", &Shibboleth::EngineConfig::main_loop, Shibboleth::OptionalAttribute{})

	.var(u8"read_file_threads", &Shibboleth::EngineConfig::read_file_threads, Shibboleth::OptionalAttribute{})

	.var(u8"flags", &Shibboleth::EngineConfig::flags, Shibboleth::OptionalAttribute{})
SHIB_REFLECTION_DEFINE_END(Shibboleth::EngineConfig)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EngineConfig)

NS_END
