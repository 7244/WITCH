typedef struct EV_async_t EV_async_t;
typedef void (*EV_async_cb_t)(EV_t *, EV_async_t *);

struct EV_async_t{
  uv_async_t async;
  EV_async_cb_t cb;
};

void _EV_async_cb(uv_async_t *w){
  EV_t *ev = (EV_t *)uv_loop_get_data(w->loop);
  EV_async_t *async = OFFSETLESS(w, EV_async_t, async);
  async->cb(ev, async);
}

void EV_async_init(EV_t *ev, EV_async_t *async, EV_async_cb_t cb){
  async->cb = cb;
}
void EV_async_start(EV_t *ev, EV_async_t *async){
  uv_async_init(ev->loop, &async->async, _EV_async_cb);
}
void EV_async_stop(EV_t *ev, EV_async_t *async){
  uv_close((uv_handle_t *)&async->async, NULL);
}

void EV_async_send(EV_t *ev, EV_async_t *async){
  uv_async_send(&async->async);
}
