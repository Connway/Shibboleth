#ifndef CONTRIVANCE_ICONTRIVANCEWINDOW_H
#define CONTRIVANCE_ICONTRIVANCEWINDOW_H

#include "Contrivance_Defines.h"
#include <QWidget>

class IContrivanceWindow
{
public:
	IContrivanceWindow(void) {}
	virtual ~IContrivanceWindow(void) {}

	virtual void registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QKeySequence& shortcut = QKeySequence()) = 0;
	virtual void registerNewToolbarAction(const QIcon& icon, const QObject* receiver, const char* member, const QString& toolbar_name, const QString& group_name) = 0;
	virtual void printToConsole(const QString& message, ConsoleMessageType type = CMT_NORMAL) = 0;
};

#endif // CONTRIVANCE_ICONTRIVANCEWINDOW_H