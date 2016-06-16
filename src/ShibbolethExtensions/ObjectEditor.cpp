#include "ObjectEditor.h"
#include "ui_ObjectEditor.h"
#include <Contrivance_IContrivanceWindow.h>

ObjectEditor::ObjectEditor(IContrivanceWindow& window):
	_ui(new Ui::ObjectEditor), _window(window)
{
	_ui->setupUi(this);

	_ui->treeWidget->installEventFilter(this);
}

ObjectEditor::~ObjectEditor()
{
	delete _ui;
}

bool ObjectEditor::eventFilter(QObject* object, QEvent* event)
{
	if (event->type() == QEvent::DragEnter) {
		_window.printToConsole("DRAG ENTER EVENT");
	} else if (event->type() == QEvent::DragResponse) {
		_window.printToConsole("DRAG RESPONSE EVENT");
	} else if (event->type() == QEvent::Drop) {
		_window.printToConsole("DROP EVENT");
	}

	return QObject::eventFilter(object, event);
}

void ObjectEditor::dragEnterEvent(QDragEnterEvent *e)
{
	e = e;
}

void ObjectEditor::dragMoveEvent(QDragMoveEvent *e)
{
	e = e;
}

void ObjectEditor::dragLeaveEvent(QDragLeaveEvent *e)
{
	e = e;
}

void ObjectEditor::dropEvent(QDropEvent *e)
{
	e = e;
}
