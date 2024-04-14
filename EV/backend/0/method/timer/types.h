#pragma once

typedef struct EV_timer_t EV_timer_t;

typedef void (*EV_timer_cb_t)(EV_t *, EV_timer_t *);

void _EV_timer_cb(struct ev_loop *, ev_timer *, int);

#define EV_timer_common_di \
  ev_timer ev;
#define EV_timer_all_di \
  EV_timer_common_di \
  EV_timer_cb_t cb;

struct EV_timer_t{
  EV_timer_all_di
};
