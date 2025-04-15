/* related functions about opening/closing peer */

typedef struct{
  sint32_t level;
  sint32_t optname;
  sint32_t value;
}NET_TCP_sockopt_t;

NET_TCP_QueuerReference_t NET_TCP_GetReadQueuerReferenceFirst(NET_TCP_peer_t *peer){
  return _NET_TCP_QueuerList_GetNodeFirst(&peer->ReadQueuerList);
}
NET_TCP_QueuerReference_t NET_TCP_GetReadQueuerReferenceLast(NET_TCP_peer_t *peer){
  return _NET_TCP_QueuerList_GetNodeLast(&peer->ReadQueuerList);
}
NET_TCP_QueuerReference_t NET_TCP_IterateReadQueuerReference(NET_TCP_peer_t *peer, NET_TCP_QueuerReference_t QueuerReference){
  return _NET_TCP_QueuerList_GetNodeByReference(&peer->ReadQueuerList, QueuerReference)->NextNodeReference;
}
bool NET_TCP_IsReadQueuerReferenceValid(NET_TCP_peer_t *peer, NET_TCP_QueuerReference_t QueuerReference){
  return !_NET_TCP_QueuerList_IsNodeReferenceRecycled(&peer->ReadQueuerList, QueuerReference);
}

NET_TCP_QueuerReference_t NET_TCP_GetWriteQueuerReferenceFirst(NET_TCP_peer_t *peer){
  return _NET_TCP_QueuerList_GetNodeFirst(&peer->WriteQueuerList);
}
NET_TCP_QueuerReference_t NET_TCP_GetWriteQueuerReferenceLast(NET_TCP_peer_t *peer){
  return _NET_TCP_QueuerList_GetNodeLast(&peer->WriteQueuerList);
}
NET_TCP_QueuerReference_t NET_TCP_IterateWriteQueuerReference(NET_TCP_peer_t *peer, NET_TCP_QueuerReference_t QueuerReference){
  return _NET_TCP_QueuerList_GetNodeByReference(&peer->WriteQueuerList, QueuerReference)->NextNodeReference;
}
bool NET_TCP_IsWriteQueuerReferenceValid(NET_TCP_peer_t *peer, NET_TCP_QueuerReference_t QueuerReference){
  return !_NET_TCP_QueuerList_IsNodeReferenceRecycled(&peer->WriteQueuerList, QueuerReference);
}

NET_TCP_layerflag_t NET_TCP_StateLoop(NET_TCP_peer_t *peer){
  NET_TCP_op_t *opstate = &peer->parent->op[NET_TCP_opid_state_e];
  while(peer->StateOffset < opstate->total){
    NET_TCP_layer_state_t *layer = &((NET_TCP_layer_state_t *)opstate->layer)[peer->StateOffset];
    NET_TCP_extinfo_t *extinfo = &peer->parent->ext.info[layer->extid];
    NET_TCP_layerflag_t flag = ((NET_TCP_cb_state_t)layer->func)(
      peer,
      peer->parent->SockData + extinfo->SockDataOffset,
      (uint8_t *)peer + sizeof(NET_TCP_peer_t) + extinfo->PeerDataOffset,
      NET_TCP_state_succ_e
    );
    if(flag & NET_TCP_EXT_dontgo_e){
      return NET_TCP_EXT_dontgo_e;
    }
    peer->StateOffset++;
  }
  /* hardcoded GodFatherStateLast starts here */
  return NET_TCP_EXT_dontgo_e;
}

