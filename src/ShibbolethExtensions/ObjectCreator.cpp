/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "ObjectCreator.h"
#include "ComponentList.h"
#include "ObjectEditor.h"
#include <QHBoxLayout>

ObjectCreator::ObjectCreator(IContrivanceWindow& window):
	ContrivanceExtension(window),
	_cl(nullptr), _oe(nullptr), _hl(nullptr)
{
	setup();
}

ObjectCreator::~ObjectCreator(void)
{
}

void ObjectCreator::setup(void)
{
	_cl = new ComponentList(_window);
	_oe = new ObjectEditor(_window);
	_hl = new QHBoxLayout(this);

	_cl->setParent(this);
	_oe->setParent(this);

	_hl->addWidget(_oe);
	_hl->addWidget(_cl);

	setLayout(_hl);
}
