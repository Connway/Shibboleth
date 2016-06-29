#include "Contrivance_DockWidgetWrapper.h"
#include "Contrivance_IContrivanceWindow.h"
#include "Contrivance_ExtensionSpawner.h"
#include <QAbstractButton>
#include <QDockWidget>
#include <QWindow>
#include <QAction>

DockWidgetWrapper::DockWidgetWrapper(IContrivanceWindow& window, QDockWidget* dw):
	_window(&window), _dw(dw)
{
	QAbstractButton* button = dw->findChild<QAbstractButton*>("qt_dockwidget_closebutton");
	connect(dw->toggleViewAction(), SIGNAL(triggered(bool)), SLOT(windowToggled(bool)));
	connect(dw, SIGNAL(visibilityChanged(bool)), SLOT(windowVisibilityChanged(bool)));
	connect(button, SIGNAL(clicked(bool)), SLOT(closeButtonClicked(bool)));

	_window->addSpawnedWindowMenuEntry(dw->toggleViewAction());
}

DockWidgetWrapper::~DockWidgetWrapper(void)
{
	if (_dw) {
		_window->removeSpawnedWindowMenuEntry(_dw->toggleViewAction());
	}
}

void DockWidgetWrapper::windowVisibilityChanged(bool visible)
{
	if (!visible && _dw->isFloating()) {
		_timer_id = startTimer(1);

	} else if (_timer_id > -1) {
		killTimer(_timer_id);
		_timer_id = -1;
	}
}

void DockWidgetWrapper::windowToggled(bool visible)
{
	if (!visible && _timer_id > -1) {
		killTimer(_timer_id);
		_timer_id = -1;
	}
}

void DockWidgetWrapper::closeButtonClicked(bool)
{
	QDockWidget* dw = _dw;
	_dw = nullptr;

	_window->removeSpawnedWindowMenuEntry(dw->toggleViewAction());
	_window->getExtensionSpawner()->destroyExtension(dw);

	delete dw;
}

void DockWidgetWrapper::timerEvent(QTimerEvent* event)
{
	Q_ASSERT(_timer_id == event->timerId());
	(void)(event);

	killTimer(_timer_id);
	_timer_id = -1;
	closeButtonClicked(true);
}
