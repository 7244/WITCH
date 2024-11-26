#pragma once

#if defined(__platform_libc)
  #include <malloc.h>
#elif defined(__platform_linux_kernel_module)
  #include <linux/slab.h>
#endif

typedef uint8_t *(*A_resize_t)(void *ptr, uintptr_t size);

#ifndef A_set_buffer
  #define A_set_buffer 512
#endif
static uintptr_t _A_calculate_buffer(uintptr_t size){
  uintptr_t r = A_set_buffer / size;
  if(!r){
    return 1;
  }
  return r;
}

static uint8_t *A_resize(void *ptr, uintptr_t size){
  #if defined(__platform_libc)
    if(ptr){
      if(size){
        void *rptr = (void *)realloc(ptr, size);
        if(rptr == 0){
          __abort();
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
          __abort();
        }
        return (uint8_t *)rptr;
      }
      else{
        return 0;
      }
    }
  #elif defined(__platform_linux_kernel_module)
    if(ptr){
      if(size){
        void *rptr = krealloc(ptr, size, GFP_KERNEL);
        if(rptr == 0){
          __abort();
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
          __abort();
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