void _NET_TCP_close_peer_StateLoop(NET_TCP_t *tcp, NET_TCP_peer_t *peer){
  NET_TCP_op_t *opstate = &peer->parent->op[NET_TCP_opid_state_e];
  NET_TCP_layerid_t StateOffset = 0;
  for(; StateOffset < peer->StateOffset; StateOffset++){
    NET_TCP_layer_state_t *layer = &((NET_TCP_layer_state_t *)opstate->layer)[StateOffset];
    NET_TCP_extinfo_t *extinfo = &tcp->ext.info[layer->extid];
    ((NET_TCP_cb_state_t)layer->func)(
      peer,
      tcp->SockData + extinfo->SockDataOffset,
      (uint8_t *)peer + sizeof(NET_TCP_peer_t) + extinfo->PeerDataOffset,
      NET_TCP_state_init_e
    );
  }
  for(; StateOffset < opstate->total; StateOffset++){
    NET_TCP_layer_state_t *layer = &((NET_TCP_layer_state_t *)opstate->layer)[StateOffset];
    NET_TCP_extinfo_t *extinfo = &tcp->ext.info[layer->extid];
    ((NET_TCP_cb_state_t)layer->func)(
      peer,
      tcp->SockData + extinfo->SockDataOffset,
      (uint8_t *)peer + sizeof(NET_TCP_peer_t) + extinfo->PeerDataOffset,
      0
    );
  }
}

/* get socket from peer */
NET_socket_t NET_TCP_gsfp(NET_TCP_peer_t *p){
  NET_socket_t s;
  EV_event_get_socket(&p->event, &s);
  return s;
}

/* internal cleanup for peer */
void _NET_TCP_close_peer(NET_TCP_peer_t *peer){
  _NET_TCP_QueuerList_Close(&peer->ReadQueuerList);
  _NET_TCP_QueuerList_Close(&peer->WriteQueuerList);
  VAS_close(&peer->queue);
  _NET_TCP_close_peer_StateLoop(peer->parent, peer);
  NET_socket_t sock = NET_TCP_gsfp(peer);
  NET_close(&sock);
  A_resize(peer, 0);
}

NET_TCP_layerflag_t NET_TCP_read_loop(
  NET_TCP_peer_t *,
  NET_TCP_QueuerReference_t,
  uint32_t,
  NET_TCP_Queue_t *
);
NET_TCP_layerflag_t NET_TCP_write_loop(
  NET_TCP_peer_t *,
  NET_TCP_QueuerReference_t,
  uint32_t,
  NET_TCP_Queue_t *
);

void NET_TCP_StopConnectTimer(EV_t *listener, NET_TCP_t *tcp, NET_TCP_peer_t *peer);
void NET_TCP_StopPeerTimer(EV_t *listener, NET_TCP_t *tcp, NET_TCP_peer_t *peer);

void _NET_TCP_CloseEstablished(NET_TCP_peer_t *peer){
  if(peer->TimerNode != -1){
    /* if there is timer */
    NET_TCP_StopPeerTimer(peer->parent->listener, peer->parent, peer);
  }

  NET_TCP_Queue_t Queue;
  NET_TCP_read_loop(
    peer,
    NET_TCP_GetReadQueuerReferenceFirst(peer),
    NET_TCP_QueueType_CloseHard,
    &Queue
  );
  NET_TCP_write_loop(
    peer,
    NET_TCP_GetWriteQueuerReferenceFirst(peer),
    NET_TCP_QueueType_CloseHard,
    &Queue
  );

  EV_event_stop(peer->parent->listener, &peer->event);

  _NET_TCP_close_peer(peer);
}

