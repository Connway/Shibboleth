/************************************************************************************
Copyright (C) 2020 by Nicholas LaCroix

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
#include <EAThread/eathread_semaphore.h>
#include <EAThread/eathread_thread.h>
#include <EAThread/eathread_futex.h>
#include <EASTL/chrono.h>
#include <atomic>

NS_GAFF

struct JobData final
{
	using JobFunc = void (*)(uintptr_t, void*);

	JobFunc job_func;
	void* job_data;
};

using Counter = std::atomic_int32_t;
using JobPair = eastl::pair<JobData, Counter*>;
using ThreadInitFunc = void (*)(uintptr_t);

template <class Allocator = DefaultAllocator>
class JobPool
{
public:
	JobPool(const Allocator& allocator = Allocator());
	~JobPool(void);

	bool init(int32_t num_threads = static_cast<int32_t>(GetNumberOfCores()), ThreadInitFunc init = nullptr);
	void destroy(void);
	void run(void);

	void addPool(const HashStringTemp32<>& name, int32_t max_concurrent_threads = 1);

	void addJobs(const JobData* jobs, int32_t num_jobs = 1, Counter** counter = nullptr, Hash32 pool = 0);
	void addJobs(const JobData* jobs, int32_t num_jobs, Counter& counter, Hash32 pool = 0);
	void addJobsForAllThreads(const JobData* jobs, int32_t num_jobs, Counter** counter = nullptr);
	void addJobsForAllThreads(const JobData* jobs, int32_t num_jobs, Counter& counter);

	void waitForAndFreeCounter(Counter* counter);
	void waitForCounter(const Counter& counter);
	void freeCounter(Counter* counter);

	void helpWhileWaiting(EA::Thread::ThreadId thread_id, const Counter& counter);
	void helpWhileWaiting(const Counter& counter);
	void helpAndFreeCounter(EA::Thread::ThreadId thread_id, Counter* counter);
	void helpAndFreeCounter(Counter* counter);

	bool help(EA::Thread::ThreadId thread_id, eastl::chrono::milliseconds ms = eastl::chrono::milliseconds::zero());
	bool help(eastl::chrono::milliseconds ms = eastl::chrono::milliseconds::zero());
	void doAJob(EA::Thread::ThreadId thread_id);
	void doAJob(void);

	int32_t getNumTotalThreads(void) const;
	void getThreadIDs(EA::Thread::ThreadId* out) const;
	EA::Thread::ThreadId getMainThreadID(void) const;

private:
	struct JobQueue final
	{
		Queue<JobPair, Allocator> jobs;
		UniquePtr<EA::Thread::Futex, Allocator> read_write_lock;
		UniquePtr<EA::Thread::Semaphore, Allocator> thread_lock;
		//EA::Thread::Futex read_write_lock;
		//EA::Thread::Semaphore thread_lock;
	};

	struct ThreadData final
	{
		JobPool<Allocator>* job_pool = nullptr;
		ThreadInitFunc init_func = nullptr;
		bool terminate : 1;
		bool pause : 1;
	};

	VectorMap<HashString32<Allocator>, JobQueue, Allocator> _job_pools;
	JobQueue _main_queue;

	VectorMap<EA::Thread::ThreadId, JobQueue, Allocator> _per_thread_jobs;
	Vector<EA::Thread::Thread, Allocator> _threads;
	ThreadData _thread_data;
	EA::Thread::ThreadId _main_thread_id;

	Allocator _allocator;

	static bool ProcessJobQueue(JobQueue& job_queue, EA::Thread::ThreadId thread_id, eastl::chrono::milliseconds ms);
	static void DoJob(EA::Thread::ThreadId thread_id, JobPair& job);

	static intptr_t JobThread(void* data);

	GAFF_NO_COPY(JobPool);
	GAFF_NO_MOVE(JobPool);
};

#include "Gaff_JobPool.inl"

NS_END
