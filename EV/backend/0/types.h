#pragma once

#include _WITCH_PATH(WITCH.h)
#include _WITCH_PATH(VEC/VEC.h)
#include _WITCH_PATH(TH/TH.h)

#define EV_READ EV_READ
#define EV_WRITE EV_WRITE
#define EV_TIMER EV_TIMER

typedef struct EV_t EV_t;

typedef struct{
  VEC_t nodes;
  TH_mutex_t mutex;
  ev_async evsync;
}_EV_queue_t;

#include _WITCH_PATH(EV/backend/0/method/timer/types.h)
#include _WITCH_PATH(EV/backend/0/method/tp/types.h)

struct EV_t{
  struct ev_loop *loop;

  _EV_queue_t queue;

  _EV_tp_t tp;

  /* w by listener, r by user */
  /* TODO this was auint32. you need to make operations of this thing atomic */
  uint32_t stat[EV_total_e];
};