void _NET_TCP_OpenQueuerList(NET_TCP_t *tcp, NET_TCP_peer_t *peer){
  _NET_TCP_QueuerList_Open(&peer->ReadQueuerList);
  _NET_TCP_QueuerList_Open(&peer->WriteQueuerList);

  NET_TCP_QueuerReference_t QueuerReference;
  _NET_TCP_QueuerList_Node_t *QueuerNode;
  NET_TCP_Queuer_t *Queuer;

  QueuerReference = _NET_TCP_QueuerList_NewNodeFirst(&peer->ReadQueuerList);
  QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(&peer->ReadQueuerList, QueuerReference);
  Queuer = &QueuerNode->data.data;
  Queuer->layerid = tcp->GodFatherReadFirst;
  Queuer->QueueFirstNode = -1;
  QueuerReference = _NET_TCP_QueuerList_NewNodeLast(&peer->ReadQueuerList);
  QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(&peer->ReadQueuerList, QueuerReference);
  Queuer = &QueuerNode->data.data;
  Queuer->layerid = tcp->GodFatherReadLast;
  Queuer->QueueLastNode = -1;

  QueuerReference = _NET_TCP_QueuerList_NewNodeFirst(&peer->WriteQueuerList);
  QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(&peer->WriteQueuerList, QueuerReference);
  Queuer = &QueuerNode->data.data;
  Queuer->layerid = tcp->GodFatherWriteFirst;
  Queuer->QueueFirstNode = -1;
  QueuerReference = _NET_TCP_QueuerList_NewNodeLast(&peer->WriteQueuerList);
  QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(&peer->WriteQueuerList, QueuerReference);
  Queuer = &QueuerNode->data.data;
  Queuer->layerid = tcp->GodFatherWriteLast;
  Queuer->QueueFirstNode = -1;
}

void _NET_TCP_GodFatherRead_evcb(EV_t *, EV_event_t *, uint32_t);

void _NET_TCP_EVPeerTimer_cb(EV_t *listener, EV_timer_t *timer){
  NET_TCP_t *tcp = OFFSETLESS(timer, NET_TCP_t, EVPeerTimer);
  begin:;
  VAS_node_t node = VAS_GetNodeFirst(&tcp->PeerTimerList);
  _NET_TCP_PeerTimer_t *PeerTimer = (_NET_TCP_PeerTimer_t *)VAS_out(&tcp->PeerTimerList, node);
  uint64_t now = EV_nowi(listener);
  if(now < PeerTimer->NextTime){
    EV_timer_stop(listener, &tcp->EVPeerTimer);
    EV_timer_init(&tcp->EVPeerTimer, (f64_t)tcp->PeerTimeoutTime / 1000000000, _NET_TCP_EVPeerTimer_cb);
    EV_timer_start(listener, &tcp->EVPeerTimer);
    return;
  }
  _NET_TCP_CloseEstablished(PeerTimer->peer);
  if(!VAS_usage(&tcp->PeerTimerList)){
    return;
  }
  goto begin;
}
void _NET_TCP_CalculatePeerTimer(EV_t *listener, NET_TCP_t *tcp){
  begin:;
  VAS_node_t node = VAS_GetNodeFirst(&tcp->PeerTimerList);
  _NET_TCP_PeerTimer_t *PeerTimer = (_NET_TCP_PeerTimer_t *)VAS_out(&tcp->PeerTimerList, node);
  uint64_t now = EV_nowi(listener);
  if(now >= PeerTimer->NextTime){
    PeerTimer->peer->TimerNode = -1;
    _NET_TCP_CloseEstablished(PeerTimer->peer);
    VAS_unlink(&tcp->PeerTimerList, node);
    if(!VAS_usage(&tcp->PeerTimerList)){
      return;
    }
    goto begin;
  }
  else{
    f64_t Time = PeerTimer->NextTime - now;
    Time /= 1000000000;
    EV_timer_init(&tcp->EVPeerTimer, Time, _NET_TCP_EVPeerTimer_cb);
    EV_timer_start(listener, &tcp->EVPeerTimer);
  }
}
void _NET_TCP_UnlinkThenReCalculatePeerTimer(EV_t *listener, NET_TCP_t *tcp){
  EV_timer_stop(listener, &tcp->EVPeerTimer);
  VAS_unlink(&tcp->PeerTimerList, VAS_GetNodeFirst(&tcp->PeerTimerList));
  if(VAS_usage(&tcp->PeerTimerList)){
    _NET_TCP_CalculatePeerTimer(listener, tcp);
  }
  else{
    /* there is no timer to calculate */
  }
}
void NET_TCP_StartPeerTimer(EV_t *listener, NET_TCP_t *tcp, NET_TCP_peer_t *peer, NET_TCP_extid_t extid){
  VAS_node_t node = VAS_NewNodeLast(&tcp->PeerTimerList);
  _NET_TCP_PeerTimer_t *PeerTimer = (_NET_TCP_PeerTimer_t *)VAS_out(&tcp->PeerTimerList, node);
  peer->TimerNode = node;
  PeerTimer->NextTime = EV_nowi(listener) + tcp->PeerTimeoutTime;
  PeerTimer->peer = peer;
  PeerTimer->extid = extid;
  if(VAS_usage(&tcp->PeerTimerList) == 1){
    /* if its first timer */
    _NET_TCP_CalculatePeerTimer(listener, tcp);
  }
}
void NET_TCP_UpdatePeerTimer(EV_t *listener, NET_TCP_t *tcp, NET_TCP_peer_t *peer){
  VAS_node_t node = peer->TimerNode;
  _NET_TCP_PeerTimer_t *PeerTimer = (_NET_TCP_PeerTimer_t *)VAS_out(&tcp->PeerTimerList, node);
  NET_TCP_extid_t extid = PeerTimer->extid;
  if(node == VAS_GetNodeFirst(&tcp->PeerTimerList)){
    EV_timer_stop(listener, &tcp->EVPeerTimer);
  }
  VAS_unlink(&tcp->PeerTimerList, node);
  NET_TCP_StartPeerTimer(listener, tcp, peer, extid);
}
NET_TCP_extid_t NET_TCP_QueryPeerTimer(NET_TCP_t *tcp, NET_TCP_peer_t *peer){
  _NET_TCP_PeerTimer_t *PeerTimer = (_NET_TCP_PeerTimer_t *)VAS_out(&tcp->PeerTimerList, peer->TimerNode);
  return PeerTimer->extid;
}
void NET_TCP_StopPeerTimer(EV_t *listener, NET_TCP_t *tcp, NET_TCP_peer_t *peer){
  VAS_node_t node = peer->TimerNode;
  peer->TimerNode = -1;
  if(node == VAS_GetNodeFirst(&tcp->PeerTimerList)){
    /* if this node is first node */
    _NET_TCP_UnlinkThenReCalculatePeerTimer(listener, tcp);
  }
  else{
    VAS_unlink(&tcp->PeerTimerList, node);
  }
}

