#include "Contrivance_ContrivanceWindow.h"
#include "ui_Contrivance_ContrivanceWindow.h"
#include "Contrivance_ShortcutEditor.h"
#include <QJsonDocument>
#include <QPushButton>
#include <QShortcut>

#include <iostream>

ContrivanceWindow::ContrivanceWindow(QWidget *parent):
	QMainWindow(parent), _ui(new Ui::ContrivanceWindow),
	_shortcutEditor(nullptr)
{
	_ui->setupUi(this);

	connect(_ui->action_Shortcut_Editor, SIGNAL(triggered()), this, SLOT(shortcutEditor()));
	connect(_ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect(_ui->actionE_xit, SIGNAL(triggered()), this, SLOT(exit()));

	QPushButton* button = new QPushButton("Test");
	_ui->gridLayout->addWidget(button, 0, 0);

	registerNewShortcut(this, SLOT(newTab()), "New Tab", "Ctrl+N");

	_shortcutEditor = new ShortcutEditor(*this);
}

ContrivanceWindow::~ContrivanceWindow()
{
	delete _ui;
}



void ContrivanceWindow::registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QString& shortcut)
{
#ifdef QT_DEBUG
	for (auto it = _shortcuts.begin(); it != _shortcuts.end(); ++it) {
		if (it->shortcut.size()) {
			Q_ASSERT(it->shortcut != shortcut);
		}
	}
#endif

	if (shortcut.size()) {
		Shortcut data(parent, member, action, shortcut);
		data.instance = new QShortcut(QKeySequence(shortcut), parent, member);
		_shortcuts.push_back(data);
	} else {
		_shortcuts.push_back(Shortcut(parent, member, action, shortcut));
	}
}

void ContrivanceWindow::setNewShortcuts(const QList<QString>& shortcuts)
{
	// Clear all current shortcuts
	for (auto it = _shortcuts.begin(); it != _shortcuts.end(); ++it) {
		if (it->instance) {
			delete it->instance;
			it->instance = nullptr;
		}
	}

	// Add the new shortcuts
	for (int i = 0; i < shortcuts.size(); ++i) {
		_shortcuts[i].shortcut = shortcuts[i];

		if (shortcuts[i].size()) {
			_shortcuts[i].instance = new QShortcut(QKeySequence(shortcuts[i]), _shortcuts[i].parent, _shortcuts[i].member);
		}
	}
}

const QList<Shortcut>& ContrivanceWindow::retrieveShortcuts(void) const
{
	return _shortcuts;
}

void ContrivanceWindow::shortcutEditor(void)
{
	if (_shortcutEditor->isHidden()) {
		_shortcutEditor->show();
	}
}

void ContrivanceWindow::aboutQt(void)
{
    qApp->aboutQt();
}

void ContrivanceWindow::newTab(void)
{
	qApp->aboutQt();
}

void ContrivanceWindow::exit(void)
{
    qApp->exit();
}
