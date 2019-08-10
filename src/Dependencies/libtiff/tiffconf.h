#pragma once

#if defined(_WIN32) || defined(_WIN64)
	//#include "tiffconf.h.win"
	#include "tiffconf.vc.h"
#elif __linux__
	//#include "tiffconf.h.linux"
#elif __APPLE__
	//#include "tiffconf.h.mac"
#else
	#error Platform not supported
#endif