void _NET_TCP_accept(EV_t *listener, EV_event_t *ev, uint32_t flag){
  NET_TCP_t *tcp = OFFSETLESS(ev, NET_TCP_t, ev);

  NET_addr_t sdstaddr;
  NET_socket_t sock;
  NET_socket_t accepter_socket;
  EV_event_get_socket(ev, &accepter_socket);
  sint32_t err = NET_accept(&accepter_socket, &sdstaddr, SOCK_NONBLOCK, &sock);
  if(err){
    __abort();
  }

  NET_TCP_peer_t *peer = (NET_TCP_peer_t *)A_resize(0, sizeof(NET_TCP_peer_t) + tcp->PeerDataSize);

  peer->parent = tcp;

  #if NET_TCP_set_Storesdstaddr
    peer->sdstaddr = sdstaddr;
  #endif

  peer->StateOffset = 0;

  peer->flag = 0;

  _NET_TCP_OpenQueuerList(tcp, peer);
  VAS_open(&peer->queue, sizeof(NET_TCP_QueueElement_t));

  EV_event_init_socket(&peer->event, &sock, _NET_TCP_GodFatherRead_evcb, EV_READ);
  peer->TimerNode = -1;

  NET_TCP_StateLoop(peer);
}

sint32_t NET_TCP_listen(NET_TCP_t *tcp){
  sint32_t err;
  err = NET_socket2(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP, &tcp->sock);
  if(err){
    return err;
  }
  err = NET_setsockopt(&tcp->sock, SOL_SOCKET, NET_reuseport, 1);
  if(err){
    NET_close(&tcp->sock);
    return err;
  }
  err = NET_bind(&tcp->sock, &tcp->ssrcaddr);
  if(err){
    NET_close(&tcp->sock);
    return err;
  }
  err = NET_listen(&tcp->sock);
  if(err){
    NET_close(&tcp->sock);
    return err;
  }

  EV_event_init_socket(&tcp->ev, &tcp->sock, _NET_TCP_accept, EV_READ);

  return 0;
}

