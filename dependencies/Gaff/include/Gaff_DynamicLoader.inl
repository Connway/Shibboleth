/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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
template <class Allocator2>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const AString<Allocator2>& filename, const AString<Allocator2>& name)
{
	assert(filename.size() && name.size());
	return loadModule(filename.getBuffer(), name.getBuffer());
}

template <class Allocator>
template <class Allocator2>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const AString<Allocator2>& filename, const char* name)
{
	assert(filename.size() && name && strlen(name));
	return loadModule(filename.getBuffer(), name);
}

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const char* filename, const char* name)
{
	assert(filename && name && strlen(filename) && strlen(name));

	if (_modules.indexOf(name) != -1) {
		return getModule(name);
	}

	ModulePtr module(_allocator.template allocT<DynamicModule>());

	if (module.valid()) {
		if (module->load(filename)) {
			HString str(name, FNV1Hash32, _allocator);
			_modules.insert(str, module);
			return module;
		}
	}

	return ModulePtr();
}

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::getModule(const AHashString<Allocator> name)
{
	assert(name.size());
	return _modules[name];
}

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::getModule(const AString<Allocator>& name)
{
	assert(name.size());
	HString str(name, FNV1Hash32, _allocator);
	return _modules[name];
}

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::getModule(const char* name)
{
	assert(name && strlen(name));
	HString str(name, FNV1Hash32, _allocator);
	return _modules[name];
}

template <class Allocator>
void DynamicLoader<Allocator>::removeModule(const char* name)
{
	assert(name && strlen(name));
	HString str(name, FNV1Hash32, _allocator);
	_modules.erase(str);
}

#ifdef _UNICODE

template <class Allocator>
template <class Allocator2>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const WString<Allocator2>& filename, const AString<Allocator2>& name)
{
	assert(filename.size() && name.size());
	return loadModule(filename.getBuffer(), name.getBuffer());
}

template <class Allocator>
template <class Allocator2>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const WString<Allocator2>& filename, const char* name)
{
	assert(filename.size() && name && strlen(name));
	return loadModule(filename.getBuffer(), name);
}

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const wchar_t* filename, const char* name)
{
	assert(filename && name && wcslen(filename) && strlen(name));

	if (_modules.indexOf(name) != -1) {
		return getModule(name);
	}

	ModulePtr module = _allocator.template allocT<DynamicModule>();

	if (module.valid()) {
		if (module->load(filename)) {
			HString str(name, FNV1Hash32, _allocator);
			_modules.insert(str, module);
			return module;
		}
	}

	return ModulePtr();
}

#endif
