#include "Shibboleth_EditorMainWindow.h"
#include <QCoreApplication>
#include <QStatusBar>
#include <QMenuBar>
#include <QLabel>

EditorMainWindow::EditorMainWindow(QWidget *parent):
	QMainWindow(parent)
{
	if (objectName().isEmpty()) {
		setObjectName(QString::fromUtf8("EditorMainWindow"));
	}

	resize(800, 600);

	QWidget* const central_widget = new QWidget(this);
	central_widget->setObjectName(QString::fromUtf8("central_widget"));
	setCentralWidget(central_widget);

	QMenuBar* const menu_bar = new QMenuBar(this);
	menu_bar->setObjectName(QString::fromUtf8("menu_bar"));
	setMenuBar(menu_bar);

	QStatusBar* const status_bar = new QStatusBar(this);
	status_bar->setObjectName(QString::fromUtf8("status_bar"));
	setStatusBar(status_bar);

	retranslateUi();

	QMetaObject::connectSlotsByName(this);


	_dock_manager = new ads::CDockManager(this);


	QMenu* const file_menu = menu_bar->addMenu(tr("&File"));

	QAction* const exitAction = new QAction(tr("E&xit"), this);
	exitAction->setStatusTip(tr("Exits and shutdowns the editor."));

	connect(exitAction, &QAction::triggered, this, &EditorMainWindow::onExit);

	file_menu->addAction(exitAction);

	
	QMenu* const window_menu = menu_bar->addMenu(tr("&Window"));
	// Add window actions
	window_menu->addSeparator();
	_windows_menu = window_menu->addMenu(tr("&Windows"));


	QMenu* const test_menu = menu_bar->addMenu(tr("&Test"));
	QAction* const testAction = new QAction(tr("Create Dock Window"), this);
	connect(testAction, &QAction::triggered, this, &EditorMainWindow::onTest);
	test_menu->addAction(testAction);

}

EditorMainWindow::~EditorMainWindow()
{
}

void EditorMainWindow::closeEvent(QCloseEvent* event)
{
	QMainWindow::closeEvent(event);
	onExit();
}

void EditorMainWindow::retranslateUi(void)
{
	setWindowTitle(QCoreApplication::translate("ShibEd", "ShibEd", nullptr));
}

void EditorMainWindow::onWindowClosed(void)
{
	ads::CDockWidget* const dock_widget = qobject_cast<ads::CDockWidget*>(sender());

	if (!dock_widget) {
		// $TODO: Log error.
		return;
	}

	QAction* const action = _window_focus_action_map[dock_widget];

	if (!action) {
		// $TODO: Log error.
		return;
	}

	_windows_menu->removeAction(action);
}

void EditorMainWindow::onFocusWindow(void)
{
	QAction* const action = qobject_cast<QAction*>(sender());

	if (!action) {
		// $TODO: Log error.
		return;
	}

	ads::CDockWidget* const dock_widget = _window_focus_action_map.key(action);

	if (!dock_widget) {
		// $TODO: Log error.
		return;
	}

	dock_widget->raise();
}

void EditorMainWindow::onExit(void)
{
	qApp->exit();
}

void EditorMainWindow::onTest(void)
{
	QLabel* const l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

	ads::CDockWidget* const dock_widget = new ads::CDockWidget("Label 1", this);
	dock_widget->setFeatures(ads::CDockWidget::DefaultDockWidgetFeatures | ads::CDockWidget::DockWidgetAlwaysCloseAndDelete);
	dock_widget->setWidget(l);

	_dock_manager->addDockWidgetFloating(dock_widget);

	QAction* const focus_action = new QAction(dock_widget->objectName(), this);
	_windows_menu->addAction(focus_action);

	connect(dock_widget, &ads::CDockWidget::closed, this, &EditorMainWindow::onWindowClosed);
	connect(focus_action, &QAction::triggered, this, &EditorMainWindow::onFocusWindow);

	_window_focus_action_map[dock_widget] = focus_action;
}
