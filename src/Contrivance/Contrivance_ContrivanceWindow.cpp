#include "Contrivance_ContrivanceWindow.h"
#include "ui_Contrivance_ContrivanceWindow.h"
#include "Contrivance_ShortcutEditor.h"
#include <QJsonDocument>
#include <QTextStream>
#include <QJsonObject>
#include <QMouseEvent>
#include <QDockWidget>
#include <QShortcut>
#include <QFile>

#include "Contrivance_ExtensionSpawner.h"

ContrivanceWindow::ContrivanceWindow(QWidget* parent):
	QMainWindow(parent), _ui(new Ui::ContrivanceWindow),
	_shortcut_editor(nullptr), _tab_renamer(nullptr)
{
	_ui->setupUi(this);

	setupTabRenamer();

	// Register the main toolbar with the toolbar management system
	_toolbars["main"].toolbar = _ui->mainToolBar;

	// Connect menu actions and tab renaming actions
	connect(_ui->action_Shortcut_Editor, SIGNAL(triggered()), this, SLOT(shortcutEditor()));
	connect(_ui->actionAbout_Qt, SIGNAL(triggered()), this, SLOT(aboutQt()));
	connect(_ui->actionE_xit, SIGNAL(triggered()), this, SLOT(exit()));
	connect(_ui->tabWidget->tabBar(), SIGNAL(tabBarDoubleClicked(int)), this, SLOT(tabDoubleClicked(int)));
	connect(_ui->tabWidget->tabBar(), SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

	// Register the new tab keyboard shortcut and toolbar button. Also create the Shortcut Editor modal widget.
	registerNewShortcut(this, SLOT(newTab()), tr("New Tab"), QKeySequence(Qt::CTRL + Qt::Key_N));
	_shortcut_editor = new ShortcutEditor(*this);

	registerNewToolbarAction(QIcon("icons/tab_add.png"), this, SLOT(newTab()), "main", "group_a");

	// load all editor extensions so that they can register themselves

	loadShortcuts("keyboard_shortcuts.json");

	setupExtensionSpawner();

	// if we have a saved config file with the editor's spawned extensions and layout, use that to populate the editor, otherwise, use defaults.
	addDockWidget(Qt::RightDockWidgetArea, (QDockWidget*)_extension_spawner->parentWidget());
}

ContrivanceWindow::~ContrivanceWindow()
{
	delete _ui;
}

void ContrivanceWindow::registerNewShortcut(QWidget* parent, const char* member, const QString& action, const QKeySequence& shortcut)
{
//#ifdef QT_DEBUG
	// Check if there is another shortuct using this key sequence.
	for (auto it = _shortcuts.begin(); it != _shortcuts.end(); ++it) {
		if (!it->shortcut.isEmpty() && it->shortcut == shortcut) {
			//Q_ASSERT(it->shortcut != shortcut);

			// Use empty key sequence if another shortcut is already using it.
			_shortcuts.push_back(Shortcut(parent, member, action, QKeySequence()));
			return;
		}
	}
//#endif

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

	// Add the new shortcuts and construct the JSON object.
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

void ContrivanceWindow::registerNewToolbarAction(const QIcon& icon, const QObject* receiver, const char* member, const QString& toolbar_name, const QString& group_name)
{
	// See if the toolbar we are adding ourselves to is already registered.
	auto it_tb = _toolbars.find(toolbar_name);

	// If not, add a new toolbar.
	if (it_tb == _toolbars.end()) {
		QToolBar* toolbar = new QToolBar(this);
		toolbar->setAllowedAreas(Qt::AllToolBarAreas);
		toolbar->setObjectName(toolbar_name);
		toolbar->installEventFilter(this);
		toolbar->setFloatable(true);
		toolbar->setMovable(true);
		addToolBar(Qt::TopToolBarArea, toolbar);

		ToolbarData data;
		data.toolbar = toolbar;

		it_tb = _toolbars.insert(toolbar_name, data);
	}

	// Check that toolbar's separator groups for the group we want to be added to.
	auto it_sep = it_tb->separators.find(group_name);

	// If it doesn't exist, add the separator we made for the previous group to the toolbar
	// and make a new separator for our group, but don't add it to the toolbar.
	if (it_sep == it_tb->separators.end()) {
		QAction* separator = it_tb->toolbar->addSeparator();
		it_tb->toolbar->removeAction(separator);

		it_sep = it_tb->separators.insert(group_name, separator);

		if (!it_tb->lastGroup.isEmpty()) {
			it_tb->toolbar->addAction(it_tb->separators[it_tb->lastGroup]);
		}

		it_tb->lastGroup = group_name;
	}

	// Add ourselves to the toolbar.
	QAction* action = new QAction(icon, "", this);
	connect(action, SIGNAL(triggered()), receiver, member);

	it_tb->toolbar->insertAction(*it_sep, action);
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

		// Find the associated action and set it's shortcut to the value from the JSON file.
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
			hideTabRenamer();
		}
	}

	return QMainWindow::eventFilter(object, event);
}

