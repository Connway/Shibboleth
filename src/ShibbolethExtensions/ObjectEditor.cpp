#include "ObjectEditor.h"
#include "ui_ObjectEditor.h"
#include <Contrivance_IContrivanceWindow.h>

ObjectEditor::ObjectEditor(IContrivanceWindow& window):
	ContrivanceExtension(window), _ui(new Ui::ObjectEditor)
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
		_window.printToConsole("OBJECTEDITOR: DRAG ENTER EVENT");
	} else if (event->type() == QEvent::DragResponse) {
		_window.printToConsole("OBJECTEDITOR: DRAG RESPONSE EVENT");
	} else if (event->type() == QEvent::Drop) {
		_window.printToConsole("OBJECTEDITOR: DROP EVENT");
	}

	return QObject::eventFilter(object, event);
}
