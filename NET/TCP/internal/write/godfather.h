enum{
  #define _SpecialPointer_e _NET_TCP_GodFather_SpecialPointer_e
  _SpecialPointer_e,
  #define _SignedSpecialPointer_e _NET_TCP_GodFather_SignedSpecialPointer_e
  _SignedSpecialPointer_e,
  #define _ReadyFile_e _NET_TCP_GodFather_ReadyFile_e
  _ReadyFile_e,
  #define _CloseIfGodFather_e _NET_TCP_GodFather_CloseIfGodFather_e
  _CloseIfGodFather_e
};

#define _SpecialPointer_t _NET_TCP_GodFather_SpecialPointer_t
typedef struct{
  uintptr_t index;
  uintptr_t size;
}_SpecialPointer_t;
#define _SignedSpecialPointer_t _NET_TCP_GodFather_SignedSpecialPointer_t
typedef struct{
  uintptr_t index;
  uintptr_t size;
  NET_TCP_QueuerReference_t SignerQueuerReference;
  uint64_t UniqueNumber;
}_SignedSpecialPointer_t;
#define _ReadyFile_t _NET_TCP_GodFather_ReadyFile_t
typedef struct{
  EV_io_sendfile_t ev;
  bool ran;
  NET_TCP_peer_t *peer;
  void *userdata;
  NET_TCP_Queue_File_cb_t cb;
}_ReadyFile_t;

void
_NET_TCP_GodFatherWriteLast_ClearQueue(
  EV_t *listener,
  NET_TCP_t *tcp,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference
){
  NET_TCP_QueueElement_t *QueueElement;
  while(!NET_TCP_EXT_write_get(peer, QueuerReference, &QueueElement)){
    switch(QueueElement->t){
      case _SpecialPointer_e:{
        A_resize(QueueElement->tdata, 0);
        break;
      }
      case _ReadyFile_e:{
        _ReadyFile_t *ReadyFile = (_ReadyFile_t *)QueueElement->tdata;
        if(ReadyFile->ran){
          EV_io_sendfile_stop(listener, &ReadyFile->ev);
        }
        else{
          A_resize(QueueElement->tdata, 0);
        }
        break;
      }
    }
    NET_TCP_EXT_write_del(peer, QueuerReference);
  }
}

void _NET_TCP_write_godfather_file_cb(EV_t *listener, EV_io_sendfile_t *req, sint32_t err){
  _ReadyFile_t *ReadyFile = OFFSETLESS(req, _ReadyFile_t, ev);
  NET_TCP_peer_t *peer = ReadyFile->peer;
  NET_TCP_QueuerReference_t QueuerReference = _NET_TCP_QueuerList_GetNodeLast(&peer->WriteQueuerList);

  NET_TCP_EXT_write_del(peer, QueuerReference);

  ReadyFile->cb(peer, ReadyFile->ev.infd, err, ReadyFile->userdata);

  A_resize(ReadyFile, 0);

  if(err == -ECANCELED){
    return;
  }

  if(!NET_TCP_EXT_write_is_active(peer, QueuerReference)){
    EV_ev_flag_add(listener, &peer->event, EV_WRITE);
  }
}

