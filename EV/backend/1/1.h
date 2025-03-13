#pragma once

#include _WITCH_PATH(TH/TH.h)
#include _WITCH_PATH(MEM/MEM.h)
#include _WITCH_PATH(A/A.h)
#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(NET/NET.h)

#include <uv.h>

#include "types.h"

uint64_t EV_now(EV_t *listener){
  return uv_now(listener->loop);
}
uint64_t EV_nowi(EV_t *listener){
  return uv_now(listener->loop) * 1000000;
}
f64_t EV_nowf(EV_t *listener){
  return (f64_t)uv_now(listener->loop) / 1000;
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

#if defined(__platform_windows)
  typedef void (*EV_ListenObjects_cb_t)(void);

  void _EV_ListenObjects(EV_t *listener){
    while(1){
      DWORD r = MsgWaitForMultipleObjects(
        listener->ListenObjects.ObjectHandlesCurrent,
        listener->ListenObjects.ObjectHandles,
        FALSE,
        INFINITE,
        QS_ALLINPUT
      );
      if(r == WAIT_FAILED){
        __abort();
      }
      if(r == WAIT_OBJECT_0 + 0){
        TH_lock(&listener->ListenObjects.mutex[0]);
        for(uint32_t i = 0; i < listener->ListenObjects.QueueArrayCurrent[0]; i++){
          EV_event_t *evio = (EV_event_t *)listener->ListenObjects.QueueArray[0][i];
          if(evio->ListenObjectsID == (uint8_t)-1){
            listener->ListenObjects.ObjectHandles[listener->ListenObjects.ObjectHandlesCurrent] =
              (HANDLE)_get_osfhandle(evio->fd.fd);
            listener->ListenObjects.ObjectHandlesIO[listener->ListenObjects.ObjectHandlesCurrent] = evio;
            evio->ListenObjectsID = listener->ListenObjects.ObjectHandlesCurrent;
            listener->ListenObjects.ObjectHandlesCurrent++;
          }
          else{
            __abort();
          }
        }
        listener->ListenObjects.QueueArrayCurrent[0] = 0;
        TH_unlock(&listener->ListenObjects.mutex[0]);
      }
      else if(r == WAIT_OBJECT_0 + 1){
        TH_lock(&listener->ListenObjects.mutex[1]);
        for(uint32_t i = 0; i < listener->ListenObjects.QueueArrayCurrent[1]; i++){
          ((EV_ListenObjects_cb_t)listener->ListenObjects.QueueArray[1][i])();
        }
        listener->ListenObjects.QueueArrayCurrent[1] = 0;
        TH_unlock(&listener->ListenObjects.mutex[1]);
      }
      else if(r == listener->ListenObjects.ObjectHandlesCurrent){
        MSG msg;
        while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }
      else{
        uint32_t Index = r - WAIT_OBJECT_0;
        if(Index > listener->ListenObjects.ObjectHandlesCurrent){
          __abort();
        }
        TH_lock(&listener->ListenObjects.cond.mutex);
        if(IO_write(
          &listener->ListenObjects.pipes[1],
          &listener->ListenObjects.ObjectHandlesIO[Index],
          sizeof(listener->ListenObjects.ObjectHandlesIO[Index])
        ) != sizeof(listener->ListenObjects.ObjectHandlesIO[Index])){
          __abort();
        }
        TH_wait(&listener->ListenObjects.cond);
        TH_unlock(&listener->ListenObjects.cond.mutex);
      }
    }
  }

  void EV_ListenObjects_AddFunction(EV_t *listener, EV_ListenObjects_cb_t cb){
    TH_lock(&listener->ListenObjects.mutex[1]);
    listener->ListenObjects.QueueArray[1][listener->ListenObjects.QueueArrayCurrent[1]] = (void *)cb;
    listener->ListenObjects.QueueArrayCurrent[1]++;
    TH_unlock(&listener->ListenObjects.mutex[1]);
    if(SetEvent(listener->ListenObjects.ObjectHandles[1]) == 0){
      __abort();
    }
  }

  void _EV_ListenObjects_io(EV_t *listener, EV_event_t *evio, uint32_t flag){
    EV_event_t *received_evio;
    IO_ssize_t size = IO_read(&listener->ListenObjects.pipes[0], &received_evio, sizeof(received_evio));
    if(size != sizeof(received_evio)){
      __abort();
    }
    TH_lock(&listener->ListenObjects.cond.mutex);
    received_evio->cb(listener, received_evio, received_evio->evflag);
    TH_signal(&listener->ListenObjects.cond);
    TH_unlock(&listener->ListenObjects.cond.mutex);
  }
#endif

int EV_start(EV_t *listener){
  int r = uv_run(listener->loop, UV_RUN_DEFAULT);
  return r;
}
void EV_stop(EV_t *listener){
  uv_stop(listener->loop);
}

#include "queue.h"

void EV_open(EV_t *listener){
  listener->loop = (uv_loop_t *)A_resize(0, sizeof(uv_loop_t));
  uv_loop_init(listener->loop);

  uv_loop_set_data(listener->loop, listener);
  _EV_queue_init(listener);

  listener->watchers = (_EV_watcher_t *)A_resize(0, sizeof(_EV_watcher_t) * _EV_fdlimit);
  listener->nwatchers = 0;

  #if defined(__platform_windows)
    TH_mutex_init(&listener->ListenObjects.mutex[0]);
    TH_mutex_init(&listener->ListenObjects.mutex[1]);
    listener->ListenObjects.ObjectHandlesCurrent = 2;
    listener->ListenObjects.ObjectHandles[0] = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(listener->ListenObjects.ObjectHandles[0] == NULL){
      __abort();
    }
    listener->ListenObjects.ObjectHandles[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
    if(listener->ListenObjects.ObjectHandles[1] == NULL){
      __abort();
    }
    listener->ListenObjects.QueueArrayCurrent[0] = 0;
    listener->ListenObjects.QueueArrayCurrent[1] = 0;
    if(IO_pipe(listener->ListenObjects.pipes, IO_pipe_Flag_NoFlag) != 0){
      __abort();
    }
    TH_cond_init(&listener->ListenObjects.cond);
    EV_event_init_fd(&listener->ListenObjects.evio, &listener->ListenObjects.pipes[0], _EV_ListenObjects_io, EV_READ);
    EV_event_start(listener, &listener->ListenObjects.evio);
    listener->ListenObjects.ThreadID = TH_open((void *)_EV_ListenObjects, listener);
    if(listener->ListenObjects.ThreadID == (TH_id_t)-1){
      __abort();
    }
  #endif

  VAS2_open(&listener->timers, sizeof(_EV_timer_t), _EV_timerlimit);

  #if EV_set_tp
    _EV_tp_init(listener);
  #endif

  MEM_set(0, &listener->stat, sizeof(listener->stat));
}
void EV_close(EV_t *listener){

}
