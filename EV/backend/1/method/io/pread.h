typedef struct EV_io_pread_t EV_io_pread_t;
typedef void (*EV_io_pread_cb_t)(EV_t *, EV_io_pread_t *, IO_ssize_t /* result */);
struct EV_io_pread_t{
  EV_tp_t tp;
  IO_fd_t fd;
  void *data;
  IO_off_t isize;
  IO_size_t nsize;
  IO_ssize_t result;
  EV_io_pread_cb_t cb;
};

bool _EV_io_pread_cb_outside(EV_t *listener, EV_tp_t *tp){
  EV_io_pread_t *req = OFFSETLESS(tp, EV_io_pread_t, tp);
  req->result = IO_pread(&req->fd, req->data, req->isize, req->nsize);
  return 0;
}
void _EV_io_pread_cb_inside(EV_t *listener, EV_tp_t *tp, sint32_t err){
  EV_io_pread_t *req = OFFSETLESS(tp, EV_io_pread_t, tp);
  req->cb(listener, req, req->result);
}
void _EV_io_pread_cb_inside_stop(EV_t *listener, EV_tp_t *tp, sint32_t err){
  EV_io_pread_t *req = OFFSETLESS(tp, EV_io_pread_t, tp);
  req->cb(listener, req, -ECANCELED);
}

void EV_io_pread_init(EV_io_pread_t *req, IO_fd_t fd, void *data, IO_off_t isize, IO_size_t nsize, EV_io_pread_cb_t cb){
  req->fd = fd;
  req->data = data;
  req->isize = isize;
  req->nsize = nsize;
  req->cb = cb;
}

void EV_io_pread_start(EV_t *listener, EV_io_pread_t *req){
  EV_tp_init(&req->tp, _EV_io_pread_cb_outside, _EV_io_pread_cb_inside, 1);
  EV_tp_start(listener, &req->tp);
}
void EV_io_pread_stop(EV_t *listener, EV_io_pread_t *req){
  req->tp.cb_inside = _EV_io_pread_cb_inside_stop;
}
