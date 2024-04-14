#pragma once

#include <uv.h>

typedef struct EV_event_t EV_event_t;

typedef void (*EV_event_cb_t)(EV_t *, EV_event_t *, uint32_t);

#if defined(WOS_WINDOWS)
  #define EV_event_common_di \
    IO_fd_t fd; \
    uint32_t evflag; \
    union{ \
      uint8_t ListenObjectsID; \
    };
#else
  #define EV_event_common_di \
    IO_fd_t fd; \
    uint32_t evflag;
#endif

struct EV_event_t{
  EV_event_common_di
  EV_event_cb_t cb;
};
