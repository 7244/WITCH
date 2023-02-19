#define EV_BSTRev "libuv"

#include _WITCH_PATH(EV/backend/1/method/event/types.h)

void EV_event_get_fd(EV_event_t *event, IO_fd_t *fd){
	*fd = event->fd;
}
void EV_event_get_socket(EV_event_t *event, NET_socket_t *sock){
	sock->fd = event->fd;
}

EV_event_cb_t EV_event_get_cb(const EV_event_t *event){
	return event->cb;
}

uint32_t EV_ev_flag(EV_event_t *event){
	return event->evflag;
}

sint32_t _EV_ev_fd(uv_poll_t *ev){
	#if defined(WOS_UNIX)
		return ev->io_watcher.fd;
	#elif defined(WOS_WINDOWS)
		return ev->socket;
	#else
		#error ?
	#endif
}

void _EV_ev_watcher_cb(uv_poll_t *ev, int status, int flag){
	EV_t *listener = (EV_t *)uv_loop_get_data(ev->loop);
	_EV_watcher_t *watcher = &listener->watchers[_EV_ev_fd(ev)];
	if(!flag){
		if(watcher->readev){
			watcher->readcb(listener, watcher->readev, EV_READ);
		}
		if(watcher->writeev){
			watcher->writecb(listener, watcher->writeev, EV_WRITE);
		}
	}
	else{
		if(flag & EV_READ){
			watcher->readcb(listener, watcher->readev, EV_READ);
		}
		if(flag & EV_WRITE){
			/* readcb able to close watcher */
			if(watcher->writeev){
				watcher->writecb(listener, watcher->writeev, EV_WRITE);
			}
		}
	}
}

void _EV_ev_watcher_set(EV_t *listener, sint32_t fd, EV_event_t *event, EV_event_cb_t cb, uint32_t flag){
	if(fd >= listener->nwatchers){
		for(; listener->nwatchers <= fd; listener->nwatchers++){
			if(listener->nwatchers == _EV_fdlimit){
				PR_abort();
			}
			_EV_watcher_t *watcher = &listener->watchers[listener->nwatchers];
			watcher->inited = 1;
			watcher->readev = 0;
			watcher->writeev = 0;
		}
	}
	_EV_watcher_t *watcher = &listener->watchers[fd];
	if(flag & EV_READ){
		if(watcher->readev != 0){
			PR_abort();
		}
		watcher->readev = event;
		watcher->readcb = cb;
	}
	if(flag & EV_WRITE){
		if(watcher->writeev != 0){
			PR_abort();
		}
		watcher->writeev = event;
		watcher->writecb = cb;
	}
	uint32_t evflag = 0;
	evflag |= EV_READ * !!watcher->readev;
	evflag |= EV_WRITE * !!watcher->writeev;
	if(evflag){
		if(watcher->inited){
			watcher->inited = 0;
			#if defined(WOS_UNIX)
				if(uv_poll_init(listener->loop, &watcher->ev, fd) != 0){
					PR_abort();
				}
			#elif defined(WOS_WINDOWS)
				if(uv_poll_init_socket(listener->loop, &watcher->ev, fd) != 0){
					PR_abort();
				}
			#else
				PR_abort();
			#endif
		}
		if(watcher->inited != 0){
			PR_abort();
		}
		if(uv_poll_start(&watcher->ev, evflag, _EV_ev_watcher_cb) != 0){
			PR_abort();
		}
	}
	else{
		PR_abort();
		if(uv_poll_stop(&watcher->ev) != 0){
			PR_abort();
		}
	}
}

void _EV_ev_watcher_update(EV_t *listener, sint32_t fd, uint32_t oldflag, uint32_t newflag){
	_EV_watcher_t *watcher = &listener->watchers[fd];
	if(oldflag & EV_READ){
		if(!(oldflag & EV_WRITE) && newflag & EV_WRITE){
			if(watcher->writeev != 0){
				PR_abort();
			}
			watcher->writeev = watcher->readev;
			watcher->writecb = watcher->readcb;
		}
		if(!(newflag & EV_READ)){
			watcher->readev = 0;
		}
	}
	if(oldflag & EV_WRITE){
		if(!(oldflag & EV_READ) && newflag & EV_READ){
			if(watcher->readev != 0){
				PR_abort();
			}
			watcher->readev = watcher->writeev;
			watcher->readcb = watcher->writecb;
		}
		if(!(newflag & EV_WRITE)){
			watcher->writeev = 0;
		}
	}
	if(newflag){
		if(oldflag != newflag){
			if(watcher->inited != 0){
				PR_abort();
			}
			if(uv_poll_start(&watcher->ev, newflag, _EV_ev_watcher_cb) != 0){
				PR_abort();
			}
		}
	}
	else{
		PR_abort();
		if(uv_poll_stop(&watcher->ev) != 0){
			PR_abort();
		}
	}
}

