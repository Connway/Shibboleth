#ifndef TESTEXTENSION_H
#define TESTEXTENSION_H

#include <QWidget>

namespace Ui {
class TestExtension;
}

class TestExtension : public QWidget
{
	Q_OBJECT

public:
	explicit TestExtension(QWidget *parent = 0);
	~TestExtension();

private:
	Ui::TestExtension *ui;
};

#endif // TESTEXTENSION_H
