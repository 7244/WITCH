#pragma once

/*
	0 xorg
		0 xlib
		1 xcb
	1 windows
*/

#ifndef MD_set_backend
	#if defined(WOS_UNIX)
		#define MD_set_backend 0
	#elif defined(WOS_WINDOWS)
		#define MD_set_backend 1
	#endif
#endif

#ifndef MD_API_set_backend
	#if MD_set_backend == 0
		#define MD_API_set_backend 1
	#endif
#endif

#if MD_set_backend == 0
	#if MD_API_set_backend == 1
		#include <xcb/xcb.h>

		xcb_screen_t *_MD_GetScreenByIndex(const xcb_setup_t *setup, uint32_t screen_num){
			xcb_screen_iterator_t it = xcb_setup_roots_iterator(setup);
			while(it.rem > 0){
				if(!screen_num){
					return it.data;
				}
				screen_num--;
				xcb_screen_next(&it);
			}
			return 0;
		}
	#endif
#endif
