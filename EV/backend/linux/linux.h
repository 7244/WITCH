#pragma once

#include _WITCH_PATH(IO/IO.h)

#include "types.h"

#include "method/timer/timer.h"

sint32_t EV_start(EV_t *ev){
  while(1){

    if()
  }
  return 0;
}
void EV_stop(EV_t *ev){
  ev->Stop = -1;
}

sint32_t EV_open(EV_t *ev){
  ev->epoll_fd = IO_epoll_create(0);
  if(ev->epoll_fd < 0){
    return ev->epoll_fd;
  }

  ev->Stop = 0;

  return 0;
}
void EV_close(EV_t *ev){
  IO_fd_t fd;
  fd.fd = ev->epoll_fd;
  IO_close(&fd);
}