void _NET_TCP_write_godfather_queue(EV_t *listener, NET_TCP_peer_t *peer){
  EV_event_t *event = &peer->event;
  NET_TCP_t *tcp = peer->parent;
  NET_TCP_op_t *opwrite = &tcp->op[NET_TCP_opid_write_e];
  NET_TCP_QueuerReference_t QueuerReference = _NET_TCP_QueuerList_GetNodeLast(&peer->WriteQueuerList);
  NET_TCP_QueueElement_t *QueueElement;
  while(!NET_TCP_EXT_write_get(peer, QueuerReference, &QueueElement)){
    switch(QueueElement->t){
      case _SpecialPointer_e:{
        _SpecialPointer_t *SpecialPointer = (_SpecialPointer_t *)QueueElement->tdata;
        uint8_t *ptr = QueueElement->tdata + sizeof(_SpecialPointer_t);
        IO_fd_t peer_fd;
        EV_event_get_fd(event, &peer_fd);
        IO_ssize_t len = IO_write(
          &peer_fd,
          ptr + SpecialPointer->index,
          SpecialPointer->size - SpecialPointer->index
        );
        if(len < 0){
          NET_TCP_CloseHard(peer);
          return;
        }
        SpecialPointer->index += len;
        if(SpecialPointer->index != SpecialPointer->size){
          return;
        }
        A_resize(SpecialPointer, 0);
        if(NET_TCP_EXT_write_del(peer, QueuerReference)){
          EV_ev_flag_rm(listener, event, EV_WRITE);
          return;
        }
        break;
      }
      case _SignedSpecialPointer_e:{
        _SignedSpecialPointer_t *SignedSpecialPointer = (_SignedSpecialPointer_t *)QueueElement->tdata;
        uint8_t *ptr = QueueElement->tdata + sizeof(_SignedSpecialPointer_t);
        IO_fd_t peer_fd;
        EV_event_get_fd(event, &peer_fd);
        IO_ssize_t len = IO_write(
          &peer_fd,
          ptr + SignedSpecialPointer->index,
          SignedSpecialPointer->size - SignedSpecialPointer->index
        );
        if(len < 0){
          NET_TCP_CloseHard(peer);
          return;
        }
        SignedSpecialPointer->index += len;
        if(SignedSpecialPointer->index != SignedSpecialPointer->size){
          return;
        }

        /* Queuer able to stop itself after sending SignedSpecialPointer */
        /* so lets check if its valid */
        if(NET_TCP_IsWriteQueuerReferenceValid(peer, SignedSpecialPointer->SignerQueuerReference)){
          _NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
            &peer->WriteQueuerList,
            SignedSpecialPointer->SignerQueuerReference);
          NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
          NET_TCP_layerid_t layerid = Queuer->layerid;
          NET_TCP_CBWriteSignConsumed CBWriteSignConsumed = 
            ((NET_TCP_layer_write_t *)tcp->op[NET_TCP_opid_write_e].layer)[layerid].CBWriteSignConsumed;
          CBWriteSignConsumed(peer, Queuer, layerid, SignedSpecialPointer->UniqueNumber);
        }

        A_resize(SignedSpecialPointer, 0);
        if(NET_TCP_EXT_write_del(peer, QueuerReference)){
          EV_ev_flag_rm(listener, event, EV_WRITE);
          return;
        }
        break;
      }
      case _ReadyFile_e:{
        EV_ev_flag_rm(listener, event, EV_WRITE);
        _ReadyFile_t *ReadyFile = (_ReadyFile_t *)QueueElement->tdata;
        ReadyFile->ran = 1;
        sint32_t err = EV_io_sendfile_start(listener, &ReadyFile->ev);
        if(err < 0){
          ReadyFile->ran = 0;
          NET_TCP_CloseHard(peer);
        }
        return;
      }
      case _CloseIfGodFather_e:{
        NET_TCP_CloseHard(peer);
        return;
      }
    }
  }
}

uint8_t *_NET_TCP_write_godfather_resize_SpecialPointer(void *ptr, uintptr_t size){
  if(size){
    if(ptr){
      ptr = A_resize((uint8_t *)ptr - sizeof(_SpecialPointer_t), sizeof(_SpecialPointer_t) + size);
      return (uint8_t *)ptr + sizeof(_SpecialPointer_t);
    }
    else{
      ptr = A_resize(0, sizeof(_SpecialPointer_t) + size);
      return (uint8_t *)ptr + sizeof(_SpecialPointer_t);
    }
  }
  else{
    return A_resize(ptr, 0);
  }
}

uint8_t *_NET_TCP_write_godfather_resize_SignedSpecialPointer(void *ptr, uintptr_t size){
  if(size){
    if(ptr){
      ptr = A_resize((uint8_t *)ptr - sizeof(_SignedSpecialPointer_t), sizeof(_SignedSpecialPointer_t) + size);
      return (uint8_t *)ptr + sizeof(_SignedSpecialPointer_t);
    }
    else{
      ptr = A_resize(0, sizeof(_SignedSpecialPointer_t) + size);
      return (uint8_t *)ptr + sizeof(_SignedSpecialPointer_t);
    }
  }
  else{
    return A_resize(ptr, 0);
  }
}

uint32_t _NET_TCP_GodFatherWriteFirst(
  NET_TCP_peer_t *peer,
  uint8_t *sockdata,
  uint8_t *peerdata,
  NET_TCP_QueuerReference_t QueuerReference,
  uint32_t *type,
  NET_TCP_Queue_t *Queue
){
  switch(*type){
    case NET_TCP_QueueType_DynamicPointer_e:
    case NET_TCP_QueueType_SpecialPointer_e:
    case NET_TCP_QueueType_SignedSpecialPointer_e:
    case NET_TCP_QueueType_PeerEvent_e:
    case NET_TCP_QueueType_File_e:
    case NET_TCP_QueueType_CloseIfGodFather_e:
    case NET_TCP_QueueType_CloseHard_e:
    {
      return NET_TCP_EXT_pass_e;
    }
  }
}