void _EV_ev_watcher_del(EV_t *listener, sint32_t fd, uint32_t flag){
	_EV_watcher_t *watcher = &listener->watchers[fd];
	if(flag & EV_READ){
		watcher->readev = 0;
	}
	if(flag & EV_WRITE){
		watcher->writeev = 0;
	}
	uint32_t evflag = 0;
	evflag |= EV_READ * !!watcher->readev;
	evflag |= EV_WRITE * !!watcher->writeev;
	if(evflag){
		if(watcher->inited != 0){
			PR_abort();
		}
		if(uv_poll_start(&watcher->ev, evflag, _EV_ev_watcher_cb) != 0){
			PR_abort();
		}
	}
	else{
		if(watcher->inited != 0){
			PR_abort();
		}
		watcher->inited = 1;
		if(uv_poll_stop(&watcher->ev) != 0){
			PR_abort();
		}
	}
}

void EV_event_init_fd(EV_event_t *event, const IO_fd_t *fd, EV_event_cb_t cb, uint32_t evflag){
	event->fd = *fd;
	event->evflag = evflag;
	event->cb = cb;
}
void EV_event_init_socket(EV_event_t *event, const NET_socket_t *sock, EV_event_cb_t cb, uint32_t evflag){
	event->fd = sock->fd;
	event->evflag = evflag;
	event->cb = cb;
}

void _EV_event_cb(EV_t *listener, EV_event_t *event, uint32_t flag){
	event->cb(listener, event, flag);
}

void _EV_event_start(EV_t *listener, EV_event_t *event){
	#if defined(WOS_WINDOWS)
		switch(_IO_get_fd(&event->fd)){
			case _IO_fd_tty_e:{
				event->ListenObjectsID = (uint8_t)-1;
				TH_lock(&listener->ListenObjects.mutex[0]);
				listener->ListenObjects.QueueArray[0][listener->ListenObjects.QueueArrayCurrent[0]] = event;
				listener->ListenObjects.QueueArrayCurrent[0]++;
				TH_unlock(&listener->ListenObjects.mutex[0]);
				if(SetEvent(listener->ListenObjects.ObjectHandles[0]) == 0){
					PR_abort();
				}
				return;
			}
			case _IO_fd_socket_e:{
				break;
			}
		}
	#endif
	IO_fd_t fd;
	EV_event_get_fd(event, &fd);
	_EV_ev_watcher_set(listener, fd.fd, event, _EV_event_cb, event->evflag);
}
void EV_event_start(EV_t *listener, EV_event_t *event){
	listener->stat[EV_event_e]++;
	_EV_event_start(listener, event);
}

void _EV_event_stop(EV_t *listener, EV_event_t *event){
	IO_fd_t fd;
	EV_event_get_fd(event, &fd);
	_EV_ev_watcher_del(listener, fd.fd, EV_ev_flag(event));
}
void EV_event_stop(EV_t *listener, EV_event_t *event){
	_EV_event_stop(listener, event);
	listener->stat[EV_event_e]--;
}

bool EV_ev_is_active(EV_t *listener, EV_event_t *event){
	IO_fd_t fd;
	EV_event_get_fd(event, &fd);
	uint32_t cevflag = EV_ev_flag(event);
	_EV_watcher_t *watcher = &listener->watchers[fd.fd];
	if(cevflag & EV_READ){
		if(watcher->readev){
			return 0;
		}
	}
	else if(cevflag & EV_WRITE){
		if(watcher->writeev){
			return 0;
		}
	}
	return 1;
}

void EV_ev_flag_set(EV_t *listener, EV_event_t *event, uint32_t evflag){
	IO_fd_t fd;
	EV_event_get_fd(event, &fd);
	_EV_watcher_t *watcher = &listener->watchers[fd.fd];
	uint32_t cevflag = EV_ev_flag(event);
	if(cevflag & EV_READ){
		if(watcher->readev){
			_EV_ev_watcher_update(listener, fd.fd, cevflag, evflag);
		}
	}
	else if(cevflag & EV_WRITE){
		if(watcher->writeev){
			_EV_ev_watcher_update(listener, fd.fd, cevflag, evflag);
		}
	}
	event->evflag = evflag;
}
void EV_ev_flag_add(EV_t *listener, EV_event_t *event, uint32_t evflag){
	uint32_t cevflag = EV_ev_flag(event);
	EV_ev_flag_set(listener, event, cevflag | evflag);
}
void EV_ev_flag_rm(EV_t *listener, EV_event_t *event, uint32_t evflag){
	uint32_t cevflag = EV_ev_flag(event);
	EV_ev_flag_set(listener, event, cevflag & ~evflag);
}
