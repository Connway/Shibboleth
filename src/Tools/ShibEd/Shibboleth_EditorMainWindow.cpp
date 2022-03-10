#include "Shibboleth_EditorMainWindow.h"
#include "ui_Shibboleth_EditorMainWindow.h"
#include <QLabel>

EditorMainWindow::EditorMainWindow(QWidget *parent):
	QMainWindow(parent)
{
	_ui = new Ui::EditorMainWindow;
	_ui->setupUi(this);

	_dock_manager = new ads::CDockManager(this);

	// Create example content label - this can be any application specific
	// widget
	QLabel* const l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

	// Create a dock widget with the title Label 1 and set the created label
	// as the dock widget content
	ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
	DockWidget->setWidget(l);

	// Add the toggleViewAction of the dock widget to the menu to give
	// the user the possibility to show the dock widget if it has been closed
	//_ui->menuView->addAction(DockWidget->toggleViewAction());

	// Add the dock widget to the top dock widget area
	_dock_manager->addDockWidget(ads::TopDockWidgetArea, DockWidget);
}

EditorMainWindow::~EditorMainWindow()
{
	delete _ui;
}
