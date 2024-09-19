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

#include "Shibboleth_AngelScriptArray.h"
#include "Shibboleth_IncludeAngelScript.h"
#include <scriptarray/scriptarray.h>
#include <span>

#define CHECK_RESULT(r) if (r < 0) { return false; }

namespace
{
	static size_t SpanGetSize(const std::span<int32_t>& span)
	{
		return span.size();
	}

	static bool SpanIsEmpty(const std::span<int32_t>& span)
	{
		return span.empty();
	}

	static std::span<int32_t> ScriptArrayConvertToSpan(CScriptArray& array)
	{
		return std::span<int32_t>{ reinterpret_cast<int32_t*>(array.GetBuffer()), array.GetSize() };
	}
}

NS_SHIBBOLETH

bool RegisterScriptArray_Native(asIScriptEngine *engine)
{
	RegisterScriptArray(engine, true);

	int result = engine->RegisterObjectType("Shibboleth::Span<class T>", sizeof(std::span<int32_t>), asOBJ_VALUE | asOBJ_TEMPLATE | asGetTypeTraits< std::span<int32_t> >()); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("Shibboleth::Span<T>", "uint64 get_size() const", asFUNCTION(SpanGetSize), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)
	result = engine->RegisterObjectMethod("Shibboleth::Span<T>", "bool get_empty() const", asFUNCTION(SpanIsEmpty), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)

	result = engine->RegisterObjectMethod("array<T>", "Shibboleth::Span<T> opImplConv() const", asFUNCTION(ScriptArrayConvertToSpan), asCALL_CDECL_OBJFIRST); CHECK_RESULT(result)

	return true;
}

NS_END
