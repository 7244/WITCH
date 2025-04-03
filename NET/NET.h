#pragma once

/* all backends must obey to this struct */
#pragma pack(push, 1)
typedef struct{
  uint32_t ip;
  uint16_t port;
}NET_addr_t;

typedef NET_addr_t NET_addr4port_t;

typedef struct{
  #if defined(__BYTE_ORDER_BIG)
    uint8_t version:4, ihl:4;
  #elif defined(__BYTE_ORDER_LITTLE)
    uint8_t ihl:4, version:4;
  #else
    #error ?
  #endif
  uint8_t tos;
  uint16_t tot_len;
  uint16_t id;
  uint16_t frag_off;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t check;
  uint32_t saddr;
  uint32_t daddr;
}NET_ipv4hdr_t;

typedef struct{
  uint16_t source;
  uint16_t dest;
  uint16_t len;
  uint16_t check;
}NET_udphdr_t;

#pragma pack(pop)

static uint32_t NET_hton32(uint32_t p){
  #if defined(__BYTE_ORDER_BIG)
    return p;
  #elif defined(__BYTE_ORDER_LITTLE)
    return byteswap32(p);
  #else
    #error ?
  #endif
}

static uint16_t NET_hton16(uint16_t p){
  #if defined(__BYTE_ORDER_BIG)
    return p;
  #elif defined(__BYTE_ORDER_LITTLE)
    return byteswap16(p);
  #else
    #error ?
  #endif
}

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
