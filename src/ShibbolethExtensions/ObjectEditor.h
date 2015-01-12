#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include <QWidget>

namespace Ui
{
	class ObjectEditor;
}

class IContrivanceWindow;

class ObjectEditor : public QWidget
{
	Q_OBJECT

public:
	explicit ObjectEditor(IContrivanceWindow& window, QWidget* parent = nullptr);
	~ObjectEditor();

	bool eventFilter(QObject* object, QEvent* event);

	void dragEnterEvent(QDragEnterEvent *);
	void dragMoveEvent(QDragMoveEvent *);
	void dragLeaveEvent(QDragLeaveEvent *);
	void dropEvent(QDropEvent *);

private:
	Ui::ObjectEditor* _ui;
	IContrivanceWindow& _window;
};

#endif // OBJECTEDITOR_H