void _NET_TCP_EVConnectTimer_cb(EV_t *listener, EV_timer_t *timer){
  NET_TCP_t *tcp = OFFSETLESS(timer, NET_TCP_t, EVConnectTimer);
  begin:;
  VAS_node_t node = VAS_GetNodeFirst(&tcp->ConnectTimerList);
  _NET_TCP_ConnectTimer_t *ConnectTimer = (_NET_TCP_ConnectTimer_t *)VAS_out(&tcp->ConnectTimerList, node);
  uint64_t now = EV_nowi(listener);
  if(now < ConnectTimer->NextTime){
    EV_timer_stop(listener, &tcp->EVConnectTimer);
    EV_timer_init(
      &tcp->EVConnectTimer,
      (f64_t)tcp->ConnectTimeoutTime / 1000000000,
      _NET_TCP_EVConnectTimer_cb
    );
    EV_timer_start(listener, &tcp->EVConnectTimer);
    return;
  }

  EV_event_stop(listener, &ConnectTimer->peer->event);
  NET_TCP_StopConnectTimer(listener, tcp, ConnectTimer->peer);
  _NET_TCP_close_peer_StateLoop(tcp, ConnectTimer->peer);
  NET_socket_t peer_socket = NET_TCP_gsfp(ConnectTimer->peer);
  NET_close(&peer_socket);
  A_resize(ConnectTimer->peer, 0);

  if(VAS_usage(&tcp->ConnectTimerList)){
    goto begin;
  }
}
void _NET_TCP_CalculateConnectTimer(EV_t *listener, NET_TCP_t *tcp){
  begin:;
  VAS_node_t node = VAS_GetNodeFirst(&tcp->ConnectTimerList);
  _NET_TCP_ConnectTimer_t *ConnectTimer = (_NET_TCP_ConnectTimer_t *)VAS_out(&tcp->ConnectTimerList, node);
  uint64_t now = EV_nowi(listener);
  if(now >= ConnectTimer->NextTime){
    EV_event_stop(listener, &ConnectTimer->peer->event);
    _NET_TCP_close_peer_StateLoop(tcp, ConnectTimer->peer);
    NET_socket_t peer_socket = NET_TCP_gsfp(ConnectTimer->peer);
    NET_close(&peer_socket);
    A_resize(ConnectTimer->peer, 0);
    VAS_unlink(&tcp->ConnectTimerList, node);
    if(!VAS_usage(&tcp->ConnectTimerList)){
      return;
    }
    goto begin;
  }
  else{
    f64_t Time = ConnectTimer->NextTime - now;
    Time /= 1000000000;
    EV_timer_init(&tcp->EVConnectTimer, Time, _NET_TCP_EVConnectTimer_cb);
    EV_timer_start(listener, &tcp->EVConnectTimer);
  }
}
void _NET_TCP_UnlinkThenReCalculateConnectTimer(EV_t *listener, NET_TCP_t *tcp){
  EV_timer_stop(listener, &tcp->EVConnectTimer);
  VAS_unlink(&tcp->ConnectTimerList, VAS_GetNodeFirst(&tcp->ConnectTimerList));
  if(VAS_usage(&tcp->ConnectTimerList)){
    _NET_TCP_CalculateConnectTimer(listener, tcp);
  }
  else{
    /* there is no timer to calculate */
  }
}
void NET_TCP_StartConnectTimer(EV_t *listener, NET_TCP_t *tcp, NET_TCP_peer_t *peer){
  VAS_node_t node = VAS_NewNodeLast(&tcp->ConnectTimerList);
  _NET_TCP_ConnectTimer_t *ConnectTimer = (_NET_TCP_ConnectTimer_t *)VAS_out(&tcp->ConnectTimerList, node);
  peer->TimerNode = node;
  ConnectTimer->NextTime = EV_nowi(listener) + tcp->ConnectTimeoutTime;
  ConnectTimer->peer = peer;
  if(VAS_usage(&tcp->ConnectTimerList) == 1){
    /* if its first timer */
    _NET_TCP_CalculateConnectTimer(listener, tcp);
  }
}
void NET_TCP_StopConnectTimer(EV_t *listener, NET_TCP_t *tcp, NET_TCP_peer_t *peer){
  VAS_node_t node = peer->TimerNode;
  peer->TimerNode = -1;
  if(node == VAS_GetNodeFirst(&tcp->ConnectTimerList)){
    /* if this node is first node */
    _NET_TCP_UnlinkThenReCalculateConnectTimer(listener, tcp);
  }
  else{
    VAS_unlink(&tcp->ConnectTimerList, node);
  }
}

