#pragma once

#include _WITCH_PATH(MEM/MEM.h)
#include _WITCH_PATH(A/A.h)
#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(NET/NET.h)

#if defined(__platform_windows)
  #include _WITCH_PATH(NET/NET.h)
#endif
#include <ev.h>

#include "types.h"

ev_tstamp EV_now(EV_t *listener){
  return ev_now(listener->loop);
}
uint64_t EV_nowi(EV_t *listener){
  return (uint64_t)(ev_now(listener->loop) * 1000000000);
}
ev_tstamp EV_nowf(EV_t *listener){
  return ev_now(listener->loop);
}

#include "method/timer/timer.h"
#if EV_set_ev
  #include "method/event/event.h"
#endif
#if EV_set_tp
  #include "method/tp/tp.h"
#endif
#if EV_set_io
  #include "method/io/io.h"
#endif
#include "method/idle/idle.h"
#include "method/async/async.h"

int EV_start(EV_t *listener){
  return ev_run(listener->loop, 0);
}
void EV_stop(EV_t *listener){
  ev_break(listener->loop, EVBREAK_ONE);
}

#include "queue.h"

void EV_open(EV_t *listener){
  listener->loop = ev_loop_new(0);
  if(listener->loop == 0){
    __abort();
  }
  ev_set_userdata(listener->loop, listener);
  _EV_queue_init(listener);

  #if EV_set_tp
    _EV_tp_init(listener);
  #endif

  MEM_set(0, &listener->stat, sizeof(listener->stat));
}
void EV_close(EV_t *listener){

}
