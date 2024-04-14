typedef struct EV_async_t EV_async_t;
typedef void (*EV_async_cb_t)(EV_t *, EV_async_t *);

struct EV_async_t{
  ev_async async;
  EV_async_cb_t cb;
};

void _EV_async_cb(struct ev_loop *loop, ev_async *w, int filler){
  EV_t *ev = (EV_t *)ev_userdata(loop);
  EV_async_t *async = OFFSETLESS(w, EV_async_t, async);
  async->cb(ev, async);
}

void EV_async_init(EV_t *ev, EV_async_t *async, EV_async_cb_t cb){
  ev_async_init(&async->async, _EV_async_cb);
  async->cb = cb;
}
void EV_async_start(EV_t *ev, EV_async_t *async){
  ev_async_start(ev->loop, &async->async);
}
void EV_async_stop(EV_t *ev, EV_async_t *async){
  ev_async_stop(ev->loop, &async->async);
}

void EV_async_send(EV_t *ev, EV_async_t *async){
  ev_async_send(ev->loop, &async->async);
}
