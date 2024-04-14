#pragma once

typedef struct EV_timer_t EV_timer_t;

typedef void (*EV_timer_cb_t)(EV_t *, EV_timer_t *);

#define EV_timer_common_di \
  VAS2_node_t node;

struct EV_timer_t{
  EV_timer_common_di
  uint64_t timeout;
  EV_timer_cb_t cb;
};
