typedef struct EV_timer_t EV_timer_t;

typedef void (*EV_timer_cb_t)(EV_t *, EV_timer_t *);

#define EV_timer_common_di \
  ev_timer ev;
#define EV_timer_all_di \
  EV_timer_common_di \
  EV_timer_cb_t cb;

struct EV_timer_t{
  EV_timer_all_di
};
