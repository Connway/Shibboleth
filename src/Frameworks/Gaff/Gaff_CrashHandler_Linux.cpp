/************************************************************************************
Copyright (C) 2018 by Nicholas LaCroix

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

#include "Gaff_CrashHandler.h"

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
#include <sys/resource.h>
#include <csignal>
#include <err.h>

NS_GAFF

extern CrashHandler g_crash_handler;

struct CrashData
{
	int sig;
	siginfo_t siginfo;
	void* context;
};

//static uint8_t g_alternate_stack[SIGSTKSZ] = { 0 };
static uint8_t* g_alternate_stack = nullptr;
static CrashData g_crash_data;

static void ExceptionHandler(int sig, siginfo_t* siginfo, void* context)
{
	// Convert to our data structure

	g_crash_data.sig = sig;
	g_crash_data.siginfo = *siginfo;
	g_crash_data.context = context;

	if (g_crash_handler) {
		g_crash_handler(&g_crash_data);
	}
}

void DefaultCrashHandler(void*)
{
}

void InitializeCrashHandler(void)
{
	/* setup alternate stack */
	stack_t ss = {};
	/* malloc is usually used here, I'm not 100% sure my static allocation
	is valid but it seems to work just fine. */
	ss.ss_sp = static_cast<void*>(g_alternate_stack);
	ss.ss_size = SIGSTKSZ;
	ss.ss_flags = 0;

	if (sigaltstack(&ss, NULL)) {
		err(1, "sigaltstack");
	}

	/* register our signal handlers */
	struct sigaction sig_action = {};
	sig_action.sa_sigaction = ExceptionHandler;
	sigemptyset(&sig_action.sa_mask);

#ifdef PLATFORM_MAC
	/* for some reason we backtrace() doesn't work on osx
	when we use an alternate stack */
	sig_action.sa_flags = SA_SIGINFO;
#else
	sig_action.sa_flags = SA_SIGINFO | SA_ONSTACK;
#endif

	if (sigaction(SIGSEGV, &sig_action, NULL)) {
		err(1, "sigaction");
	}

	if (sigaction(SIGFPE, &sig_action, NULL)) {
		err(1, "sigaction");
	}

	if (sigaction(SIGINT, &sig_action, NULL)) {
		err(1, "sigaction");
	}

	if (sigaction(SIGILL, &sig_action, NULL)) {
		err(1, "sigaction");
	}

	if (sigaction(SIGTERM, &sig_action, NULL)) {
		err(1, "sigaction");
	}

	if (sigaction(SIGABRT, &sig_action, NULL)) {
		err(1, "sigaction");
	}

	// allow core dumps
	rlimit core_limits;
	core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &core_limits);

	g_alternate_stack = new uint8_t[SIGSTKSZ];
}

void UninitializeCrashHandler(void)
{
	delete[] g_alternate_stack;
	g_alternate_stack = nullptr;
}

NS_END

#endif
