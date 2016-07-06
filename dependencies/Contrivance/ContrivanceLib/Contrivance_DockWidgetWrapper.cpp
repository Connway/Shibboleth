/************************************************************************************
Copyright (C) 2016 by Nicholas LaCroix

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

#include "Contrivance_DockWidgetWrapper.h"
#include "Contrivance_IContrivanceWindow.h"
#include "Contrivance_ExtensionSpawner.h"
#include <QAbstractButton>
#include <QDockWidget>
#include <QWindow>
#include <QAction>

NS_CONTRIVANCE

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

NS_END
