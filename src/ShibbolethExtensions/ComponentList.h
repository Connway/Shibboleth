#ifndef COMPONENTLIST_H
#define COMPONENTLIST_H

#include <QWidget>

namespace Ui
{
	class ComponentList;
}

class IContrivanceWindow;

class ComponentList : public QWidget
{
	Q_OBJECT

public:
	explicit ComponentList(IContrivanceWindow& window, QWidget* parent = nullptr);
	~ComponentList();

private:
	Ui::ComponentList* _ui;
	IContrivanceWindow& _window;
};

#endif // COMPONENTLIST_H
