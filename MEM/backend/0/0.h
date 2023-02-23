#include <string.h>

void MEM_copy(const void *src, void *dst, uintptr_t size){
  memcpy(dst, src, size);
}

void MEM_move(void *src, void *dst, const uintptr_t size){
  memmove(dst, src, size);
}

void MEM_set(const uint8_t src, void *dst, const uintptr_t size){
  memset(dst, src, size);
}

bool MEM_cmp(const void *src, const void *dst, const uintptr_t size){
  return !memcmp(dst, src, size);
}

bool MEM_ncmp(const void *src, const uintptr_t src_size, const void *dst, const uintptr_t dst_size){
  if(src_size < dst_size){
    return 0;
  }
  return !memcmp(dst, src, dst_size);
}

bool MEM_ncmpn(const void *src, const uintptr_t src_size, const void *dst, const uintptr_t dst_size){
  if(src_size != dst_size){
    return 0;
  }
  return !memcmp(dst, src, dst_size);
}

uint8_t *MEM_findmem(const void *src, const uintptr_t src_size, const void *dst, const uintptr_t dst_size){
  #if defined(_GNU_SOURCE)
    return (uint8_t *)memmem(src, src_size, dst, dst_size);
  #else
    const uint8_t *src_end = (const uint8_t *)src + src_size;
    while(((const uint8_t *)src + dst_size) < src_end){
      if(MEM_cmp(src, dst, dst_size)){
        return (uint8_t *)src;
      }
      src = (const void *)((const uint8_t *)src + 1);
    }
    return 0;
  #endif
}

uint8_t *MEM_findchr(const void *src, const uintptr_t src_size, const uint8_t c){
  return (uint8_t *)memchr(src, c, src_size);
}

uintptr_t MEM_findchru(const void *Data, const uintptr_t Size, const uint8_t c){
  const void *r = memchr(Data, c, Size);
  if(r){
    return (uint8_t *)r - (uint8_t *)Data;
  }
  return Size;
}

uint8_t *MEM_findchr0(const void *src, const uint8_t c){
  for(; *(uint8_t *)src != c; src = (const void *)((uint8_t *)src + 1));
  return (uint8_t *)src;
}

uintptr_t MEM_cstreu(const void *cstr){
  return (const uint8_t *)MEM_findchr0(cstr, 0) - (const uint8_t *)cstr;
}

uint8_t *MEM_cstrep(const void *cstr){
  return (uint8_t *)MEM_findchr0(cstr, 0);
}
