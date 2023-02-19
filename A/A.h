#pragma once

#include _WITCH_PATH(PR/PR.h)

#if WITCH_LIBC
	#include <malloc.h>
#elif defined(WITCH_PLATFORM_linux_kernel_module)
	#include <linux/slab.h>
#endif

typedef uint8_t *(*A_resize_t)(void *ptr, uintptr_t size);

#ifndef A_set_buffer
	#define A_set_buffer 512
#endif
uintptr_t _A_calculate_buffer(uintptr_t size){
	uintptr_t r = A_set_buffer / size;
	if(!r){
		return 1;
	}
	return r;
}

uint8_t *A_resize(void *ptr, uintptr_t size){
	#if WITCH_LIBC
		if(ptr){
			if(size){
				void *rptr = (void *)realloc(ptr, size);
				if(rptr == 0){
					PR_abort();
				}
				return (uint8_t *)rptr;
			}
			else{
				free(ptr);
				return 0;
			}
		}
		else{
			if(size){
				void *rptr = (void *)malloc(size);
				if(rptr == 0){
					PR_abort();
				}
				return (uint8_t *)rptr;
			}
			else{
				return 0;
			}
		}
	#elif defined(WITCH_PLATFORM_linux_kernel_module)
		if(ptr){
			if(size){
				void *rptr = krealloc(ptr, size, GFP_KERNEL);
				if(rptr == 0){
					PR_abort();
				}
				return (uint8_t *)rptr;
			}
			else{
				kfree(ptr);
				return 0;
			}
		}
		else{
			if(size){
				void *rptr = kmalloc(size, GFP_KERNEL);
				if(rptr == 0){
					PR_abort();
				}
				return (uint8_t *)rptr;
			}
			else{
				return 0;
			}
		}
	#else
		#error ?
	#endif
}
