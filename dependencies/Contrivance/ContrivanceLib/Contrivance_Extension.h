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

#pragma once

#include "Contrivance_Defines.h"
#include <QWidget>

#define CONTRIVANCE_EXTENSION_DECLARATIONS \
	public: \
		QSize sizeHint(void) const override; \
		void setSizeHint(QSize size) override; \
		int getID(void) const override; \
		void setID(int id) override; \
		const QWidget* getWidget(void) const override; \
		QWidget* getWidget(void) override; \
	private: \
		QSize _size; \
		int _ext_id; \
		Contrivance::ExtensionSpawner* _spawner; \
	protected: \
		Contrivance::IContrivanceWindow& _window

#define CONTRIVANCE_EXTENSION_IMPLEMENATION(ClassName) \
	QSize ClassName::sizeHint(void) const \
	{ \
		return _size; \
	} \
	void ClassName::setSizeHint(QSize size) \
	{ \
		_size = size; \
	} \
	int ClassName::getID(void) const \
	{ \
		return _ext_id; \
	} \
	void ClassName::setID(int id) \
	{ \
		_ext_id = id; \
	} \
	const QWidget* ClassName::getWidget(void) const \
	{ \
		return this; \
	} \
	QWidget* ClassName::getWidget(void) \
	{ \
		return this; \
	}

#define CONTRIVANCE_EXTENSION_INITIALIZER_LIST \
	_ext_id(-1), _spawner(window.getExtensionSpawner()), \
	_window(window)

#define CONTRIVANCE_EXTENSION_DESTRUCTOR _spawner->addFreeID(_ext_id)

NS_CONTRIVANCE

class IContrivanceWindow;
class ExtensionSpawner;

class IExtension
{
public:
	IExtension(void) {}
	virtual ~IExtension(void) {}

	virtual QSize sizeHint(void) const = 0;
	virtual void setSizeHint(QSize size) = 0;

	virtual int getID(void) const = 0;
	virtual void setID(int id) = 0;

	virtual const QWidget* getWidget(void) const = 0;
	virtual QWidget* getWidget(void) = 0;
};

class Extension : public QWidget, public IExtension
{
	Q_OBJECT

public:
	Extension(IContrivanceWindow& window);
	~Extension(void);

	CONTRIVANCE_EXTENSION_DECLARATIONS;
};

NS_END
