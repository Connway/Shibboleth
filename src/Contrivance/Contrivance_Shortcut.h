#ifndef CONTRIVANCE_SHORTCUT_H
#define CONTRIVANCE_SHORTCUT_H

#include <QKeySequence>
#include <QString>

class QShortcut;
class QWidget;

struct Shortcut
{
public:
	Shortcut(QWidget* _parent, const char* _member, const QString& _action, const QKeySequence& _shortcut = QKeySequence()):
		shortcut(_shortcut), action(_action), instance(nullptr), member(_member), parent(_parent)
	{
	}

	QKeySequence shortcut;
	QString action;

	QShortcut* instance;
	const char* member;
	QWidget* parent;
};

#endif // CONTRIVANCE_SHORTCUT_H