void _NET_TCP_connect_CB(EV_t *listener, EV_event_t *event, uint32_t flag){
  NET_TCP_peer_t *peer = OFFSETLESS(event, NET_TCP_peer_t, event);
  NET_TCP_t *tcp = peer->parent;
  EV_event_stop(listener, event);
  NET_TCP_StopConnectTimer(listener, tcp, peer);

  NET_socket_t sock = NET_TCP_gsfp(peer);
  sint32_t err;
  if(NET_getsockopt(&sock, SOL_SOCKET, SO_ERROR, &err) != 0){
    __abort();
  }
  if(err){
    _NET_TCP_close_peer_StateLoop(tcp, peer);
    NET_close(&sock);
    A_resize(peer, 0);
    return;
  }

  _NET_TCP_OpenQueuerList(tcp, peer);
  VAS_open(&peer->queue, sizeof(NET_TCP_QueueElement_t));

  EV_event_init_socket(event, &sock, _NET_TCP_GodFatherRead_evcb, EV_READ);

  NET_TCP_StateLoop(peer);
}

void _NET_TCP_CloseConnect(NET_TCP_t *tcp, NET_TCP_peer_t *peer){
  EV_t *listener = tcp->listener;
  EV_event_stop(listener, &peer->event);
  NET_TCP_StopConnectTimer(listener, tcp, peer);
  NET_socket_t sock = NET_TCP_gsfp(peer);
  NET_close(&sock);
  A_resize(peer, 0);
}

void NET_TCP_CloseHard_MayConnecting(NET_TCP_peer_t *peer){
  if(EV_event_get_cb(&peer->event) == _NET_TCP_connect_CB){
    _NET_TCP_CloseConnect(peer->parent, peer);
  }
  else{
    _NET_TCP_CloseEstablished(peer);
  }
}

void NET_TCP_CloseHard(NET_TCP_peer_t *peer){
  _NET_TCP_CloseEstablished(peer);
}
void NET_TCP_CloseSoft(NET_TCP_peer_t *peer){
  NET_TCP_Queue_t Queue;
  NET_TCP_write_loop(
    peer,
    NET_TCP_GetWriteQueuerReferenceFirst(peer),
    NET_TCP_QueueType_CloseIfGodFather,
    &Queue
  );
}

void NET_TCP_CloseSoft_MayConnecting(NET_TCP_peer_t *peer){
  if(EV_event_get_cb(&peer->event) == _NET_TCP_connect_CB){
    _NET_TCP_CloseConnect(peer->parent, peer);
  }
  else{
    NET_TCP_CloseSoft(peer);
  }
}

