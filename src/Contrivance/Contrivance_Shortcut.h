#ifndef CONTRIVANCE_SHORTCUT_H
#define CONTRIVANCE_SHORTCUT_H

#include <QString>
#include <QPair>

class QShortcut;
class QWidget;

struct Shortcut
{
public:
	Shortcut(QWidget* _parent, const char* _member, const QString& _action, const QString& _shortcut = QString()):
		shortcut(_shortcut), action(_action), instance(nullptr), member(_member), parent(_parent)
	{
	}

	QString shortcut;
	QString action;

	QShortcut* instance;
	const char* member;
	QWidget* parent;
};

typedef QPair<QString, QString> ActionKeySequence;

#endif // CONTRIVANCE_SHORTCUT_H

