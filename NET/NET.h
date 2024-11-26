#pragma once

/* all backends must obey to this struct */
#pragma pack(push, 1)
typedef struct{
  uint32_t ip;
  uint16_t port;
}NET_addr_t;
#pragma pack(pop)

#ifndef NET_set_backend
  #if defined(__platform_unix_linux)
    #define NET_set_backend 0
  #elif defined(__platform_windows)
    #define NET_set_backend 1
  #elif defined(__platform_unix_freebsd)
    #define NET_set_backend 2
  #elif defined(__platform_linux_kernel_module)
    #define NET_set_backend 3
  #else
    #error NET_set_backend is not defined
  #endif
#endif

#if NET_set_backend == 0
  #include _WITCH_PATH(NET/backend/0/0.h)
#elif NET_set_backend == 1
  #include _WITCH_PATH(NET/backend/1/1.h)
#elif NET_set_backend == 2
  #include _WITCH_PATH(NET/backend/2/2.h)
#elif NET_set_backend == 3
  #include _WITCH_PATH(NET/backend/linux_kernel_module/linux_kernel_module.h)
#else
  #error ?
#endif
