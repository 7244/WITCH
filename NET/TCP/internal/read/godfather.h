/* read godfather included first so lets declare it */
void _NET_TCP_write_godfather_queue(EV_t *, NET_TCP_peer_t *);

uint32_t _NET_TCP_GodFatherReadFirst(
  NET_TCP_peer_t *peer,
  uint8_t *sockdata,
  uint8_t *peerdata,
  NET_TCP_QueuerReference_t QueuerReference,
  uint32_t *type,
  NET_TCP_Queue_t *Queue
){
  switch(*type){
    case NET_TCP_QueueType_DynamicPointer:
    case NET_TCP_QueueType_SpecialPointer:
    case NET_TCP_QueueType_SignedSpecialPointer:
    case NET_TCP_QueueType_PeerEvent:
    case NET_TCP_QueueType_File:
    case NET_TCP_QueueType_CloseHard:
    {
      return NET_TCP_EXT_pass_e;
    }
  }
}

uint32_t _NET_TCP_GodFatherReadLast(
  NET_TCP_peer_t *peer,
  uint8_t *sockdata,
  uint8_t *peerdata,
  NET_TCP_QueuerReference_t QueuerReference,
  uint32_t *type,
  NET_TCP_Queue_t *Queue
){
  switch(*type){
    case NET_TCP_QueueType_DynamicPointer:{
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_SpecialPointer:{
      __abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_SignedSpecialPointer:{
      __abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_PeerEvent:{
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_File:{
      __abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_CloseHard:{
      return NET_TCP_EXT_dontgo_e;
    }
  }
}

void _NET_TCP_GodFatherRead_evcb(EV_t *listener, EV_event_t *event, uint32_t evflag){
  NET_TCP_peer_t *peer = OFFSETLESS(event, NET_TCP_peer_t, event);
  if(evflag & EV_READ) do{
    NET_TCP_Queue_t Queue;
    NET_TCP_read_loop(
      peer,
      NET_TCP_GetReadQueuerReferenceFirst(peer),
      NET_TCP_QueueType_PeerEvent,
      &Queue
    );
  }while(0);
  if(evflag & EV_WRITE){
    _NET_TCP_write_godfather_queue(listener, peer);
  }
}
