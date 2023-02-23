#include _WITCH_PATH(EV/backend/1/method/event/types.h)

typedef struct EV_idle_t EV_idle_t;
typedef void (*EV_idle_cb_t)(EV_t *, EV_idle_t *);

struct EV_idle_t{
  uv_idle_t ev;
  EV_idle_cb_t cb;
};

void _EV_idle_cb(uv_idle_t *handle){
  EV_t *listener = (EV_t *)uv_loop_get_data(handle->loop);
  EV_idle_t *idle = OFFSETLESS(handle, EV_idle_t, ev);
  idle->cb(listener, idle);
}

void _EV_idle_start(EV_t *listener, EV_idle_t *idle){
  uv_idle_init(listener->loop, &idle->ev);
  uv_idle_start(&idle->ev, _EV_idle_cb);
}
void EV_idle_start(EV_t *listener, EV_idle_t *idle){
  listener->stat[EV_idle_e]++;
  _EV_idle_start(listener, idle);
}

void _EV_idle_stop(EV_t *listener, EV_idle_t *idle){
  uv_idle_stop(&idle->ev);
}
void EV_idle_stop(EV_t *listener, EV_idle_t *idle){
  _EV_idle_stop(listener, idle);
  listener->stat[EV_idle_e]--;
}

void EV_idle_init(EV_idle_t *idle, EV_idle_cb_t cb){
  idle->cb = cb;
}
