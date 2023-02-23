#define EV_BSTRtp "witch_tp"

#include _WITCH_PATH(EV/backend/1/method/tp/types.h)

/* most operating systems probably cant open too much thread */
/* so it can be good idea to limit it to avoid bookkeeping */
#ifndef EV_tp_set_limit
  #define EV_tp_set_limit 0x4000
#endif

void _EV_tp_work(_EV_tp_internal_pool_started_t *pool_started){
  EV_t *listener = pool_started->listener;
  VAS2_node_t pool_node;
  goto wait_gt;
  while(pool_started->conti){
    pool_node = pool_started->tp->pool_node;
    if(!pool_started->tp->cb_outside(listener, pool_started->tp)){
      if(IO_write(&listener->tp.write_fd, &pool_node, sizeof(VAS2_node_t)) != sizeof(VAS2_node_t)){
        PR_abort();
      }
    }
    wait_gt:
    TH_lock(&pool_started->cond.mutex);
    while(!pool_started->ping){
      TH_wait(&pool_started->cond);
    }
    pool_started->ping = 0;
    TH_unlock(&pool_started->cond.mutex);
  }
}

void _EV_tp_free_timeout_cb(EV_t *listener, EV_timer_t *t){
  EV_timer_stop(listener, t);

  _EV_tp_internal_pool_free_t *pool_free = OFFSETLESS(t, _EV_tp_internal_pool_free_t, evt);
  _EV_tp_internal_pool_started_t *pool_started = (_EV_tp_internal_pool_started_t *)VAS2_out(&listener->tp.pool.started, pool_free->started_node);

  pool_started->conti = 0;
  TH_lock(&pool_started->cond.mutex);
  pool_started->ping = 1;
  TH_signal(&pool_started->cond);
  TH_unlock(&pool_started->cond.mutex);

  TH_join(pool_started->tid);

  VAS2_unlink(&listener->tp.pool.started, pool_free->started_node);
  VAS1_unlink(&listener->tp.pool.free, pool_free->this_node);
}

void EV_tp_del(EV_t *listener, VAS2_node_t pool_node){
  _EV_tp_internal_pool_started_t *pool_started = (_EV_tp_internal_pool_started_t *)VAS2_out(&listener->tp.pool.started, pool_node);
  pool_started->tp->cb_inside(listener, pool_started->tp, 0);

  listener->tp.c[pool_started->flag]++;

  if(VAS_usage(&listener->tp.queue)) do{
    VAS_node_t queued_node = *VAS_road0(&listener->tp.queue, listener->tp.queue.src);
    _EV_tp_internal_queue_t *queued = (_EV_tp_internal_queue_t *)VAS_out(&listener->tp.queue, queued_node);

    /* queued doesnt have points */
    /* btw queued is always flag 0 */
    if(!listener->tp.c[queued->tp->flag]){
      break;
    }
    listener->tp.c[queued->tp->flag]--;

    pool_started->tp = queued->tp;
    pool_started->flag = queued->tp->flag;

    queued->tp->pool_node = pool_node;

    VAS_unlink(&listener->tp.queue, queued_node);

    TH_lock(&pool_started->cond.mutex);
    pool_started->ping = 1;
    TH_signal(&pool_started->cond);
    TH_unlock(&pool_started->cond.mutex);

    return;
  }while(0);

  VAS1_node_t free_node = VAS1_NewNodeFirst(&listener->tp.pool.free);
  _EV_tp_internal_pool_free_t *pool_free = (_EV_tp_internal_pool_free_t *)VAS1_out(&listener->tp.pool.free, free_node);
  pool_free->this_node = free_node;
  pool_free->started_node = pool_node;
  EV_timer_init(&pool_free->evt, 2, _EV_tp_free_timeout_cb);
  EV_timer_start(listener, &pool_free->evt);
}

