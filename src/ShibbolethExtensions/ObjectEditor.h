#ifndef OBJECTEDITOR_H
#define OBJECTEDITOR_H

#include <Contrivance_ContrivanceExtension.h>

namespace Ui
{
	class ObjectEditor;
}

class IContrivanceWindow;

class ObjectEditor : public ContrivanceExtension
{
	Q_OBJECT

public:
	explicit ObjectEditor(IContrivanceWindow& window);
	~ObjectEditor();

	bool eventFilter(QObject* object, QEvent* event);

private:
	Ui::ObjectEditor* _ui;
};

#endif // OBJECTEDITOR_H
