/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

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

template <class Allocator>
DynamicLoader<Allocator>::DynamicLoader(const Allocator& allocator):
	_modules(allocator), _allocator(allocator)
{
}

template <class Allocator>
DynamicLoader<Allocator>::~DynamicLoader(void)
{
	clear();
}

template <class Allocator>
void DynamicLoader<Allocator>::clear(void)
{
	_modules.clear();
}

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const char8_t* filename, const char8_t* name)
{
	GAFF_ASSERT(filename && name && eastl::CharStrlen(filename) && eastl::CharStrlen(name));

	auto it = Find(_modules, FNV1aHash32String(name));

	if (it != _modules.end()) {
		return it->second;
	}

	ModulePtr module_ptr = MakeShared<DynamicModule, Allocator>(_allocator);

	if (module_ptr) {
		if (module_ptr->load(filename)) {
			_modules.emplace(std::move(HString(name, _allocator)), module_ptr);
			return module_ptr;
		}
	}

	return ModulePtr();
}

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::getModule(const char8_t* name)
{
	GAFF_ASSERT(name && eastl::CharStrlen(name));
	auto it = Find(_modules, FNV1aHash32String(name));

	if (it != _modules.end()) {
		return it->second;
	}

	return ModulePtr();
}

template <class Allocator>
void DynamicLoader<Allocator>::removeModule(const char8_t* name)
{
	GAFF_ASSERT(name && eastl::CharStrlen(name));
	auto it = Find(_modules, FNV1aHash32String(name));

	if (it != _modules.end()) {
		_modules.erase(it);
	}
}
