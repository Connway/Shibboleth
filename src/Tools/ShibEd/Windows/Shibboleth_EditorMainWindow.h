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

#pragma once

#include <QMainWindow>
#include <DockManager.h>

class EditorMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	EditorMainWindow(QWidget* parent = nullptr);
	~EditorMainWindow();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	ads::CDockManager* _dock_manager = nullptr;
	QMenu* _windows_menu = nullptr;

	QMap<ads::CDockWidget*, QAction*> _window_focus_action_map;

	void retranslateUi(void);

	void onWindowClosed(void);
	void onFocusWindow(void);
	void onExit(void);

	void onTest(void);
};