sint32_t _NET_TCP_connect(
  NET_TCP_t *tcp,
  NET_TCP_peer_t **_peer,
  NET_addr_t *addr,
  NET_TCP_sockopt_t *sockopt,
  uint32_t sockoptSize
){
  sint32_t err;
  NET_socket_t sock;
  err = NET_socket2(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, IPPROTO_TCP, &sock);
  if(err){
    return err;
  }

  err = NET_setsockopt(&sock, SOL_SOCKET, NET_reuseport, 1);
  if(err){
    NET_close(&sock);
    return err;
  }

  err = NET_bind(&sock, &tcp->ssrcaddr);
  if(err){
    NET_close(&sock);
    return err;
  }

  for(uint32_t i = 0; i < sockoptSize; i++){
    err = NET_setsockopt(&sock, sockopt[i].level, sockopt[i].optname, sockopt[i].value);
    if(err){
      NET_close(&sock);
      return err;
    }
  }

  NET_TCP_peer_t *peer = (NET_TCP_peer_t *)A_resize(0, sizeof(NET_TCP_peer_t) + tcp->PeerDataSize);
  peer->parent = tcp;
  EV_event_init_socket(&peer->event, &sock, _NET_TCP_connect_CB, EV_WRITE);

  peer->StateOffset = 0;

  peer->flag = NET_TCP_PClient_e;

  NET_addr_t *paddr = addr;
  NET_TCP_op_t *opconnect = &tcp->op[NET_TCP_opid_connect_e];
  for(uintptr_t oconnectoff = 0; oconnectoff < opconnect->total; oconnectoff++){
    NET_TCP_layer_connect_t *layer = &((NET_TCP_layer_connect_t *)opconnect->layer)[oconnectoff];
    NET_TCP_extinfo_t *extinfo = &tcp->ext.info[layer->extid];
    ((NET_TCP_cb_connect_t)layer->func)(
      peer,
      tcp->SockData + extinfo->SockDataOffset,
      (uint8_t *)peer + sizeof(NET_TCP_peer_t) + extinfo->PeerDataOffset,
      &paddr
    );
  }

  #if NET_TCP_set_Storesdstaddr
    peer->sdstaddr = *addr;
  #endif

  err = NET_connect(&sock, addr);
  if(err){
    if(err != -EINPROGRESS){
      NET_close(&sock);
      A_resize(peer, 0);
      return err;
    }
  }

  *_peer = peer;

  return 0;
}

sint32_t NET_TCP_connect(
  NET_TCP_t *tcp,
  NET_TCP_peer_t **_peer,
  NET_addr_t *addr,
  NET_TCP_sockopt_t *sockopt,
  uint32_t sockoptSize
){
  sint32_t r = _NET_TCP_connect(tcp, _peer, addr, sockopt, sockoptSize);
  if(r != 0){
    return r;
  }

  NET_TCP_peer_t *peer = *_peer;

  EV_event_start(tcp->listener, &peer->event);
  NET_TCP_StartConnectTimer(tcp->listener, tcp, peer);

  return 0;
}
void _NET_TCP_connect_ThreadSafe_StartConnectTimer(EV_t *listener, NET_TCP_peer_t *peer){
  NET_TCP_StartConnectTimer(listener, peer->parent, peer);
}
sint32_t NET_TCP_connect_ThreadSafe(
  NET_TCP_t *tcp,
  NET_TCP_peer_t **_peer,
  NET_addr_t *addr,
  NET_TCP_sockopt_t *sockopt,
  uint32_t sockoptSize
){
  sint32_t r = _NET_TCP_connect(tcp, _peer, addr, sockopt, sockoptSize);
  if(r != 0){
    return r;
  }

  NET_TCP_peer_t *peer = *_peer;

  EV_queue_lock(tcp->listener);
  EV_queue_add(tcp->listener, (EV_queue_cb_t)EV_event_start, &peer->event);
  EV_queue_add(tcp->listener, (EV_queue_cb_t)_NET_TCP_connect_ThreadSafe_StartConnectTimer, peer);
  EV_queue_unlock(tcp->listener);
  EV_queue_signal(tcp->listener);

  return 0;
}

sint32_t NET_TCP_MakePeerNoDelay(NET_TCP_peer_t *peer){
  NET_socket_t peer_socket = NET_TCP_gsfp(peer);
  return NET_setsockopt(&peer_socket, IPPROTO_TCP, TCP_NODELAY, 1);
}
