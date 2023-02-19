#include _WITCH_PATH(EV/backend/0/method/event/types.h)

typedef struct EV_idle_t EV_idle_t;
typedef void (*EV_idle_cb_t)(EV_t *, EV_idle_t *);

struct EV_idle_t{
	ev_idle ev;
	EV_idle_cb_t cb;
};

void _EV_idle_cb(struct ev_loop *loop, ev_idle *w, int flag){
	EV_t *listener = (EV_t *)ev_userdata(loop);
	EV_idle_t *idle = OFFSETLESS(w, EV_idle_t, ev);
	idle->cb(listener, idle);
}

void _EV_idle_start(EV_t *listener, EV_idle_t *idle){
	ev_idle_start(listener->loop, &idle->ev);
}
void EV_idle_start(EV_t *listener, EV_idle_t *idle){
	listener->stat[EV_idle_e]++;
	_EV_idle_start(listener, idle);
}

void _EV_idle_stop(EV_t *listener, EV_idle_t *idle){
	ev_idle_stop(listener->loop, &idle->ev);
}
void EV_idle_stop(EV_t *listener, EV_idle_t *idle){
	_EV_idle_stop(listener, idle);
	listener->stat[EV_idle_e]--;
}

void EV_idle_init(EV_idle_t *idle, EV_idle_cb_t cb){
	ev_idle_init(&idle->ev, _EV_idle_cb);
	idle->cb = cb;
}
