void EV_timer_init(EV_timer_t *timer, f64_t timeout, EV_timer_cb_t cb){
  timer->timeout = timeout * 1000;
  timer->cb = cb;
}

void _EV_timer_cb(uv_timer_t *w){
  EV_timer_t *timer = (EV_timer_t *)((_EV_timer_t *)w)->data;
  EV_t *listener = (EV_t *)uv_loop_get_data(w->loop);
  timer->cb(listener, timer);
}

void _EV_timer_start(EV_t *listener, EV_timer_t *timer){
  timer->node = VAS2_NewNode(&listener->timers);
  _EV_timer_t *_timer = (_EV_timer_t *)VAS2_out(&listener->timers, timer->node);
  _timer->data = (void *)timer;
  if(uv_timer_init(listener->loop, &_timer->ev) != 0){
    PR_abort();
  }
  if(uv_timer_start(&_timer->ev, _EV_timer_cb, timer->timeout, timer->timeout) != 0){
    PR_abort();
  }
}
void EV_timer_start(EV_t *listener, EV_timer_t *timer){
  listener->stat[EV_timer_e]++;
  _EV_timer_start(listener, timer);
}

void _EV_timer_stop(EV_t *listener, EV_timer_t *timer){
  _EV_timer_t *_timer = (_EV_timer_t *)VAS2_out(&listener->timers, timer->node);
  uv_timer_stop(&_timer->ev);
  VAS2_unlink(&listener->timers, timer->node);
}
void EV_timer_stop(EV_t *listener, EV_timer_t *timer){
  _EV_timer_stop(listener, timer);
  listener->stat[EV_timer_e]--;
}
