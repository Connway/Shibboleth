/************************************************************************************
Copyright (C) 2019 by Nicholas LaCroix

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

#include "Gaff_SmartPtrs.h"
#include "Gaff_Assert.h"
#include "Gaff_Vector.h"
#include "Gaff_Queue.h"
#include "Gaff_Utils.h"
#include <EASTL/chrono.h>
#include <atomic>
#include <thread>
#include <mutex>

NS_GAFF

struct JobData
{
	using JobFunc = void (*)(void*);

	JobFunc job_func;
	void* job_data;
};

using Counter = std::atomic_int32_t;
using JobPair = std::pair<JobData, Counter*>;
using ThreadInitFunc = void (*)(void);

template <class Allocator = DefaultAllocator>
class JobPool
{
public:
	JobPool(const Allocator& allocator = Allocator());
	~JobPool(void);

	bool init(int32_t num_pools = 7, int32_t num_threads = static_cast<int32_t>(GetNumberOfCores()), ThreadInitFunc init = nullptr);
	void destroy(void);

	void addJobs(JobData* jobs, size_t num_jobs = 1, Counter** counter = nullptr, int32_t pool = 0);
	void addJobs(JobData* jobs, size_t num_jobs, Counter& counter, int32_t pool = 0);
	void waitForAndFreeCounter(Counter* counter);
	void waitForCounter(const Counter* counter);
	void freeCounter(Counter* counter);
	void helpWhileWaiting(const Counter* counter);
	void helpAndFreeCounter(Counter* counter);

	void help(eastl::chrono::milliseconds ms = eastl::chrono::milliseconds::zero());
	void doAJob(void);

	size_t getNumTotalThreads(void) const;
	void getThreadIDs(size_t* out) const;

private:
	struct JobQueue
	{
		Queue<JobPair, Allocator> jobs;
		UniquePtr<std::mutex, Allocator> read_write_lock;
		UniquePtr<std::mutex, Allocator> thread_lock;
		//std::mutex read_write_lock;
		//std::mutex thread_lock;
	};

	struct ThreadData
	{
		JobPool<Allocator>* job_pool;
		ThreadInitFunc init_func;
		bool terminate;
	};

	Vector<JobQueue, Allocator> _job_pools;
	Vector<std::thread, Allocator> _threads;
	ThreadData _thread_data;

	Allocator _allocator;

	static bool ProcessJobQueue(JobQueue& job_queue, eastl::chrono::milliseconds ms);
	static void DoJob(JobPair& job);

	static void JobThread(ThreadData& thread_data);

	GAFF_NO_COPY(JobPool);
	GAFF_NO_MOVE(JobPool);
};

#include "Gaff_JobPool.inl"

NS_END
