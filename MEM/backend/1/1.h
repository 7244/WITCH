/* TODO add restrict */
static void MEM_copy(
  const void *src,
  void *dst,
  uintptr_t size
){
  const uint8_t *_src = (const uint8_t *)src;
  uint8_t *_dst = (uint8_t *)dst;
  while(size--){
    *_dst++ = *_src++;
  }
}

static void MEM_move(
  void *src,
  void *dst,
  uintptr_t size
){
  const uint8_t *_src = (const uint8_t *)src;
  uint8_t *_dst = (uint8_t *)dst;
  uint8_t b;
  if(size){
    b = *_src++;
  }
  while(size--){
    uint8_t t;
    if(size){
      t = *_src++;
    }
    *_dst++ = b;
    b = t;
  }
}

static void MEM_set(
  uint8_t src,
  void *dst,
  uintptr_t size
){
  for(uintptr_t isize = 0; isize < size; isize++){
    ((uint8_t *)dst)[isize] = src;
  }
}

static bool MEM_cmp(
  const void *src,
  const void *dst,
  uintptr_t size
){
  for(uintptr_t isize = 0; isize < size; isize++){
    if(((uint8_t *)dst)[isize] != ((uint8_t *)src)[isize]){
      return 1;
    }
  }
  return 0;
}

static bool MEM_ncmp(
  const void *src,
  uintptr_t src_size,
  const void *dst,
  uintptr_t dst_size
){
  if(src_size < dst_size){
    return 0;
  }
  for(uintptr_t isize = 0; isize < dst_size; isize++){
    if(((uint8_t *)dst)[isize] != ((uint8_t *)src)[isize]){
      return 1;
    }
  }
  return 0;
}

static bool MEM_ncmpn(
  const void *src,
  uintptr_t src_size,
  const void *dst,
  uintptr_t dst_size
){
  if(src_size != dst_size){
    return 0;
  }
  for(uintptr_t isize = 0; isize < dst_size; isize++){
    if(((uint8_t *)dst)[isize] != ((uint8_t *)src)[isize]){
      return 1;
    }
  }
  return 0;
}

static const void *MEM_findmem(
  const void *src,
  uintptr_t src_size,
  const void *dst,
  uintptr_t dst_size
){
  const uint8_t *src_end = (const uint8_t *)src + src_size;
  while(((const uint8_t *)src + dst_size) < src_end){
    if(!MEM_cmp(src, dst, dst_size)){
      return src;
    }
    src = (const void *)((const uint8_t *)src + 1);
  }

  return NULL;
}

static uint8_t *MEM_findchr(
  const void *src,
  uintptr_t src_size,
  uint8_t c
){
  const uint8_t *_src = (const uint8_t *)src;
  const uint8_t *_src_limit = &_src[src_size];
  while(_src != _src_limit){
    if(*_src == c){
      return (uint8_t *)_src;
    }
    _src++;
  }
  return 0;
}

static uint8_t *MEM_findchr0(
  const void *src,
  uint8_t c
){
  for(; *(uint8_t *)src != c; src = (const void *)((uint8_t *)src + 1));
  return (uint8_t *)src;
}

static uintptr_t MEM_cstreu(
  const void *cstr
){
  #if 0
    /* compilers cant understand how to optimize this */
    return (uintptr_t)MEM_findchr0(cstr, 0) - (uintptr_t)cstr;
  #elif 0
    /* compilers trying to change this to strlen when there is no libc */
    return __builtin_strlen(cstr);
  #else
    uintptr_t i = 0;
    while(EXPECT(*((const uint8_t *)cstr + i), 1)){
      i++;
    }
    return i;
  #endif
}

static uint8_t *MEM_cstrep(
  const void *cstr
){
  return (uint8_t *)MEM_findchr0(cstr, 0);
}
