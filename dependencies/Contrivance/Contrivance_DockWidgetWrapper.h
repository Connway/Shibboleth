#ifndef DockWidgetWrapper_H
#define DockWidgetWrapper_H

#include <QObject>

class ContrivanceWindow;
class QDockWidget;

class DockWidgetWrapper : public QObject, public QObjectUserData
{
	Q_OBJECT

public:
	DockWidgetWrapper(ContrivanceWindow& window, QDockWidget* dw);
	~DockWidgetWrapper(void);

private slots:
	void windowVisibilityChanged(bool visible);
	void windowToggled(bool visible);
	void closeButtonClicked(bool);

private:
	ContrivanceWindow& _window;
	QDockWidget* _dw = nullptr;
	int _timer_id = -1;

	void timerEvent(QTimerEvent *event) override;
};

#endif // DockWidgetWrapper_H
