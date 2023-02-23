A_resize_t NET_TCP_write_GetResize_SpecialPointer(
  NET_TCP_t *tcp,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference
){
  _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerReference);
  NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
  NET_TCP_layerid_t layerid = Queuer->layerid;
  return ((NET_TCP_layer_write_t *)tcp->op[NET_TCP_opid_write_e].layer)[layerid].resize_SpecialPointer;
}
A_resize_t NET_TCP_write_GetNextResize_SpecialPointer(
  NET_TCP_t *tcp,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference
){
  _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerReference);
  _NET_TCP_QueuerList_Node_t *NextQueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerNode->NextNodeReference);
  NET_TCP_Queuer_t *NextQueuer = &NextQueuerNode->data.data;
  NET_TCP_layerid_t layerid = NextQueuer->layerid;
  return ((NET_TCP_layer_write_t *)tcp->op[NET_TCP_opid_write_e].layer)[layerid].resize_SpecialPointer;
}

A_resize_t NET_TCP_write_GetResize_SignedSpecialPointer(
  NET_TCP_t *tcp,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference
){
  _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerReference);
  NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
  NET_TCP_layerid_t layerid = Queuer->layerid;
  return ((NET_TCP_layer_write_t *)tcp->op[NET_TCP_opid_write_e].layer)[layerid].resize_SignedSpecialPointer;
}
A_resize_t NET_TCP_write_GetNextResize_SignedSpecialPointer(
  NET_TCP_t *tcp,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference
){
  _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerReference);
  _NET_TCP_QueuerList_Node_t *NextQueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerNode->NextNodeReference);
  NET_TCP_Queuer_t *NextQueuer = &NextQueuerNode->data.data;
  NET_TCP_layerid_t layerid = NextQueuer->layerid;
  return ((NET_TCP_layer_write_t *)tcp->op[NET_TCP_opid_write_e].layer)[layerid].resize_SignedSpecialPointer;
}

bool NET_TCP_EXT_write_add(
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference,
  const NET_TCP_QueueElement_t *QueueElement
){
  if(peer->TimerNode != -1){
    NET_TCP_UpdatePeerTimer(peer->parent->listener, peer->parent, peer);
  }
  _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerReference);
  NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
  if(Queuer->QueueFirstNode == -1){
    VAS_node_t node = VAS_NewNodeFirst(&peer->queue);
    *(NET_TCP_QueueElement_t *)VAS_out(&peer->queue, node) = *QueueElement;
    Queuer->QueueFirstNode = node;
    Queuer->QueueLastNode = node;
    return 1;
  }
  else{
    VAS_node_t node = VAS_NewNode(&peer->queue);
    *(NET_TCP_QueueElement_t *)VAS_out(&peer->queue, node) = *QueueElement;
    VAS_link0(&peer->queue, Queuer->QueueLastNode, node);
    Queuer->QueueLastNode = node;
    return 0;
  }
}
void NET_TCP_EXT_write_add2(
  NET_TCP_peer_t *peer,
  NET_TCP_Queuer_t *Queuer,
  const NET_TCP_QueueElement_t *QueueElement
){
  if(peer->TimerNode != -1){
    NET_TCP_UpdatePeerTimer(peer->parent->listener, peer->parent, peer);
  }
  if(Queuer->QueueFirstNode == -1){
    VAS_node_t node = VAS_NewNodeFirst(&peer->queue);
    *(NET_TCP_QueueElement_t *)VAS_out(&peer->queue, node) = *QueueElement;
    Queuer->QueueFirstNode = node;
    Queuer->QueueLastNode = node;
  }
  else{
    VAS_node_t node = VAS_NewNode(&peer->queue);
    *(NET_TCP_QueueElement_t *)VAS_out(&peer->queue, node) = *QueueElement;
    VAS_link0(&peer->queue, Queuer->QueueLastNode, node);
    Queuer->QueueLastNode = node;
  }
}

