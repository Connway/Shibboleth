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

#include "Gaff_SpinLock.h"
#include "Gaff_Vector.h"
#include "Gaff_Queue.h"
#include "Gaff_Utils.h"
#include <atomic>
#include <thread>

NS_GAFF

struct JobData
{
	using JobFunc = void (*)(void*);

	JobData(JobFunc func = nullptr, void* data = nullptr):
		job_func(func), job_data(data)
	{
	}

	JobData(const JobData& job):
		job_func(job.job_func), job_data(job.job_data)
	{
	}

	const JobData& operator=(const JobData& job)
	{
		job_func = job.job_func;
		job_data = job.job_data;
		return *this;
	}

	JobFunc job_func;
	void* job_data;
};

struct Counter
{
	std::atomic_int32_t count;
};

using JobPair = std::pair<JobData, Counter*>;

template <class Allocator = DefaultAllocator>
class JobPool
{
public:
	JobPool(const Allocator& allocator = Allocator());
	~JobPool(void);

	bool init(unsigned int num_pools = 7, unsigned int num_threads = static_cast<unsigned int>(GetNumberOfCores()));
	void destroy(void);

	void addJobs(JobData* jobs, size_t num_jobs = 1, Counter** counter = nullptr, unsigned int pool = 0);
	void waitForAndFreeCounter(Counter* counter);
	void waitForCounter(Counter* counter);
	void freeCounter(Counter* counter);
	void helpWhileWaiting(Counter* counter);
	void helpAndFreeCounter(Counter* counter);

	void helpUntilNoJobs(void);
	void doAJob(void);

	int32_t getNumActiveThreads(void) const;
	size_t getNumTotalThreads(void) const;

	void getThreadIDs(size_t* out) const;

private:
	struct JobQueue
	{
		Queue<JobPair, Allocator> jobs;
		UniquePtr<SpinLock> read_write_lock;
		UniquePtr<SpinLock> thread_lock;
	};

	struct ThreadData
	{
		JobPool<Allocator>* job_pool;
		bool terminate;
	};

	Vector<JobQueue, Allocator> _job_pools;
	Vector<std::thread, Allocator> _threads;
	ThreadData _thread_data;

	Allocator _allocator;

	std::atomic_int32_t _active_threads;

	static void ProcessMainJobQueue(JobQueue& job_queue);
	static void ProcessJobQueue(JobQueue& job_queue);
	static void DoJob(JobPair& job);

	static void JobThread(ThreadData& td);

	GAFF_NO_COPY(JobPool);
	GAFF_NO_MOVE(JobPool);
};

#include "Gaff_JobPool.inl"

NS_END