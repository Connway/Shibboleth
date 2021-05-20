/************************************************************************************
Copyright (C) 2021 by Nicholas LaCroix

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

#include "Shibboleth_RuntimeVarManager.h"

#ifdef SHIB_RUNTIME_VAR_ENABLED

NS_SHIBBOLETH

IRuntimeVar* IRuntimeVar::_root = nullptr;


void RegisterRuntimeVars(void)
{
	RuntimeVarManager& run_var_mgr = GetApp().getRuntimeVarManager();

	for (IRuntimeVar* var = IRuntimeVar::_root; var; var = var->_next) {
		run_var_mgr.addRuntimeVar(*var);
	}
}

void RuntimeVarManager::addRuntimeVar(IRuntimeVar& runtime_var)
{
	const auto it = Gaff::LowerBound(
		_runtime_vars,
		runtime_var.getName(),
		[](IRuntimeVar* const lhs, Gaff::Hash64 rhs) -> bool { return lhs->getName() < rhs; }
	);

	GAFF_ASSERT(it == _runtime_vars.end() || (*it)->getName() != runtime_var.getName());
	_runtime_vars.insert(it, &runtime_var);
}

IRuntimeVar* RuntimeVarManager::getRuntimeVar(Gaff::Hash64 name) const
{
	const auto it = Gaff::LowerBound(
		_runtime_vars,
		name,
		[](IRuntimeVar* const lhs, Gaff::Hash64 rhs) -> bool { return lhs->getName() < rhs; }
	);

	return it != _runtime_vars.end() && (*it)->getName() == name ? *it : nullptr;
}

IRuntimeVar* RuntimeVarManager::getRuntimeVar(const char* name) const
{
	return getRuntimeVar(Gaff::FNV1aHash64String(name));
}

NS_END

#endif
