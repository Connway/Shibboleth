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

#include "Contrivance_DockWidgetHelper.h"
#include "Contrivance_ExtensionSpawner.h"

DockWidgetHelper::DockWidgetHelper(ExtensionSpawner* spawner, QWidget* child):
	_id(0), _spawner(spawner)
{
	child->setParent(this);
}

DockWidgetHelper::~DockWidgetHelper(void)
{
	_spawner->addFreeID(_id);
}

QSize DockWidgetHelper::sizeHint(void) const
{
	return _size;
}

void DockWidgetHelper::setSizeHint(QSize size)
{
	_size = size;
}

int DockWidgetHelper::getID(void) const
{
	return _id;
}

void DockWidgetHelper::setID(int id)
{
	_id = id;
}
