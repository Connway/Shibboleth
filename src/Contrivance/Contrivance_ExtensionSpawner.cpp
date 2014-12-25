#include "Contrivance_ExtensionSpawner.h"
#include "ui_Contrivance_ExtensionSpawner.h"

ExtensionSpawner::ExtensionSpawner(QWidget* parent):
	QWidget(parent), _ui(new Ui::ExtensionSpawner)
{
	_ui->setupUi(this);

	_ui->listWidget->addItem("Test Item");
}

ExtensionSpawner::~ExtensionSpawner()
{
	delete _ui;
}
