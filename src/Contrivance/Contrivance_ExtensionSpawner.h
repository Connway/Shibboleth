#ifndef CONTRIVANCE_EXTENSIONSPAWNER_H
#define CONTRIVANCE_EXTENSIONSPAWNER_H

#include <QWidget>

namespace Ui
{
	class ExtensionSpawner;
}

class ExtensionSpawner : public QWidget
{
	Q_OBJECT

public:
	explicit ExtensionSpawner(QWidget* parent = nullptr);
	~ExtensionSpawner();

private:
	Ui::ExtensionSpawner* _ui;
};

#endif // CONTRIVANCE_EXTENSIONSPAWNER_H
