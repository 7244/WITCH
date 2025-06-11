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
  return _builtin_strlen(cstr);
}

static uint8_t *MEM_cstrep(
  const void *cstr
){
  return (uint8_t *)MEM_findchr0(cstr, 0);
}
