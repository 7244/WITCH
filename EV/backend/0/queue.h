typedef void (*EV_queue_cb_t)(EV_t *, void *);
typedef struct{
	EV_queue_cb_t cb;
	void *p;
}_EV_queue_node_t;

void _EV_queue_cb(struct ev_loop *loop, ev_async *w, int flag){
	EV_t *listener = (EV_t *)ev_userdata(loop);
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
	ev_async_send(listener->loop, &listener->queue.evsync);
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
	VEC_init(&listener->queue.nodes, sizeof(_EV_queue_node_t), A_resize);
	TH_mutex_init(&listener->queue.mutex);
	ev_async_init(&listener->queue.evsync, _EV_queue_cb);
	ev_async_start(listener->loop, &listener->queue.evsync);
}