void _EV_tp_read_cb(EV_t *listener, EV_event_t *io, uint32_t flag){
  VAS2_node_t pool_node[1024];
  while(1){
    IO_fd_t fd;
    EV_event_get_fd(io, &fd);
    IO_ssize_t readsize = IO_read(&fd, pool_node, sizeof(pool_node));
    if(readsize <= 0){
      if(readsize == 0){
        return;
      }
      else{
        PR_abort();
      }
    }
    if(readsize % sizeof(VAS2_node_t) != 0){
      PR_abort();
    }
    readsize /= sizeof(VAS2_node_t);

    for(VAS2_node_t ireadsize = 0; ireadsize < readsize; ireadsize++){
      EV_tp_del(listener, pool_node[ireadsize]);
    }

    if(readsize != 1024){
      return;
    }
  }
}

void EV_tp_init(EV_tp_t *tp, EV_tp_cb_outside_t cb_outside, EV_tp_cb_inside_t cb_inside, bool flag){
  tp->flag = flag;
  tp->cb_outside = cb_outside;
  tp->cb_inside = cb_inside;
}

VAS2_node_t _EV_tp_get_from_pool(EV_t *listener){
  if(VAS1_usage(&listener->tp.pool.free)){
    VAS1_node_t first = *VAS1_road0(&listener->tp.pool.free, listener->tp.pool.free.src);
    _EV_tp_internal_pool_free_t *pool_free = (_EV_tp_internal_pool_free_t *)VAS1_out(&listener->tp.pool.free, first);
    EV_timer_stop(listener, &pool_free->evt);
    VAS2_node_t r = pool_free->started_node;
    VAS1_unlink(&listener->tp.pool.free, first);
    return r;
  }
  else{
    VAS2_node_t started_node = VAS2_NewNode(&listener->tp.pool.started);
    _EV_tp_internal_pool_started_t *pool_started = (_EV_tp_internal_pool_started_t *)VAS2_out(&listener->tp.pool.started, started_node);
    pool_started->listener = listener;
    TH_cond_init(&pool_started->cond);
    pool_started->ping = 0;
    pool_started->conti = 1;
    pool_started->tid = TH_open((void *)_EV_tp_work, pool_started);
    return started_node;
  }
}

void EV_tp_start(EV_t *listener, EV_tp_t *tp){
  if(listener->tp.c[tp->flag]){
    listener->tp.c[tp->flag]--;
    VAS2_node_t started_node = _EV_tp_get_from_pool(listener);
    _EV_tp_internal_pool_started_t *pool_started = (_EV_tp_internal_pool_started_t *)VAS2_out(&listener->tp.pool.started, started_node);
    pool_started->tp = tp;
    pool_started->flag = tp->flag;

    tp->pool_node = started_node;

    TH_lock(&pool_started->cond.mutex);
    pool_started->ping = 1;
    TH_signal(&pool_started->cond);
    TH_unlock(&pool_started->cond.mutex);
  }
  else{
    VAS_node_t queued_node = VAS_NewNodeLast(&listener->tp.queue);
    _EV_tp_internal_queue_t *queued = (_EV_tp_internal_queue_t *)VAS_out(&listener->tp.queue, queued_node);
    queued->tp = tp;
  }
}

/* only called by EV_open */
void _EV_tp_init(EV_t *listener){
  VAS_open(&listener->tp.queue, sizeof(_EV_tp_internal_queue_t));
  listener->tp.c[0] = WITCH_num_online_cpus();
  listener->tp.c[1] = (uint32_t)-1;
  IO_fd_t pips[2];
  if(IO_pipe(pips, IO_pipe_Flag_NonblockRead) != 0){
    PR_abort();
  }
  listener->tp.write_fd = pips[1];
  VAS2_open(&listener->tp.pool.started, sizeof(_EV_tp_internal_pool_started_t), EV_tp_set_limit);
  VAS1_open(&listener->tp.pool.free, sizeof(_EV_tp_internal_pool_free_t), EV_tp_set_limit);
  EV_event_init_fd(&listener->tp.read_ev, &pips[0], _EV_tp_read_cb, EV_READ);
  EV_event_start(listener, &listener->tp.read_ev);
}
