#pragma once

typedef struct EV_event_t EV_event_t;

typedef void (*EV_event_cb_t)(EV_t *, EV_event_t *, uint32_t);

void _EV_event_cb(struct ev_loop *, ev_io *, int);

#define EV_event_common_di \
  ev_io ev;
#define EV_event_all_di \
  EV_event_common_di \
  EV_event_cb_t cb;

struct EV_event_t{
  EV_event_all_di
};
