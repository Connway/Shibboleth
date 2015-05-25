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
#include <Shibboleth_IManager.h>
#include <Shibboleth_Array.h>
#include <Gaff_Function.h>
#include <Gaff_SpinLock.h>
#include <Gaff_Timer.h>

NS_GAFF
	struct JobData;
	struct Counter;
NS_END

NS_SHIBBOLETH

class FrameManager;

using UpdateCallback = Gaff::FunctionBinder<void, double, void*>;

class UpdateManager : public IManager
{
public:
	UpdateManager(void);
	~UpdateManager(void);

	const char* getName(void) const;
	void allManagersCreated(void);

	void update(void);

	void* rawRequestInterface(unsigned int class_id) const;

private:
	class UpdatePhase
	{
	public:
		UpdatePhase(void);
		~UpdatePhase(void);

		const char* getName(void) const;
		void setName(const char* name);

		void addUpdate(size_t row, const UpdateCallback& callback);
		void setNumRows(size_t num_rows);

		size_t getID(void) const;
		void setID(size_t id);

		bool isDone(void) const;
		bool grab(void);
		void run(void);

	private:
		struct UpdateData
		{
			UpdateCallback* callback;
			void* frame_data;
			double delta_time;
			size_t phase_id;
		};

		Array< Array<UpdateCallback> > _callbacks;
		Array<UpdateData> _data_cache;
		Array<Gaff::JobData> _jobs;
		Gaff::Timer _timer;
		AString _name;
		Gaff::Counter* _counter;

		FrameManager& _frame_mgr;

		size_t _id;

		Gaff::SpinLock _grab_lock;

		static void UpdatePhaseJob(void* data);
		static void UpdateJob(void* data);

		GAFF_NO_COPY(UpdatePhase);
		GAFF_NO_MOVE(UpdatePhase);
	};

	Array<UpdatePhase> _phases;

	GAFF_NO_COPY(UpdateManager);
	GAFF_NO_MOVE(UpdateManager);

	SHIB_REF_DEF(UpdateManager);
};

NS_END
