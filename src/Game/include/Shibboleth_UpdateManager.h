/************************************************************************************
Copyright (C) 2015 by Nicholas LaCroix

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

#include <Shibboleth_ReflectionDefinitions.h>
#include <Shibboleth_RefCounted.h>
#include <Shibboleth_IManager.h>
#include <Shibboleth_Array.h>
#include <Shibboleth_ITask.h>
#include <Gaff_Function.h>
#include <Gaff_SpinLock.h>

NS_GAFF
	struct JobData;
	struct Counter;
NS_END

NS_SHIBBOLETH

using UpdateCallback = Gaff::FunctionBinder<void, double>;

struct UpdateData
{
	UpdateData(UpdateCallback& cb, double t):
		callback(cb), dt(t)
	{
	}

	const UpdateData& operator=(const UpdateData& rhs)
	{
		callback = rhs.callback;
		dt = rhs.dt;
		return *this;
	}

	UpdateCallback& callback;
	double dt;
};

class UpdateManager : public IManager
{
public:
	UpdateManager(void);
	~UpdateManager(void);

	const char* getName(void) const;
	void allManagersCreated(void);

	void update(double dt);

	void* rawRequestInterface(unsigned int class_id) const;

private:
	Array< Array<UpdateCallback> > _table;
	Array<Gaff::JobData> _job_data;
	Array<UpdateData> _update_data;

	Gaff::Counter* _counter;

	GAFF_NO_COPY(UpdateManager);
	GAFF_NO_MOVE(UpdateManager);

	SHIB_REF_DEF(UpdateManager);
};

NS_END
