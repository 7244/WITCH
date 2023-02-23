#define EV_BSTRev "libev"

#include _WITCH_PATH(EV/backend/0/method/event/types.h)

void EV_event_get_fd(EV_event_t *event, IO_fd_t *fd){
  fd->fd = ((ev_io *)event)->fd;
}
void EV_event_get_socket(EV_event_t *event, NET_socket_t *sock){
  sock->fd.fd = ((ev_io *)event)->fd;
}

EV_event_cb_t EV_event_get_cb(const EV_event_t *event){
  return event->cb;
}

void EV_event_init_fd(EV_event_t *event, const IO_fd_t *fd, EV_event_cb_t cb, uint32_t evflag){
  ev_io_init(&event->ev, _EV_event_cb, fd->fd, evflag);
  event->cb = cb;
}
void EV_event_init_socket(EV_event_t *event, const NET_socket_t *sock, EV_event_cb_t cb, uint32_t evflag){
  ev_io_init(&event->ev, _EV_event_cb, sock->fd.fd, evflag);
  event->cb = cb;
}

void _EV_event_cb(struct ev_loop *loop, ev_io *w, int flag){
  EV_t *listener = (EV_t *)ev_userdata(loop);
  EV_event_t *event = (EV_event_t *)w;
  uint32_t evflag = (flag & EV_READ) | (flag & EV_WRITE);
  event->cb(listener, event, evflag);
}

void _EV_event_start(EV_t *listener, EV_event_t *event){
  ev_io_start(listener->loop, &event->ev);
}
void EV_event_start(EV_t *listener, EV_event_t *event){
  listener->stat[EV_event_e]++;
  _EV_event_start(listener, event);
}

void _EV_event_stop(EV_t *listener, EV_event_t *event){
  ev_io_stop(listener->loop, &event->ev);
}
void EV_event_stop(EV_t *listener, EV_event_t *event){
  _EV_event_stop(listener, event);
  listener->stat[EV_event_e]--;
}

bool EV_ev_is_active(EV_t *listener, EV_event_t *event){
  return !ev_is_active(event);
}

uint32_t EV_ev_flag(EV_event_t *event){
  return event->ev.events;
}

void EV_ev_flag_set(EV_t *listener, EV_event_t *event, uint32_t evflag){
  bool start = 0;
  if(!EV_ev_is_active(listener, event)){
    ev_io_stop(listener->loop, &event->ev);
    start = 1;
  }
  #if defined(ev_io_modify)
    ev_io_modify(&event->ev, evflag);
  #else
    ev_io_set(&event->ev, event->ev.fd, evflag);
  #endif
  if(start){
    ev_io_start(listener->loop, &event->ev);
  }
}
void EV_ev_flag_add(EV_t *listener, EV_event_t *event, uint32_t evflag){
  uint32_t cevflag = event->ev.events;
  EV_ev_flag_set(listener, event, cevflag | evflag);
}
void EV_ev_flag_rm(EV_t *listener, EV_event_t *event, uint32_t evflag){
  uint32_t cevflag = event->ev.events;
  EV_ev_flag_set(listener, event, cevflag & ~evflag);
}
