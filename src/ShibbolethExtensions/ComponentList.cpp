#include "ComponentList.h"
#include "ui_ComponentList.h"

ComponentList::ComponentList(IContrivanceWindow& window, QWidget *parent):
	QWidget(parent), _ui(new Ui::ComponentList), _window(window)
{
	_ui->setupUi(this);

	_ui->listWidget->addItem("Test Item");
}

ComponentList::~ComponentList()
{
	delete _ui;
}