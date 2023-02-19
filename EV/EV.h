#pragma once

#include _WITCH_PATH(WITCH.h)

#ifndef EV_set_backend
	#if defined(WOS_UNIX)
		#define EV_set_backend 0
	#elif defined(WOS_WINDOWS)
		#define EV_set_backend 1
	#else
		#error EV_set_backend is not defined
	#endif
#endif

#ifndef EV_set_ev
	#define EV_set_ev 1
#endif
#ifndef EV_set_tp
	#define EV_set_tp 1
#endif
#ifndef EV_set_io
	#define EV_set_io 1
#endif

#include _WITCH_PATH(EV/types.h)

#if EV_set_backend == 0
	#include _WITCH_PATH(EV/backend/0/0.h)
#elif EV_set_backend == 1
	#include _WITCH_PATH(EV/backend/1/1.h)
#else
	#error ?
#endif
