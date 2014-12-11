#include "ContrivanceWindow.h"
#include "ui_ContrivanceWindow.h"

ContrivanceWindow::ContrivanceWindow(QWidget *parent):
    QMainWindow(parent), ui(new Ui::ContrivanceWindow)
{
    ui->setupUi(this);
}

ContrivanceWindow::~ContrivanceWindow()
{
    delete ui;
}
