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

#pragma once

#include <Config/Shibboleth_Config.h>

NS_SHIBBOLETH

class ScriptConfig final : public Refl::IReflectionObject
{
public:
	static constexpr const char8_t* const k_script_pool_name = u8"Lua";
	static constexpr Gaff::Hash32 k_script_pool = Gaff::FNV1aHash32StringConst(k_script_pool_name);
	static constexpr const char* const k_script_loaded_chunks_name = "__loaded_chunks";

	int32_t num_threads = 4;

	SHIB_REFLECTION_CLASS_DECLARE(ScriptConfig);
};

NS_END

SHIB_REFLECTION_DECLARE(Shibboleth::ScriptConfig)
