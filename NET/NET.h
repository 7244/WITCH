#pragma once

#include <WITCH/STR/STR.h>

/* all backends must obey to this struct */
#pragma pack(push, 1)
typedef struct{
  uint32_t ip;
  uint16_t port;
}NET_addr_t;

typedef NET_addr_t NET_addr4port_t;

typedef struct{
  uint32_t ip;
  uint8_t prefix;
}NET_addr4prefix_t;

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

static uint32_t NET_ntoh32(uint32_t p){
  #if defined(__BYTE_ORDER_BIG)
    return p;
  #elif defined(__BYTE_ORDER_LITTLE)
    return byteswap32(p);
  #else
    #error ?
  #endif
}

static uint16_t NET_ntoh16(uint16_t p){
  #if defined(__BYTE_ORDER_BIG)
    return p;
  #elif defined(__BYTE_ORDER_LITTLE)
    return byteswap16(p);
  #else
    #error ?
  #endif
}

static uint32_t _NET_addr4_from_string(const void *str, uintptr_t *index){
  uint32_t ret;

  uint8_t i = 0;
  while(1){
    #if defined(__BYTE_ORDER_BIG)
      ((uint8_t *)&ret)[i] = STR_psu8_iguess_abort(str, index);
    #elif defined(__BYTE_ORDER_LITTLE)
      ((uint8_t *)&ret)[3 - i] = STR_psu8_iguess_abort(str, index);
    #else
      #error ?
    #endif

    if(i < 3 && ((uint8_t *)str)[*index] != '.'){
      __abort();
    }

    i++;
    if(i >= 4){
      break;
    }

    (*index)++;
  }

  return ret;
}

static uint32_t NET_ipv4_from_string(const void *str){
  uintptr_t index = 0;
  return _NET_addr4_from_string(str, &index);
}

static sint32_t NET_addr4prefix_from_string(const void *str, NET_addr4prefix_t *addr4prefix){
  uintptr_t index = 0;
  addr4prefix->ip = _NET_addr4_from_string(str, &index);
  addr4prefix->prefix = 32;

  if(((uint8_t *)str)[index] == '/'){
    index++;

    addr4prefix->prefix = STR_psu8_iguess_abort(str, &index);
  }

  if(addr4prefix->prefix > 32){
    return -1;
  }

  if(addr4prefix->prefix == 0){
    addr4prefix->ip = 0;
  }
  else{
    addr4prefix->ip &= ~(((uint32_t)1 << 32 - addr4prefix->prefix) - 1);
  }


  return 0;
}

static sint32_t NET_addr4port_from_string(const void* str, NET_addr4port_t* addr4port) {
  uintptr_t index = 0;
  addr4port->ip = _NET_addr4_from_string(str, &index);
  addr4port->port = 0;

  if (((uint8_t*)str)[index] == ':') {
    index++;

    addr4port->port = STR_psu16_iguess_abort(str, &index);
  }

  return 0;
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
