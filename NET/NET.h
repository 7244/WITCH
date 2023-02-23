#pragma once

/* all backends must obey to this struct */
#pragma pack(push, 1)
typedef struct{
  uint32_t ip;
  uint16_t port;
}NET_addr_t;
#pragma pack(pop)

#ifndef NET_set_backend
  #if defined(WOS_UNIX_LINUX)
    #define NET_set_backend 0
  #elif defined(WOS_WINDOWS)
    #define NET_set_backend 1
  #elif defined(WOS_UNIX_BSD)
    #define NET_set_backend 2
  #elif defined(WITCH_PLATFORM_linux_kernel_module)
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
