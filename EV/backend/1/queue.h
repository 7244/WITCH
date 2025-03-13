typedef void (*EV_queue_cb_t)(EV_t *, void *);
typedef struct{
  EV_queue_cb_t cb;
  void *p;
}_EV_queue_node_t;

void _EV_queue_cb(uv_async_t *w){
  EV_t *listener = (EV_t *)uv_loop_get_data(w->loop);
  _EV_queue_t *queue = &listener->queue;

  TH_lock(&queue->mutex);
  _EV_queue_node_t *node = (_EV_queue_node_t *)queue->nodes.ptr;
  _EV_queue_node_t *limit = &node[queue->nodes.Current];
  for(; node != limit; node++){
    node->cb(listener, node->p);
  }
  queue->nodes.Current = 0;
  TH_unlock(&queue->mutex);
}

void EV_queue_lock(EV_t *listener){
  TH_lock(&listener->queue.mutex);
}
void EV_queue_unlock(EV_t *listener){
  TH_unlock(&listener->queue.mutex);
}
void EV_queue_signal(EV_t *listener){
  if(uv_async_send(&listener->queue.evsync) != 0){
    __abort();
  }
}
void EV_queue_add(EV_t *listener, EV_queue_cb_t cb, void *p){
  _EV_queue_t *queue = &listener->queue;
  _EV_queue_node_t node;
  node.cb = cb;
  node.p = p;
  VEC_handle(&queue->nodes);
  ((_EV_queue_node_t *)queue->nodes.ptr)[queue->nodes.Current] = node;
  queue->nodes.Current++;
}

void _EV_queue_init(EV_t *listener){
  VEC_init(&listener->queue.nodes, 1, A_resize);
  TH_mutex_init(&listener->queue.mutex);
  if(uv_async_init(listener->loop, &listener->queue.evsync, _EV_queue_cb) != 0){
    __abort();
  }
}
