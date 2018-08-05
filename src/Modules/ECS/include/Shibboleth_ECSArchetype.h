/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include <Shibboleth_Vector.h>

NS_GAFF
	class IReflectionDefinition;
NS_END

NS_SHIBBOLETH

class ECSArchetype
{
public:
	void add(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	void add(const Gaff::IReflectionDefinition* ref_def);
	void remove(const Vector<const Gaff::IReflectionDefinition*>& ref_defs);
	void remove(const Gaff::IReflectionDefinition* ref_def);
	void remove(int32_t index);

	int32_t size(void) const;

private:
	Vector<const Gaff::IReflectionDefinition*> _vars;
	int32_t _alloc_size = 0;
};

NS_END