bool ContrivanceWindow::event(QEvent* event)
{
	if (_tab_renamer && _tab_renamer->isVisible()) {
		if (event->type() == QEvent::MouseButtonPress) {
			QMouseEvent* mouse_event = (QMouseEvent*)event;
			QWidget* widget = childAt(mouse_event->pos());

			if (widget != _tab_renamer) {
				hideTabRenamer();
			}
		}
	}

	return QMainWindow::event(event);
}

void ContrivanceWindow::setupTabRenamer(void)
{
	// Create the Tab Renamer widget
	_tab_renamer = new QTextEdit(this);
	_tab_renamer->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_tab_renamer->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	_tab_renamer->hide();
	_tab_renamer->setEnabled(false);

	// Register event filters for disabling the tab renamer when clicking on other things
	_ui->mainToolBar->installEventFilter(this);
	_ui->menuBar->installEventFilter(this);
	_tab_renamer->installEventFilter(this);
}

void ContrivanceWindow::hideTabRenamer(void)
{
	_ui->tabWidget->setTabText(_tab_being_renamed, _tab_renamer->toPlainText());
	_tab_renamer->setEnabled(false);
	_tab_renamer->hide();
}

void ContrivanceWindow::setupExtensionSpawner(void)
{
	// Create the initial dock widget with the Extension Spawner.
	QDockWidget* dw = new QDockWidget(tr("Extension Spawner"));
	QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	dw->resize(400, 300);
	dw->setSizePolicy(sizePolicy);
	dw->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dw->setAllowedAreas(Qt::AllDockWidgetAreas);

	_extension_spawner = new ExtensionSpawner(dw);
	dw->setWidget(_extension_spawner);

	// Add the window hide checkbox for the Extension Spawner to the "Window" menu.
	_ui->menu_Window->addActions(QList<QAction*>{dw->toggleViewAction()});

	// Add "Spawned Windows" menu to the "Window" menu.
	QMenu* spawned_windows = new QMenu(tr("Spawned Windows"));
	_ui->menu_Window->addMenu(spawned_windows);
	spawned_windows->setEnabled(false);
}

void ContrivanceWindow::currentTabChanged(int /*index*/)
{
	if (_tab_renamer->isVisible()) {
		hideTabRenamer();
	}
}

void ContrivanceWindow::tabDoubleClicked(int index)
{
	// Show the tab renamer widget at the location of the tab
	QPoint pos = _ui->centralWidget->mapToParent(QPoint(0, 0)); // Get our position in the window
	QRect rect = _ui->tabWidget->tabBar()->tabRect(index);
	_tab_renamer->setText(_ui->tabWidget->tabText(index));
	_tab_renamer->move(rect.topLeft() + pos); // Add relative tab position to our window position
	_tab_renamer->resize(rect.size());
	_tab_renamer->setEnabled(true);
	_tab_renamer->show();

	_tab_being_renamed = index;
}

void ContrivanceWindow::shortcutEditor(void)
{
	if (_shortcut_editor->isHidden()) {
		_shortcut_editor->refreshShortcuts();
		_shortcut_editor->show();
	}
}

void ContrivanceWindow::aboutQt(void)
{
	qApp->aboutQt();
}

void ContrivanceWindow::newTab(void)
{
	static unsigned int tab_num = 2;

	QString tab_text;
	QTextStream text_stream(&tab_text);
	text_stream << tr("Tab") << " " << tab_num;
	++tab_num;

	_ui->tabWidget->addTab(new QWidget(), tab_text);
}

void ContrivanceWindow::exit(void)
{
	qApp->exit();
}
