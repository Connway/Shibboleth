#ifndef COMPONENTLIST_H
#define COMPONENTLIST_H

#include <Contrivance_ContrivanceExtension.h>
#include <QListWidget>

namespace Ui
{
	class ComponentList;
}

class IContrivanceWindow;

class ComponentList : public QListWidget, public IContrivanceExtension
{
	Q_OBJECT

public:
	explicit ComponentList(IContrivanceWindow& window);
	~ComponentList();

	//bool eventFilter(QObject* object, QEvent* event);

	CONTRIVANCE_EXTENSION_DECLARATIONS;
};

#endif // COMPONENTLIST_H
