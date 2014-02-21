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
}

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const char* filename)
{
	ModulePtr module = _allocator.template allocT<DynamicModule>();

	if (module.valid()) {
		if (module->load(filename)) {
			_modules.push(module);
			return module;
		}
	}

	return ModulePtr();
}

#ifdef _UNICODE

template <class Allocator>
typename DynamicLoader<Allocator>::ModulePtr DynamicLoader<Allocator>::loadModule(const wchar_t* filename)
{
	ModulePtr module = _allocator.template allocT<DynamicModule>();

	if (module.valid()) {
		if (module->load(filename)) {
			_modules.push(module);
			return module;
		}
	}

	return ModulePtr();
}

#endif