uint32_t _NET_TCP_GodFatherWriteLast(
  NET_TCP_peer_t *peer,
  uint8_t *sockdata,
  uint8_t *peerdata,
  NET_TCP_QueuerReference_t QueuerReference,
  uint32_t *type,
  NET_TCP_Queue_t *Queue
){
  NET_TCP_t *tcp = peer->parent;
  EV_t *listener = tcp->listener;
  switch(*type){
    case NET_TCP_QueueType_DynamicPointer_e:{
      NET_TCP_QueueElement_t QueueElement;
      QueueElement.t = _SpecialPointer_e;
      void *ptr = Queue->DynamicPointer.ptr;
      uintptr_t size = Queue->DynamicPointer.size;
      QueueElement.tdata = A_resize(0, sizeof(_SpecialPointer_t) + size);
      _SpecialPointer_t *SpecialPointer = (_SpecialPointer_t *)QueueElement.tdata;
      SpecialPointer->index = 0;
      SpecialPointer->size = size;
      MEM_copy(ptr, QueueElement.tdata + sizeof(_SpecialPointer_t), size);
      if(NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement)){
        EV_ev_flag_add(listener, &peer->event, EV_WRITE);
      }
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_SpecialPointer_e:{
      NET_TCP_QueueElement_t QueueElement;
      QueueElement.t = _SpecialPointer_e;
      void *ptr = Queue->SpecialPointer.ptr;
      uintptr_t size = Queue->SpecialPointer.size;
      QueueElement.tdata = (uint8_t *)ptr - sizeof(_SpecialPointer_t);
      _SpecialPointer_t *SpecialPointer = (_SpecialPointer_t *)QueueElement.tdata;
      SpecialPointer->index = 0;
      SpecialPointer->size = size;
      if(NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement)){
        EV_ev_flag_add(listener, &peer->event, EV_WRITE);
      }
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_SignedSpecialPointer_e:{
      void *ptr = Queue->SignedSpecialPointer.ptr;
      uintptr_t size = Queue->SignedSpecialPointer.size;
      NET_TCP_QueuerReference_t SignerQueuerReference = Queue->SignedSpecialPointer.SignerNode;
      uint64_t UniqueNumber = Queue->SignedSpecialPointer.UniqueNumber;
      NET_TCP_QueueElement_t QueueElement;
      QueueElement.t = _SignedSpecialPointer_e;
      QueueElement.tdata = (uint8_t *)ptr - sizeof(_SignedSpecialPointer_t);
      _SignedSpecialPointer_t *SignedSpecialPointer = (_SignedSpecialPointer_t *)QueueElement.tdata;
      SignedSpecialPointer->index = 0;
      SignedSpecialPointer->size = size;
      SignedSpecialPointer->SignerQueuerReference = SignerQueuerReference;
      SignedSpecialPointer->UniqueNumber = UniqueNumber;
      if(NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement)){
        EV_ev_flag_add(listener, &peer->event, EV_WRITE);
      }
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_PeerEvent_e:{
      PR_abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_File_e:{
      NET_TCP_QueueElement_t QueueElement;
      QueueElement.t = _ReadyFile_e;
      QueueElement.tdata = A_resize(0, sizeof(_ReadyFile_t));
      _ReadyFile_t *ReadyFile = (_ReadyFile_t *)QueueElement.tdata;
      ReadyFile->ran = 0;
      IO_fd_t infd = Queue->File.infd;
      IO_off_t isize = Queue->File.isize;
      IO_off_t nsize = Queue->File.nsize;
      IO_fd_t outfd;
      EV_event_get_fd(&peer->event, &outfd);
      EV_io_sendfile_init(
        &ReadyFile->ev,
        &infd,
        &outfd,
        isize,
        nsize,
        _NET_TCP_write_godfather_file_cb
      );
      ReadyFile->peer = peer;
      ReadyFile->cb = Queue->File.cb;
      ReadyFile->userdata = Queue->File.userdata;
      if(NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement)){
        EV_ev_flag_add(listener, &peer->event, EV_WRITE);
      }
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_CloseIfGodFather_e:{
      NET_TCP_QueueElement_t QueueElement;
      QueueElement.t = _CloseIfGodFather_e;
      if(NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement)){
        NET_TCP_CloseHard(peer);
        return NET_TCP_EXT_PeerIsClosed_e;
      }
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_CloseHard_e:{
      _NET_TCP_GodFatherWriteLast_ClearQueue(listener, tcp, peer, QueuerReference);
      return NET_TCP_EXT_PeerIsClosed_e;
    }
  }
}

#undef _SpecialPointer_e
#undef _SignedSpecialPointer_e
#undef _ReadyFile_e
#undef _CloseIfGodFather_e

#undef _SpecialPointer_t
#undef _SignedSpecialPointer_t
#undef _ReadyFile_t
