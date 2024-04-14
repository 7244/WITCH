typedef struct EV_io_sendfile_t EV_io_sendfile_t;
typedef void (*EV_io_sendfile_cb_t)(EV_t *, EV_io_sendfile_t *, sint32_t /* err */);
struct EV_io_sendfile_t{
  EV_event_t ev; /* outfd is here */
  uint64_t isize, nsize;
  IO_fd_t infd;
  EV_io_sendfile_cb_t cb;
};

void _EV_sendfile_writeablecb(EV_t *listener, EV_event_t *ev, uint32_t flag){
  EV_io_sendfile_t *req = OFFSETLESS(ev, EV_io_sendfile_t, ev);

  IO_fd_t outfd;
  EV_event_get_fd(ev, &outfd);
  IO_off_t size = IO_sendfile(&req->infd, &outfd, req->isize, req->nsize - req->isize);
  if(size < 0){
    _EV_event_stop(listener, ev);
    req->cb(listener, req, size);
    listener->stat[EV_io_e]--;
    return;
  }
  req->isize += size;
  if(req->isize != req->nsize){
    return;
  }
  _EV_event_stop(listener, ev);
  req->cb(listener, req, 0);
  listener->stat[EV_io_e]--;
}
void EV_io_sendfile_init(EV_io_sendfile_t *req, const IO_fd_t *infd, const IO_fd_t *outfd, uint64_t isize, uint64_t nsize, EV_io_sendfile_cb_t cb){
  EV_event_init_fd(&req->ev, outfd, _EV_sendfile_writeablecb, EV_WRITE);
  req->isize = isize;

  /* we store isize and nsize like range */
  /* i dont know how comfortable it is for user */
  /* but its clearly looks like more wise inside this header */
  req->nsize = isize + nsize;

  req->infd = *infd;
  req->cb = cb;
}
sint32_t EV_io_sendfile_start(EV_t *listener, EV_io_sendfile_t *req){
  IO_fd_t outfd;
  EV_event_get_fd(&req->ev, &outfd);
  IO_off_t size = IO_sendfile(&req->infd, &outfd, req->isize, req->nsize - req->isize);
  if(size < 0){
    return size;
  }
  req->isize += size;
  if(req->isize == req->nsize){
    req->cb(listener, req, 0);
    return 0;
  }

  listener->stat[EV_io_e]++;
  _EV_event_start(listener, &req->ev);
  return 0;
}

void EV_io_sendfile_stop(EV_t *listener, EV_io_sendfile_t *req){
  _EV_event_stop(listener, &req->ev);
  req->cb(listener, req, -ECANCELED);
  listener->stat[EV_io_e]--;
}
