/************************************************************************************
Copyright (C) 2017 by Nicholas LaCroix

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

#include "Gaff_Defines.h"

NS_GAFF

template <class T>
class RefPtr
{
public:
	explicit RefPtr(T* data = nullptr):
		_data(data)
	{
		if (_data) {
			_data->addRef();
		}
	}

	RefPtr(const RefPtr<T>& rhs):
		_data(rhs._data)
	{
		if (_data) {
			_data->addRef();
		}
	}

	~RefPtr(void)
	{
		SAFEGAFFRELEASE(_data);
	}

	bool operator==(const RefPtr<T>& rhs) const
	{
		return _data == rhs._data;
	}

	bool operator==(const T* rhs) const
	{
		return _data == rhs;
	}

	bool operator!=(const RefPtr<T>& rhs) const
	{
		return _data != rhs._data;
	}

	bool operator!=(const T* rhs) const
	{
		return _data != rhs;
	}

	bool operator<(const T* rhs) const
	{
		return _data < rhs;
	}

	bool operator>(const T* rhs) const
	{
		return _data > rhs;
	}

	const RefPtr<T>& operator=(const RefPtr<T>& rhs)
	{
		SAFEGAFFRELEASE(_data);
		_data = rhs._data;
		SAFEGAFFADDREF(_data);
		return *this;
	}

	const RefPtr<T>& operator=(T* rhs)
	{
		SAFEGAFFRELEASE(_data);
		_data = rhs;
		SAFEGAFFADDREF(_data);
		return *this;
	}

	const T* operator->(void) const
	{
		return _data;
	}

	T* operator->(void)
	{
		return _data;
	}

	const T& operator*(void) const
	{
		return *_data;
	}

	T& operator*(void)
	{
		return *_data;
	}

	operator bool(void) const
	{
		return _data != nullptr;
	}

	const T* get(void) const
	{
		return _data;
	}

	T* get(void)
	{
		return _data;
	}

	/*!
		\brief Sets the internal pointer without incrementing the reference count.
	*/
	void set(T* data)
	{
		_data = data;
	}

	RefPtr(RefPtr<T>&& rhs):
		_data(rhs._data)
	{
		rhs._data = nullptr;
	}

	const RefPtr<T>& operator=(RefPtr<T>&& rhs)
	{
		SAFEGAFFRELEASE(_data);
		_data = rhs._data;
		rhs._data = nullptr;
		return *this;
	}

private:
	T* _data;
};



template <class T>
class COMRefPtr
{
public:
	COMRefPtr(T* data = nullptr):
		_data(data)
	{
		if (_data) {
			_data->AddRef();
		}
	}

	COMRefPtr(const COMRefPtr<T>& rhs):
		_data(rhs._data)
	{
		if (_data) {
			_data->AddRef();
		}
	}

	~COMRefPtr(void)
	{
		SAFERELEASE(_data);
	}

	bool operator==(const COMRefPtr<T>& rhs) const
	{
		return _data == rhs._data;
	}

	bool operator==(const T* rhs) const
	{
		return _data == rhs;
	}

	bool operator!=(const COMRefPtr<T>& rhs) const
	{
		return _data != rhs._data;
	}

	bool operator!=(const T* rhs) const
	{
		return _data != rhs;
	}

	const COMRefPtr<T>& operator=(const COMRefPtr<T>& rhs)
	{
		SAFERELEASE(_data);
		_data = rhs._data;
		SAFEADDREF(_data);
		return *this;
	}

	const COMRefPtr<T>& operator=(T* rhs)
	{
		SAFERELEASE(_data);
		_data = rhs;
		SAFEADDREF(_data);
		return *this;
	}

	const T* operator->(void) const
	{
		return _data;
	}

	T* operator->(void)
	{
		return _data;
	}

	const T& operator*(void) const
	{
		return *_data;
	}

	T& operator*(void)
	{
		return *_data;
	}

	operator bool(void) const
	{
		return _data != nullptr;
	}

	const T* get(void) const
	{
		return _data;
	}

	T* get(void)
	{
		return _data;
	}

	/*!
		\brief Sets the internal pointer without incrementing the reference count.
	*/
	void set(T* data)
	{
		_data = data;
	}

	COMRefPtr(COMRefPtr<T>&& rhs):
		_data(rhs._data)
	{
		rhs._data = nullptr;
	}

	const COMRefPtr<T>& operator=(COMRefPtr<T>&& rhs)
	{
		SAFERELEASE(_data);
		_data = rhs._data;
		rhs._data = nullptr;
		return *this;
	}

private:
	T* _data;
};

NS_END
