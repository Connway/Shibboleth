#ifndef CONTRIVANCEWINDOW_H
#define CONTRIVANCEWINDOW_H

#include <QMainWindow>

namespace Ui {
class ContrivanceWindow;
}

class ContrivanceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ContrivanceWindow(QWidget *parent = 0);
    ~ContrivanceWindow();

private:
    Ui::ContrivanceWindow *ui;
};

#endif // CONTRIVANCEWINDOW_H
