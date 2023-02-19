#pragma once

#ifndef FS_set_backend
	#if defined(WOS_UNIX_LINUX)
		#define FS_set_backend 0
	#elif defined(WOS_WINDOWS)
		#define FS_set_backend 1
	#elif defined(WOS_UNIX_BSD)
		#define FS_set_backend 2
	#else
		#error FS_set_backend is not defined
	#endif
#endif

#if FS_set_backend == 0
	#include _WITCH_PATH(FS/backend/0/0.h)
#elif FS_set_backend == 1
	#include _WITCH_PATH(FS/backend/1/1.h)
#elif FS_set_backend == 2
	#include _WITCH_PATH(FS/backend/2/2.h)
#else
	#error ?
#endif