bool NET_TCP_EXT_write_is_active(NET_TCP_peer_t *peer, NET_TCP_QueuerReference_t QueuerReference){
  _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerReference);
  NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
  if(Queuer->QueueFirstNode == -1){
    return 1;
  }
  else{
    return 0;
  }
}
bool NET_TCP_EXT_write_is_active2(NET_TCP_peer_t *peer, NET_TCP_Queuer_t *Queuer){
  return Queuer->QueueFirstNode != -1;
}

bool NET_TCP_EXT_write_get(
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference,
  NET_TCP_QueueElement_t **QueueElement
){
  _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerReference);
  NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
  if(Queuer->QueueFirstNode == -1){
    return 1;
  }
  else{
    *QueueElement = (NET_TCP_QueueElement_t *)VAS_out(&peer->queue, Queuer->QueueFirstNode);
    return 0;
  }
}
void NET_TCP_EXT_write_get2(NET_TCP_peer_t *peer, NET_TCP_Queuer_t *Queuer, NET_TCP_QueueElement_t **QueueElement){
  *QueueElement = (NET_TCP_QueueElement_t *)VAS_out(&peer->queue, Queuer->QueueFirstNode);
}

bool NET_TCP_EXT_write_del(NET_TCP_peer_t *peer, NET_TCP_QueuerReference_t QueuerReference){
  if(peer->TimerNode != -1){
    NET_TCP_UpdatePeerTimer(peer->parent->listener, peer->parent, peer);
  }
  _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
    &peer->WriteQueuerList,
    QueuerReference);
  NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
  VAS_node_t next = *VAS_road0(&peer->queue, Queuer->QueueFirstNode);
  VAS_unlink(&peer->queue, Queuer->QueueFirstNode);
  if(Queuer->QueueFirstNode == Queuer->QueueLastNode){
    Queuer->QueueFirstNode = -1;
    return 1;
  }
  else{
    Queuer->QueueFirstNode = next;
    return 0;
  }
}
void NET_TCP_EXT_write_del2(NET_TCP_peer_t *peer, NET_TCP_Queuer_t *Queuer){
  if(peer->TimerNode != -1){
    NET_TCP_UpdatePeerTimer(peer->parent->listener, peer->parent, peer);
  }
  VAS_node_t next = *VAS_road0(&peer->queue, Queuer->QueueFirstNode);
  VAS_unlink(&peer->queue, Queuer->QueueFirstNode);
  if(Queuer->QueueFirstNode == Queuer->QueueLastNode){
    Queuer->QueueFirstNode = -1;
  }
  else{
    Queuer->QueueFirstNode = next;
  }
}

NET_TCP_layerflag_t NET_TCP_write_loop(
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference,
  uint32_t type,
  NET_TCP_Queue_t *Queue
){
  NET_TCP_op_t *opwrite = &peer->parent->op[NET_TCP_opid_write_e];
  while(1){
    _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
      &peer->WriteQueuerList,
      QueuerReference);
    NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
    NET_TCP_layer_write_t *layer = &((NET_TCP_layer_write_t *)opwrite->layer)[Queuer->layerid];
    NET_TCP_extinfo_t *extinfo = &peer->parent->ext.info[layer->extid];
    NET_TCP_layerflag_t layerflag = ((NET_TCP_cb_write_t)layer->func)(
      peer,
      peer->parent->SockData + extinfo->SockDataOffset,
      (uint8_t *)peer + sizeof(NET_TCP_peer_t) + extinfo->PeerDataOffset,
      QueuerReference,
      &type,
      Queue
    );
    if(layerflag & NET_TCP_EXT_PeerIsClosed_e){
      return NET_TCP_EXT_PeerIsClosed_e;
    }
    if(layerflag & NET_TCP_EXT_dontgo_e){
      break;
    }
    /* TODO layerflag cant be other than 0 and dontgo */
    QueuerReference = QueuerNode->NextNodeReference;
  }
  return 0;
}
