#pragma once

#include _WITCH_PATH(VAS/VAS.h)
#include _WITCH_PATH(VAS/VAS1.h)
#include _WITCH_PATH(VAS/VAS2.h)
#include _WITCH_PATH(EV/backend/0/method/event/types.h)

typedef struct EV_tp_t EV_tp_t;
typedef bool (*EV_tp_cb_outside_t)(EV_t *, EV_tp_t *);
typedef void (*EV_tp_cb_inside_t)(EV_t *, EV_tp_t *, sint32_t err);
struct EV_tp_t{
  union{
    VAS2_node_t pool_node;

    /* will be passed to pool_started and queue struct */
    bool flag;
  };

  /* this used for general processing you must dont free EV_tp_t in here */
  /* returning FALSE will call cb_inside. */
  EV_tp_cb_outside_t cb_outside;
  /* you can free EV_tp_t * in here */
  /* this can be called with 2 way. */
  /* 0 cb_outside returned FALSE */
  /* 1 cb_outside failed to called for some reason */
  EV_tp_cb_inside_t cb_inside;
};

/* VAS_t node */
typedef struct{
  EV_tp_t *tp;
}_EV_tp_internal_queue_t;

/* VAS2_t node */
typedef struct{
  EV_t *listener;
  TH_cond_t cond;
  bool ping;
  bool conti;
  TH_id_t tid;

  EV_tp_t *tp;
  bool flag;
}_EV_tp_internal_pool_started_t;

/* VAS1_t node */
typedef struct{
  EV_timer_t evt;
  VAS1_node_t this_node;
  VAS2_node_t started_node;
}_EV_tp_internal_pool_free_t;

typedef struct{
  VAS_t queue;

  /*
    0 is THREADS
    1 is -1
  */
  uint32_t c[2];

  struct{
    /* started holds all started threads as _EV_tp_internal_pool_t */
    /* after work is done it will pass own node to .free then wait till get timeout or someone removes it from .free and restarts */

    VAS2_t started;
    VAS1_t free;
  }pool;

  EV_event_t read_ev;
  IO_fd_t write_fd;
}_EV_tp_t;
