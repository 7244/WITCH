#pragma once

#include _WITCH_PATH(A/A.h)
#include _WITCH_PATH(TH/TH.h)
#include _WITCH_PATH(VAS/VAS2.h)

#define EV_READ UV_READABLE
#define EV_WRITE UV_WRITABLE
#define EV_TIMER UV_DISCONNECT

typedef struct EV_t EV_t;

#define _EV_fdlimit 16384
#define _EV_timerlimit 16384

#include _WITCH_PATH(EV/backend/1/method/timer/types.h)
#include _WITCH_PATH(EV/backend/1/method/tp/types.h)

typedef struct{
  VEC_t nodes;
  TH_mutex_t mutex;
  uv_async_t evsync;
}_EV_queue_t;

typedef struct{
  uv_poll_t ev;
  bool inited;
  EV_event_t *readev;
  EV_event_cb_t readcb;
  EV_event_t *writeev;
  EV_event_cb_t writecb;
}_EV_watcher_t;

typedef struct{
  uv_timer_t ev;
  void *data;
}_EV_timer_t;

struct EV_t{
  uv_loop_t *loop;
  _EV_queue_t queue;
  _EV_tp_t tp;

  _EV_watcher_t *watchers;
  uint32_t nwatchers;

  #if defined(__platform_windows)
    struct{
      TH_mutex_t mutex[2];
      TH_id_t ThreadID;
      HANDLE ObjectHandles[MAXIMUM_WAIT_OBJECTS];
      void *ObjectHandlesIO[MAXIMUM_WAIT_OBJECTS];
      DWORD ObjectHandlesCurrent;

      IO_fd_t pipes[2];
      EV_event_t evio;
      TH_cond_t cond;

      void *QueueArray[2][MAXIMUM_WAIT_OBJECTS * 2];
      uint32_t QueueArrayCurrent[2];
    }ListenObjects;
  #endif

  /* _EV_timer_t */
  VAS2_t timers;

  /* w by listener, r by user */
  /* TODO this was auint32. you need to make operations of this thing atomic */
  uint32_t stat[EV_total_e];
};
