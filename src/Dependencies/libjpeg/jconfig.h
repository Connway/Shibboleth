#pragma once

#if defined(_WIN32) || defined(_WIN64)
	#include "jconfig.vc"
#elif __linux__
	#include "jconfig.cfg"
#elif __APPLE__
	#include "jconfig.mac"
#else
	#error Platform not supported
#endif
