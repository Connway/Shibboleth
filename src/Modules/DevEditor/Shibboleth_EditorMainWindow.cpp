/************************************************************************************
Copyright (C) 2022 by Nicholas LaCroix

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
************************************************************************************/

#include "Shibboleth_EditorMainWindow.h"
#include "Shibboleth_EditorAttributesCommon.h"
#include <Shibboleth_Utilities.h>
#include <Shibboleth_IApp.h>
#include <QCoreApplication>
#include <QStatusBar>
#include <QMenuBar>
#include <QLabel>

namespace
{
	static Shibboleth::ProxyAllocator s_allocator("Editor");
}

EditorMainWindow::EditorMainWindow(QWidget *parent):
	QMainWindow(parent)
{
	Shibboleth::IApp& app = Shibboleth::GetApp();
	Shibboleth::ReflectionManager& refl_mgr = app.getReflectionManager();

	refl_mgr.registerAttributeBucket<Shibboleth::EditorWindowAttribute>();

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

	const auto* const wnd_bucket = refl_mgr.getAttributeBucket<Shibboleth::EditorWindowAttribute>();

	if (wnd_bucket) {
		for (const Refl::IReflectionDefinition* ref_def : *wnd_bucket) {
			const char8_t* name = ref_def->getFriendlyName();

			if (!name || !eastl::CharStrlen(name)) {
				name = ref_def->getReflectionInstance().getName();
			}

			const Shibboleth::EditorWindowAttribute* const attr = ref_def->getClassAttr<Shibboleth::EditorWindowAttribute>();
			GAFF_ASSERT(attr);

			QAction* const create_window_action = new QAction(name, this);
			window_menu->addAction(create_window_action);

			connect(
				create_window_action,
				&QAction::triggered,
				[this, ref_def, attr, name]() -> void { createFloatingWindow(*ref_def, *attr, name); }
			);
		}
	}

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

bool EditorMainWindow::isWindowAlreadyOpen(const char8_t* name)
{
	GAFF_REF(name);
	return false;
}

void EditorMainWindow::createFloatingWindow(
	const Refl::IReflectionDefinition& ref_def,
	const Shibboleth::EditorWindowAttribute& attr,
	const char8_t* name)
{
	if (attr.isSingleInstance() && isWindowAlreadyOpen(name)) {
		return;
	}

	QWidget* const widget = ref_def.createT<QWidget, QWidget*>(
		CLASS_HASH(QWidget),
		ARG_HASH(QWidget*),
		s_allocator,
		this
	);

	if (!widget) {
		// $TODO: Log error.
		return;
	}

	ads::CDockWidget* const dock_widget = new ads::CDockWidget(name, this);
	dock_widget->setFeatures(ads::CDockWidget::DefaultDockWidgetFeatures | ads::CDockWidget::DockWidgetAlwaysCloseAndDelete);
	dock_widget->setWidget(widget);
	dock_widget->resize(400, 300);

	_dock_manager->addDockWidgetFloating(dock_widget);

	QAction* const focus_action = new QAction(dock_widget->objectName(), this);
	_windows_menu->addAction(focus_action);

	connect(dock_widget, &ads::CDockWidget::closed, this, &EditorMainWindow::onWindowClosed);
	connect(focus_action, &QAction::triggered, this, &EditorMainWindow::onFocusWindow);

	_window_focus_action_map[dock_widget] = focus_action;
}
