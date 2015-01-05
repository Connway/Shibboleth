#include "TestExtension.h"
#include "ui_TestExtension.h"

TestExtension::TestExtension(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TestExtension)
{
	ui->setupUi(this);
}

TestExtension::~TestExtension()
{
	delete ui;
}
