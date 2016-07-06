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

#include "Shibboleth_ComponentList.h"
#include <Contrivance_IContrivanceWindow.h>
#include <Contrivance_ExtensionSpawner.h>

NS_SHIBBOLETH

CONTRIVANCE_EXTENSION_IMPLEMENATION(ComponentList)

ComponentList::ComponentList(Contrivance::IContrivanceWindow& window):
	CONTRIVANCE_EXTENSION_INITIALIZER_LIST
{
	setup();
	addItem("Test Item");
}

ComponentList::~ComponentList(void)
{
	CONTRIVANCE_EXTENSION_DESTRUCTOR;
}

/*bool ComponentList::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::DragEnter) {
		_window.printToConsole("DRAG ENTER EVENT");
	} else if (event->type() == QEvent::DragResponse) {
		_window.printToConsole("DRAG RESPONSE EVENT");
	} else if (event->type() == QEvent::Drop) {
		_window.printToConsole("DROP EVENT");
	} else if (event->type() == QEvent::MouseButtonDblClick) {
		_window.printToConsole("DOUBLE CLICK");
	}

	return QObject::eventFilter(object, event);
}*/

void ComponentList::setup(void)
{
	QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sp.setHorizontalStretch(0);
	sp.setVerticalStretch(0);
	sp.setHeightForWidth(sizePolicy().hasHeightForWidth());

	setSizePolicy(sp);

	setFrameShape(QFrame::StyledPanel);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setProperty("showDropIndicator", QVariant(false));
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragOnly);
	setDefaultDropAction(Qt::IgnoreAction);
	setResizeMode(QListView::Adjust);
	setSortingEnabled(true);
}

NS_END
