#pragma once

#ifndef MEM_set_backend
  #if defined(__platform_libc)
    #define MEM_set_backend 0
  #else
    #define MEM_set_backend 1
  #endif
#endif

#if MEM_set_backend == 0
  #include _WITCH_PATH(MEM/backend/0/0.h)
#elif MEM_set_backend == 1
  #include _WITCH_PATH(MEM/backend/1/1.h)
#else
  #error ?
#endif

/* use this function only if you know the character is exists */
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
