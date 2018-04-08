#pragma once

#if defined(_WIN32) || defined(_WIN64)
	#include "tif_config.h.win"
#elif __linux__
	#include "tif_config.h.linux"
#elif __APPLE__
	#include "tif_config.h.mac"
#else
	#error Platform not supported
#endif
