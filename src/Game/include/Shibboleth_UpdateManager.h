/************************************************************************************
Copyright (C) 2014 by Nicholas LaCroix

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

#include <Shibboleth_IManager.h>
#include <Shibboleth_Array.h>
#include <Shibboleth_ITask.h>
#include <Gaff_Function.h>
#include <Gaff_SpinLock.h>

NS_SHIBBOLETH

class App;

class UpdateManager : public IManager
{
public:
	typedef const Gaff::FunctionBinder<void, double> UpdateCallback;

	struct UpdateID
	{
		unsigned int row;
		unsigned int id;
	};

	UpdateManager(App& app);
	~UpdateManager(void);

	const char* getName(void) const;

	UpdateID registerForUpdate(const UpdateCallback& callback, unsigned int row);
	void unregisterForUpdate(const UpdateID& id);

	void update(double dt);

private:
	typedef Gaff::Pair<unsigned int, UpdateCallback> RowEntry;

	struct UpdateRow
	{
		Array<RowEntry> callbacks;
		unsigned int next_id;
	};

	class UpdateTask : public ITask
	{
	public:
		UpdateTask(RowEntry& entry, double dt):
			_entry(entry), _dt(dt)
		{
		}

		void doTask(void)
		{
			_entry.second(_dt);
		}

	private:
		RowEntry& _entry;
		double _dt;
	};

	Array< Gaff::TaskPtr<ProxyAllocator> > _tasks_cache;

	Array<UpdateID> _unregister_queue;
	Array<UpdateRow> _table;
	App& _app;
	Gaff::SpinLock _unreg_queue_lock;

	void unregister(const UpdateID& id);

	GAFF_NO_COPY(UpdateManager);
	GAFF_NO_MOVE(UpdateManager);
};

NS_END
