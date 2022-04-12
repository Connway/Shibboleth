/* zip.h -- Compatibility layer shim
   part of the minizip-ng project

   This program is distributed under the terms of the same license as zlib.
   See the accompanying LICENSE file for the full text of the license.
*/
#ifndef MZ_COMPAT_ZIP
#define MZ_COMPAT_ZIP

#if defined(ZLIB_COMPAT)
	#include <zlib.h>
#endif

#include "mz_compat.h"

#endif
