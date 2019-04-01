/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Shibboleth_EditorItemSelectedMessage.h"

SHIB_REFLECTION_DEFINE(EditorItemSelectedMessage)

NS_SHIBBOLETH

SHIB_REFLECTION_CLASS_DEFINE(EditorItemSelectedMessage)

EditorItemSelectedMessage::EditorItemSelectedMessage(const Gaff::IReflectionDefinition& ref_def, void* data):
	_ref_def(ref_def), _data(data)
{
}

EditorItemSelectedMessage::EditorItemSelectedMessage(Gaff::IReflectionObject& item):
	_ref_def(item.getReflectionDefinition()), _data(item.getBasePointer())
{
}

const Gaff::IReflectionDefinition& EditorItemSelectedMessage::getRefDef(void) const
{
	return _ref_def;
}

void* EditorItemSelectedMessage::getData(void) const
{
	return _data;
}

NS_END
