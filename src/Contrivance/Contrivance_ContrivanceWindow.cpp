#include "Contrivance_ContrivanceWindow.h"
#include "ui_Contrivance_ContrivanceWindow.h"
#include "Contrivance_ShortcutEditor.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QMouseEvent>
#include <QShortcut>
#include <QFile>

ContrivanceWindow::ContrivanceWindow(QWidget* parent):
	QMainWindow(parent), _ui(new Ui::ContrivanceWindow),
	_shortcutEditor(nullptr), _tab_renamer(nullptr)
{
	_ui->setupUi(this);

	_tab_renamer = new QTextEdit(this);
	_tab_renamer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_tab_renamer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_tab_renamer->hide();
	_tab_renamer->setEnabled(false);

	_ui->mainToolBar->installEventFilter(this);
	_tab_renamer->installEventFilter(this);

	connect(_ui->action_Shortcut_Editor, SIGNAL(triggered()), this, SLOT(shortcutEditor()));
	connect(_ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect(_ui->actionE_xit, SIGNAL(triggered()), this, SLOT(exit()));
	connect(_ui->tabWidget->tabBar(), SIGNAL(tabBarDoubleClicked(int)), this, SLOT(tabDoubleClicked(int)));
	connect(_ui->tabWidget->tabBar(), SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

	registerNewShortcut(this, SLOT(newTab()), "New Tab", QKeySequence(Qt::CTRL + Qt::Key_N));

	_shortcutEditor = new ShortcutEditor(*this);

	registerNewToolbarAction(QIcon("document_new.png"), this, SLOT(newTab()));

	// load all editor extensions so that they can register their actions

	loadShortcuts("keyboard_shortcuts.json");
}

ContrivanceWindow::~ContrivanceWindow()
{
	delete _ui;
}

void ContrivanceWindow::registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QKeySequence& shortcut)
{
#ifdef QT_DEBUG
	for (auto it = _shortcuts.begin(); it != _shortcuts.end(); ++it) {
		if (!it->shortcut.isEmpty()) {
			Q_ASSERT(it->shortcut != shortcut);
		}
	}
#endif

	if (shortcut.isEmpty()) {
		_shortcuts.push_back(Shortcut(parent, member, action, shortcut));
	} else {
		Shortcut data(parent, member, action, shortcut);
		data.instance = new QShortcut(shortcut, parent, member);
		_shortcuts.push_back(data);
	}
}

void ContrivanceWindow::setNewShortcuts(const QList<QKeySequence>& shortcuts)
{
	QJsonObject root_obj;

	// Add the new shortcuts
	for (int i = 0; i < shortcuts.size(); ++i) {
		root_obj.insert(_shortcuts[i].action, QJsonValue(shortcuts[i].toString()));
		_shortcuts[i].instance->setKey(shortcuts[i]);
		_shortcuts[i].shortcut = shortcuts[i];
	}

	saveShortcuts(root_obj, "keyboard_shortcuts.json");
}

const QList<Shortcut>& ContrivanceWindow::retrieveShortcuts(void) const
{
	return _shortcuts;
}

void ContrivanceWindow::registerNewToolbarAction(const QIcon& icon, const QObject* receiver, const char* member)
{
	_ui->mainToolBar->addAction(icon, "", receiver, member);
}

bool ContrivanceWindow::saveShortcuts(const QJsonObject& shortcuts, const QString& file)
{
	QFile config(file);

	if (config.open(QFile::WriteOnly | QFile::Text)) {
		QJsonDocument json_doc(shortcuts);
		return config.write(json_doc.toJson()) > 0;
	}

	return false;
}

bool ContrivanceWindow::loadShortcuts(const QString& file)
{
	QFile config(file);

	if (config.open(QFile::ReadOnly | QFile::Text)) {
		QJsonDocument json_doc = QJsonDocument::fromJson(config.readAll());
		QJsonObject root_obj = json_doc.object();

		for (auto it = _shortcuts.begin(); it != _shortcuts.end(); ++it) {
			auto value = root_obj.find(it->action);

			if (value != root_obj.end()) {
				it->shortcut = QKeySequence::fromString((*value).toString());
				it->instance->setKey(it->shortcut);
			}
		}

		return true;
	}

	return false;
}

bool ContrivanceWindow::eventFilter(QObject* object, QEvent* event)
{
	if (_tab_renamer->isVisible()) {
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent* key_event = (QKeyEvent*)event;

			if ((key_event->key() == Qt::Key_Enter) || (key_event->key() == Qt::Key_Return)) {
				hideTabRenamer();
				return true;
			}
		} else if (event->type() == QEvent::MouseButtonPress) {
			QMouseEvent* mouse_event = (QMouseEvent*)event;

			if (mouse_event->button() == Qt::LeftButton) {
				hideTabRenamer();
			}
		}
	}

	return QMainWindow::eventFilter(object, event);
}

bool ContrivanceWindow::event(QEvent* event)
{
	if (_tab_renamer && _tab_renamer->isVisible()) {
		if (event->type() == QEvent::MouseButtonPress) {
			QMouseEvent* mouse_event = (QMouseEvent*)event;

			if (mouse_event->button() == Qt::LeftButton) {
				QWidget* widget = childAt(mouse_event->pos());

				if (widget != _tab_renamer) {
					hideTabRenamer();
				}
			}
		}
	}

	return QMainWindow::event(event);
}

void ContrivanceWindow::hideTabRenamer(void)
{
	_ui->tabWidget->setTabText(_tab_being_renamed, _tab_renamer->toPlainText());
	_tab_renamer->hide();
	_tab_renamer->setEnabled(false);
}

void ContrivanceWindow::currentTabChanged(int /*index*/)
{
	if (_tab_renamer->isVisible()) {
		hideTabRenamer();
	}
}

void ContrivanceWindow::tabDoubleClicked(int index)
{
	QPoint toolBarContribution(0, 0);
	_tab_being_renamed = index;

	switch (toolBarArea(_ui->mainToolBar)) {
		case Qt::TopToolBarArea:
			toolBarContribution = QPoint(0, _ui->mainToolBar->height());
			break;

		case Qt::LeftToolBarArea:
			toolBarContribution = QPoint(_ui->mainToolBar->width(), 0);
			break;

		default:
			break;
	}

	QRect rect = _ui->tabWidget->tabBar()->tabRect(index);
	_tab_renamer->setText(_ui->tabWidget->tabText(index));
	_tab_renamer->move(rect.bottomLeft() + toolBarContribution);
	_tab_renamer->resize(rect.size());
	_tab_renamer->setEnabled(true);
	_tab_renamer->show();
}

void ContrivanceWindow::shortcutEditor(void)
{
	if (_shortcutEditor->isHidden()) {
		_shortcutEditor->refreshShortcuts();
		_shortcutEditor->show();
	}
}

void ContrivanceWindow::aboutQt(void)
{
	qApp->aboutQt();
}

void ContrivanceWindow::newTab(void)
{
	_ui->tabWidget->addTab(new QWidget(), "Tab 2");
}

void ContrivanceWindow::exit(void)
{
	qApp->exit();
}
