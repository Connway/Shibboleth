/************************************************************************************
Copyright (C) 2023 by Nicholas LaCroix

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

#include "Shibboleth_PlayerInputSubsystem.h"
#include "Shibboleth_InputManager.h"
#include "Shibboleth_InputMapping.h"
#include <Shibboleth_AppUtils.h>

SHIB_REFLECTION_DEFINE_BEGIN(Shibboleth::PlayerInputSubsystem)
	.template base<Shibboleth::LocalPlayerSubsystem>()
	.template ctor<>()
SHIB_REFLECTION_DEFINE_END(Shibboleth::PlayerInputSubsystem)


NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(PlayerInputSubsystem)


void PlayerInputSubsystem::init(const SubsystemCollectorBase& /*collector*/)
{
	InputManager& input_mgr = GetManagerTFast<InputManager>();
	GAFF_REF(input_mgr);

	const auto* const default_mapping_config = GetConfig<InputMappingConfig>();

	for (const InputMapping& mapping : default_mapping_config->mappings) {
		GAFF_REF(mapping);
	}
}

void PlayerInputSubsystem::destroy(const SubsystemCollectorBase& /*collector*/)
{
}

NS_END

