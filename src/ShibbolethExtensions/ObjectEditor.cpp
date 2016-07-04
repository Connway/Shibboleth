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

#include "ObjectEditor.h"
#include <Contrivance_IContrivanceWindow.h>
#include <Contrivance_ExtensionSpawner.h>
#include <QDropEvent>
#include <QHeaderView>

ObjectEditor::ObjectEditor(IContrivanceWindow& window):
	_window(window)
{
	setup();
}

ObjectEditor::~ObjectEditor(void)
{
}

/*bool ObjectEditor::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::DragEnter) {
		_window.printToConsole("OBJECTEDITOR: DRAG ENTER EVENT");
	} else if (event->type() == QEvent::DragResponse) {
		_window.printToConsole("OBJECTEDITOR: DRAG RESPONSE EVENT");
	} else if (event->type() == QEvent::Drop) {
		_window.printToConsole("OBJECTEDITOR: DROP EVENT");
	}

	return QObject::eventFilter(object, event);
}*/

void ObjectEditor::dropEvent(QDropEvent* event)
{
	(event);
	_window.printToConsole("OBJ EDIT: Drag Drop!");
	//event->acceptProposedAction();
}

void ObjectEditor::setup(void)
{
	QTreeWidgetItem* item = new QTreeWidgetItem();
	item->setText(0, QStringLiteral("1"));
	setHeaderItem(item);

	setAcceptDrops(true);
	setFrameShape(QFrame::StyledPanel);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setDragDropMode(QAbstractItemView::DropOnly);
	setDefaultDropAction(Qt::TargetMoveAction);
	setSortingEnabled(true);
	setAnimated(true);

	header()->setVisible(false);
}
