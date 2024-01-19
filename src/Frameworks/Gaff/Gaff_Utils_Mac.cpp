/************************************************************************************
Copyright (C) 2024 by Nicholas LaCroix

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

#ifdef __APPLE__

#include "Gaff_Utils.h"
#include <malloc/malloc.h>
#include <sys/sysctl.h>
#include <unistd.h>

NS_GAFF

bool IsDebuggerAttached(void)
{
	int items[4];
	struct kinfo_proc info;
	size_t size = sizeof(info);

	info.kp_proc.p_flag = 0;
	items[0] = CTL_KERN;
	items[1] = KERN_PROC;
	items[2] = KERN_PROC_PID;
	items[3] = getpid();

	sysctl(items, sizeof(items) / sizeof(*items), &info, &size, nullptr, 0);

	return (info.kp_proc.p_flag & P_TRACED) != 0;
}

size_t GetUsableSize(void* ptr)
{
	return malloc_size(ptr);
}

NS_END

#endif
