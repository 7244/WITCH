#include _WITCH_PATH(EV/backend/0/method/timer/types.h)

void EV_timer_init(EV_timer_t *timer, f64_t timeout, EV_timer_cb_t timer_cb){
	ev_timer_init(&timer->ev, _EV_timer_cb, timeout, timeout);
	timer->cb = timer_cb;
}

void _EV_timer_cb(struct ev_loop *loop, ev_timer *w, int flag){
	EV_timer_t *timer = (EV_timer_t *)w;
	EV_t *listener = (EV_t *)ev_userdata(loop);
	timer->cb(listener, timer);
}

void _EV_timer_start(EV_t *listener, EV_timer_t *timer){
	ev_timer_start(listener->loop, &timer->ev);
}
void EV_timer_start(EV_t *listener, EV_timer_t *timer){
	listener->stat[EV_timer_e]++;
	_EV_timer_start(listener, timer);
}

void _EV_timer_stop(EV_t *listener, EV_timer_t *timer){
	ev_timer_stop(listener->loop, &timer->ev);
}
void EV_timer_stop(EV_t *listener, EV_timer_t *timer){
	_EV_timer_stop(listener, timer);
	listener->stat[EV_timer_e]--;
}
