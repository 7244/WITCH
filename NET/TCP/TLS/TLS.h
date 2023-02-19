#pragma once

#include _WITCH_PATH(NET/TCP/TCP.h)
#include _WITCH_PATH(ETC/TLS.h)

#ifndef NET_TCP_TLS_set_FileBufferSize
  #define NET_TCP_TLS_set_FileBufferSize 0x4000
#endif
#ifndef NET_TCP_TLS_set_SignLimit
  #define NET_TCP_TLS_set_SignLimit 2
#endif

typedef struct{
  TLS_ctx_t *ctx;
  NET_TCP_layerid_t ReadLayerID;
  NET_TCP_layerid_t WriteLayerID;
}_NET_TCP_TLS_sockdata_t;
typedef struct{
  _NET_TCP_TLS_sockdata_t *sockdata;
  NET_TCP_QueuerReference_t QueuerReference;
  TLS_peer_t tlspeer;
  NET_TCP_peer_t *peer;
}_NET_TCP_TLS_peerdata_t;

enum{
  #define _SpecialPointer_e _NET_TCP_TLS_SpecialPointer_e
  _SpecialPointer_e,
  #define _NotReadyFile_e _NET_TCP_TLS_NotReadyFile_e
  _NotReadyFile_e,
  #define _ReadyFile_e _NET_TCP_TLS_ReadyFile_e
  _ReadyFile_e,
  #define _CloseIfGodFather_e _NET_TCP_TLS_CloseIfGodFather_e
  _CloseIfGodFather_e
};

#define _SpecialPointer_t _NET_TCP_TLS_SpecialPointer_t
typedef struct{
  uintptr_t size;
}_SpecialPointer_t;
#define _NotReadyFile_t _NET_TCP_TLS_NotReadyFile_t
typedef struct{
  IO_fd_t infd;
  IO_off_t isize;
  IO_off_t nsize;
  NET_TCP_Queue_File_cb_t cb;
  void *userdata;
}_NotReadyFile_t;
#define _ReadyFile_t _NET_TCP_TLS_ReadyFile_t
typedef struct{
  EV_io_pread_t io;
  IO_off_t nsize;
  uint64_t UniqueNumber;
  _NET_TCP_TLS_peerdata_t *peerdata;
  uint8_t HowManySigned;
  NET_TCP_Queue_File_cb_t cb;
  void *userdata;
}_ReadyFile_t;

void _NET_TCP_TLS_tlscb(TLS_peer_t *tlspeer, uint32_t state, bool val){
  _NET_TCP_TLS_peerdata_t *peerdata = OFFSETLESS(tlspeer, _NET_TCP_TLS_peerdata_t, tlspeer);
  NET_TCP_peer_t *peer = peerdata->peer;
  if(state == TLS_state_handshake_e){
    if(!val){
      NET_TCP_StateLoop(peer);
    }
    else{
      NET_TCP_CloseHard(peer);
    }
  }
}

void _NET_TCP_TLS_WriteIfoutread(
  NET_TCP_t *tcp,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference,
  TLS_peer_t *tlspeer
){
  IO_ssize_t size = TLS_outreadsize(tlspeer);
  if(size < 1){
    /* peer can send not enough data to tls lib process so */
    return;
  }
  NET_TCP_QueuerReference_t NextQueuerReference = NET_TCP_IterateWriteQueuerReference(peer, QueuerReference);
  A_resize_t resize = NET_TCP_write_GetResize_SpecialPointer(tcp, peer, NextQueuerReference);
  uint8_t *ptr = resize(0, size);
  if(TLS_outread(tlspeer, ptr, size) != size){
    PR_abort();
  }
  NET_TCP_Queue_t wQueue;
  wQueue.SpecialPointer.ptr = ptr;
  wQueue.SpecialPointer.size = size;
  if(NET_TCP_write_loop(peer, NextQueuerReference, NET_TCP_QueueType_SpecialPointer_e, &wQueue) != 0){
    /* TODO enjoy dealing with it */
    PR_abort();
  }
}
uint32_t _NET_TCP_TLS_ChangeIfoutread(
  NET_TCP_t *tcp,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference,
  TLS_peer_t *tlspeer,
  uint32_t *type,
  NET_TCP_Queue_t *Queue
){
  IO_ssize_t size = TLS_outreadsize(tlspeer);
  if(size < 1){
    /* peer can send not enough data to tls lib process so */
    return NET_TCP_EXT_dontgo_e;
  }
  NET_TCP_QueuerReference_t NextQueuerReference = NET_TCP_IterateWriteQueuerReference(peer, QueuerReference);
  A_resize_t resize = NET_TCP_write_GetResize_SpecialPointer(tcp, peer, NextQueuerReference);
  uint8_t *ptr = resize(0, size);
  if(TLS_outread(tlspeer, ptr, size) != size){
    PR_abort();
  }
  *type = NET_TCP_QueueType_SpecialPointer_e;
  Queue->SpecialPointer.ptr = ptr;
  Queue->SpecialPointer.size = size;
  return 0;
}
void _NET_TCP_TLS_WriteSignedSpecialPointer(
  NET_TCP_t *tcp,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference,
  TLS_peer_t *tlspeer,
  uint64_t UniqueNumber
){
  IO_ssize_t size = TLS_outreadsize(tlspeer);
  if(size < 1){
    /* peer can send not enough data to tls lib process so */
    return;
  }
  NET_TCP_QueuerReference_t NextQueuerReference = NET_TCP_IterateWriteQueuerReference(peer, QueuerReference);
  A_resize_t resize = NET_TCP_write_GetResize_SignedSpecialPointer(peer->parent, peer, NextQueuerReference);
  uint8_t *ptr = resize(0, size);
  if(TLS_outread(tlspeer, ptr, size) != size){
    PR_abort();
  }
  NET_TCP_Queue_t wQueue;
  wQueue.SignedSpecialPointer.ptr = ptr;
  wQueue.SignedSpecialPointer.size = size;
  wQueue.SignedSpecialPointer.SignerNode = QueuerReference;
  wQueue.SignedSpecialPointer.UniqueNumber = UniqueNumber;
  if(NET_TCP_write_loop(peer, NextQueuerReference, NET_TCP_QueueType_SignedSpecialPointer_e, &wQueue) != 0){
    /* TODO enjoy */
    PR_abort();
  }
}

uint32_t _NET_TCP_TLS_state(
  NET_TCP_peer_t *peer,
  _NET_TCP_TLS_sockdata_t *sockdata,
  _NET_TCP_TLS_peerdata_t *peerdata,
  uint8_t flag
){
  if(flag & NET_TCP_state_succ_e){
    NET_TCP_StartReadLayer(peer, sockdata->ReadLayerID);
    peerdata->QueuerReference = NET_TCP_StartWriteLayer(peer, sockdata->WriteLayerID);
    uint32_t TLSflag = 0;
    if(NET_TCP_IsPeerServer(peer)){
      TLSflag |= TLS_peer_server_e;
    }
    if(TLS_peer_open(&peerdata->tlspeer, sockdata->ctx, TLSflag)){
      NET_TCP_CloseHard(peer);
      return NET_TCP_EXT_PeerIsClosed_e;
    }
    TLS_peer_set_cb(&peerdata->tlspeer, _NET_TCP_TLS_tlscb);
    peerdata->sockdata = sockdata;
    peerdata->peer = peer;
    if(TLSflag & TLS_peer_server_e); else{
      _NET_TCP_TLS_WriteIfoutread(peer->parent, peer, peerdata->QueuerReference, &peerdata->tlspeer);
    }
    return NET_TCP_EXT_dontgo_e;
  }
  else do{
    if(!(flag & NET_TCP_state_init_e)){
      break;
    }
    TLS_peer_close(&peerdata->tlspeer);
  }while(0);
  return 0;
}

uint32_t _NET_TCP_TLS_read_DynamicPointerLike(
  NET_TCP_peer_t *peer,
  _NET_TCP_TLS_sockdata_t *sockdata,
  _NET_TCP_TLS_peerdata_t *peerdata,
  NET_TCP_QueuerReference_t QueuerReference,
  void *Data,
  uintptr_t Size
){
  NET_TCP_t *tcp = peer->parent;
  IO_ssize_t r = TLS_inwrite(&peerdata->tlspeer, Data, Size);
  if(r < 0){
    NET_TCP_CloseHard(peer);
    return NET_TCP_EXT_dontgo_e;
  }
  if(r != Size){
    PR_abort();
  }
  inreadsize_gt:;
  uint8_t Buffer[0x1000];
  IO_ssize_t AmountOfRead = TLS_inread(&peerdata->tlspeer, Buffer, sizeof(Buffer));
  if(AmountOfRead == 0){
    _NET_TCP_TLS_WriteIfoutread(tcp, peer, peerdata->QueuerReference, &peerdata->tlspeer);
    return NET_TCP_EXT_dontgo_e;
  }
  else if(AmountOfRead < 0){
    NET_TCP_CloseHard(peer);
    return NET_TCP_EXT_PeerIsClosed_e;
  }
  NET_TCP_QueuerReference_t NextQueuerReference = NET_TCP_IterateReadQueuerReference(
    peer,
    QueuerReference);
  NET_TCP_Queue_t rQueue;
  rQueue.DynamicPointer.ptr = Buffer;
  rQueue.DynamicPointer.size = AmountOfRead;
  if(NET_TCP_read_loop(peer, NextQueuerReference, NET_TCP_QueueType_DynamicPointer_e, &rQueue) != 0){
    return NET_TCP_EXT_PeerIsClosed_e;
  }
  goto inreadsize_gt;
}

uint32_t _NET_TCP_TLS_read(
  NET_TCP_peer_t *peer,
  _NET_TCP_TLS_sockdata_t *sockdata,
  _NET_TCP_TLS_peerdata_t *peerdata,
  NET_TCP_QueuerReference_t QueuerReference,
  uint32_t *type,
  NET_TCP_Queue_t *Queue
){
  switch(*type){
    case NET_TCP_QueueType_DynamicPointer_e:{
      return _NET_TCP_TLS_read_DynamicPointerLike(
        peer,
        sockdata,
        peerdata,
        QueuerReference,
        Queue->DynamicPointer.ptr,
        Queue->DynamicPointer.size
      );
    }
    case NET_TCP_QueueType_SpecialPointer_e:{
      PR_abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_SignedSpecialPointer_e:{
      PR_abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_PeerEvent_e:{
      uint8_t Data[0x2000];
      IO_fd_t peer_fd;
      EV_event_get_fd(&peer->event, &peer_fd);
      IO_ssize_t Size = IO_read(&peer_fd, Data, sizeof(Data));
      if(Size < 0){
        NET_TCP_CloseHard(peer);
        return NET_TCP_EXT_PeerIsClosed_e;
      }
      return _NET_TCP_TLS_read_DynamicPointerLike(
        peer,
        sockdata,
        peerdata,
        QueuerReference,
        Data,
        Size
      );
    }
    case NET_TCP_QueueType_File_e:{
      PR_abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_CloseIfGodFather_e:{
      return NET_TCP_EXT_pass_e;
    }
    case NET_TCP_QueueType_CloseHard_e:{
      return 0;
    }
  }
}

void _NET_TCP_TLS_ReadyFile_open(
  EV_t *listener,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference,
  _NET_TCP_TLS_peerdata_t *peerdata,
  IO_fd_t infd,
  IO_off_t isize,
  IO_off_t nsize,
  NET_TCP_Queue_File_cb_t cb,
  void *userdata
);

void _NET_TCP_TLS_ReadyFile_Finish(_ReadyFile_t *ReadyFile, sint32_t err){
  _NET_TCP_TLS_peerdata_t *peerdata = ReadyFile->peerdata;
  NET_TCP_peer_t *peer = peerdata->peer;
  NET_TCP_QueuerReference_t QueuerReference = peerdata->QueuerReference;
  ReadyFile->cb(peer, ReadyFile->io.fd, err, ReadyFile->userdata);
  A_resize(ReadyFile, 0);
  NET_TCP_EXT_write_del(peer, QueuerReference);
  NET_TCP_QueueElement_t *QueueElement;
  while(!NET_TCP_EXT_write_get(peer, QueuerReference, &QueueElement)){
    switch(QueueElement->t){
      case _SpecialPointer_e:{
        uint8_t *ptr = QueueElement->tdata + sizeof(_SpecialPointer_t);
        _SpecialPointer_t *SpecialPointer = (_SpecialPointer_t *)QueueElement->tdata;
        IO_ssize_t r = TLS_outwrite(&peerdata->tlspeer, ptr, SpecialPointer->size);
        if(r < 0){
          PR_abort();
        }
        _NET_TCP_TLS_WriteIfoutread(peer->parent, peer, QueuerReference, &peerdata->tlspeer);
        A_resize(SpecialPointer, 0);
        NET_TCP_EXT_write_del(peer, QueuerReference);
        break;
      }
      case _NotReadyFile_e:{
        _NotReadyFile_t *NotReadyFile = (_NotReadyFile_t *)QueueElement->tdata;
        IO_fd_t infd = NotReadyFile->infd;
        IO_off_t isize = NotReadyFile->isize;
        IO_off_t nsize = NotReadyFile->nsize;
        NET_TCP_Queue_File_cb_t cb = NotReadyFile->cb;
        void *userdata = NotReadyFile->userdata;
        A_resize(NotReadyFile, 0);
        NET_TCP_EXT_write_del(peer, QueuerReference);
        _NET_TCP_TLS_ReadyFile_open(
          peer->parent->listener,
          peer,
          QueuerReference,
          peerdata,
          infd,
          isize,
          nsize,
          cb,
          userdata
        );
        return;
      }
      case _CloseIfGodFather_e:{
        NET_TCP_EXT_write_del(peer, QueuerReference);
        NET_TCP_QueuerReference_t NextQueuerReference = NET_TCP_IterateWriteQueuerReference(
          peer,
          QueuerReference);
        NET_TCP_Queue_t wQueue;
        if(NET_TCP_write_loop(peer, NextQueuerReference, NET_TCP_QueueType_CloseIfGodFather_e, &wQueue) != 0){
          /* TODO enjoy */
          PR_abort();
        }
        return;
      }
    }
  }
}

void _NET_TCP_TLS_ReadyFile_pread_cb(EV_t *listener, EV_io_pread_t *io, IO_ssize_t result){
  _ReadyFile_t *ReadyFile = OFFSETLESS(io, _ReadyFile_t, io);
  if(result < 0){
    if(result == -ECANCELED){
      /* peer is gone */
      ReadyFile->cb(0, ReadyFile->io.fd, -ECANCELED, ReadyFile->userdata);
      A_resize(ReadyFile, 0);
    }
    else{
      _NET_TCP_TLS_ReadyFile_Finish(ReadyFile, result);
    }
    return;
  }

  _NET_TCP_TLS_peerdata_t *peerdata = ReadyFile->peerdata;
  TLS_peer_t *tlspeer = &peerdata->tlspeer;
  IO_ssize_t r = TLS_outwrite(tlspeer, (uint8_t *)ReadyFile + sizeof(_ReadyFile_t), result);
  NET_TCP_peer_t *peer = peerdata->peer;
  if(r < 0){
    NET_TCP_CloseHard(peer);
    return;
  }
  _NET_TCP_TLS_WriteSignedSpecialPointer(
    peer->parent,
    peer,
    peerdata->QueuerReference,
    tlspeer,
    ReadyFile->UniqueNumber);

  ReadyFile->nsize -= result;
  if(!ReadyFile->nsize){
    _NET_TCP_TLS_ReadyFile_Finish(ReadyFile, 0);
    return;
  }
  io->isize += result;

  ReadyFile->HowManySigned--;
  /* not enough sign left */
  if(!ReadyFile->HowManySigned){
    io->tp.pool_node = -1;
    return;
  }

  IO_size_t ReadSize;
  if(ReadyFile->nsize < NET_TCP_TLS_set_FileBufferSize){
    ReadSize = ReadyFile->nsize;
  }
  else{
    ReadSize = NET_TCP_TLS_set_FileBufferSize;
  }
  io->nsize = ReadSize;
  EV_io_pread_start(listener, io);
}

void _NET_TCP_TLS_ReadyFile_Iterate(_ReadyFile_t *ReadyFile){
  IO_size_t ReadSize;
  if(ReadyFile->nsize < NET_TCP_TLS_set_FileBufferSize){
    ReadSize = ReadyFile->nsize;
  }
  else{
    ReadSize = NET_TCP_TLS_set_FileBufferSize;
  }
  ReadyFile->io.nsize = ReadSize;
  EV_io_pread_start(ReadyFile->peerdata->peer->parent->listener, &ReadyFile->io);
}

void _NET_TCP_TLS_CBWriteSignConsumed(
  NET_TCP_peer_t *peer,
  NET_TCP_Queuer_t *Queuer,
  NET_TCP_layerid_t layerid,
  uint64_t UniqueNumber
){
  NET_TCP_QueueElement_t *QueueElement;
  if(!NET_TCP_EXT_write_is_active2(peer, Queuer)){
    return;
  }
  NET_TCP_EXT_write_get2(peer, Queuer, &QueueElement);
  if(QueueElement->t != _ReadyFile_e){
    return;
  }
  _ReadyFile_t *ReadyFile = (_ReadyFile_t *)QueueElement->tdata;
  if(ReadyFile->UniqueNumber != UniqueNumber){
    return;
  }
  ReadyFile->HowManySigned++;
  if(ReadyFile->io.tp.pool_node != -1){
    /* its already */
    return;
  }
  _NET_TCP_TLS_ReadyFile_Iterate(ReadyFile);
}

void _NET_TCP_TLS_ReadyFile_open(
  EV_t *listener,
  NET_TCP_peer_t *peer,
  NET_TCP_QueuerReference_t QueuerReference,
  _NET_TCP_TLS_peerdata_t *peerdata,
  IO_fd_t infd,
  IO_off_t isize,
  IO_off_t nsize,
  NET_TCP_Queue_File_cb_t cb,
  void *userdata
){
  IO_size_t FileBufferSize;
  if(nsize < NET_TCP_TLS_set_FileBufferSize){
    FileBufferSize = nsize;
  }
  else{
    FileBufferSize = NET_TCP_TLS_set_FileBufferSize;
  }
  NET_TCP_QueueElement_t QueueElement;
  QueueElement.t = _ReadyFile_e;
  QueueElement.tdata = A_resize(0, sizeof(_ReadyFile_t) + FileBufferSize);
  NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement);
  _ReadyFile_t *ReadyFile = (_ReadyFile_t *)QueueElement.tdata;
  ReadyFile->nsize = nsize;
  ReadyFile->UniqueNumber = NET_TCP_GetUniqueNumber(peer->parent);
  ReadyFile->peerdata = peerdata;
  ReadyFile->HowManySigned = NET_TCP_TLS_set_SignLimit;
  ReadyFile->cb = cb;
  ReadyFile->userdata = userdata;
  EV_io_pread_init(
    &ReadyFile->io,
    infd,
    QueueElement.tdata + sizeof(_ReadyFile_t),
    isize,
    FileBufferSize,
    _NET_TCP_TLS_ReadyFile_pread_cb
  );
  EV_io_pread_start(listener, &ReadyFile->io);
}

uint32_t _NET_TCP_TLS_write(
  NET_TCP_peer_t *peer,
  _NET_TCP_TLS_sockdata_t *sockdata,
  _NET_TCP_TLS_peerdata_t *peerdata,
  NET_TCP_QueuerReference_t QueuerReference,
  uint32_t *type,
  NET_TCP_Queue_t *Queue
){
  switch(*type){
    case NET_TCP_QueueType_DynamicPointer_e:{
      uint8_t *ptr = (uint8_t *)Queue->DynamicPointer.ptr;
      uintptr_t size = Queue->DynamicPointer.size;
      if(!NET_TCP_EXT_write_is_active(peer, QueuerReference)){
        NET_TCP_QueueElement_t QueueElement;
        QueueElement.t = _SpecialPointer_e;
        QueueElement.tdata = A_resize(0, sizeof(_SpecialPointer_t) + size);
        _SpecialPointer_t *SpecialPointer = (_SpecialPointer_t *)QueueElement.tdata;
        SpecialPointer->size = size;
        MEM_copy(ptr, QueueElement.tdata, size);
        NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement);
        return NET_TCP_EXT_dontgo_e;
      }
      else{
        TLS_peer_t *tlspeer = &peerdata->tlspeer;
        IO_ssize_t r = TLS_outwrite(tlspeer, ptr, size);
        if(r < 0){
          NET_TCP_CloseHard(peer);
          return NET_TCP_EXT_dontgo_e;
        }
        return _NET_TCP_TLS_ChangeIfoutread(peer->parent, peer, QueuerReference, tlspeer, type, Queue);
      }
    }
    case NET_TCP_QueueType_SpecialPointer_e:{
      PR_abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_SignedSpecialPointer_e:{
      PR_abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_PeerEvent_e:{
      PR_abort();
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_File_e:{
      if(!NET_TCP_EXT_write_is_active(peer, QueuerReference)){
        NET_TCP_QueueElement_t QueueElement;
        QueueElement.t = _NotReadyFile_e;
        QueueElement.tdata = A_resize(0, sizeof(_NotReadyFile_t));
        _NotReadyFile_t *NotReadyFile = (_NotReadyFile_t *)QueueElement.tdata;
        NotReadyFile->infd = Queue->File.infd;
        NotReadyFile->isize = Queue->File.isize;
        NotReadyFile->nsize = Queue->File.nsize;
        NotReadyFile->cb = Queue->File.cb;
        NotReadyFile->userdata = Queue->File.userdata;
        NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement);
      }
      else{
        IO_fd_t infd = Queue->File.infd;
        IO_off_t isize = Queue->File.isize;
        IO_off_t nsize = Queue->File.nsize;
        NET_TCP_Queue_File_cb_t cb = Queue->File.cb;
        void *userdata = Queue->File.userdata;
        _NET_TCP_TLS_ReadyFile_open(
          peer->parent->listener,
          peer,
          QueuerReference,
          peerdata,
          infd,
          isize,
          nsize,
          cb,
          userdata
        );
      }
      return NET_TCP_EXT_dontgo_e;
    }
    case NET_TCP_QueueType_CloseIfGodFather_e:{
      if(!NET_TCP_EXT_write_is_active(peer, QueuerReference)){
        NET_TCP_QueueElement_t QueueElement;
        QueueElement.t = _CloseIfGodFather_e;
        NET_TCP_EXT_write_add(peer, QueuerReference, &QueueElement);
        return NET_TCP_EXT_dontgo_e;
      }
      else{
        return 0;
      }
    }
    case NET_TCP_QueueType_CloseHard_e:{
      while(!NET_TCP_EXT_write_is_active(peer, QueuerReference)){
        NET_TCP_QueueElement_t *QueueElement;
        NET_TCP_EXT_write_get(peer, QueuerReference, &QueueElement);
        switch(QueueElement->t){
          case _SpecialPointer_e:{
            A_resize(QueueElement->tdata, 0);
            break;
          }
          case _NotReadyFile_e:{
            _NotReadyFile_t *NotReadyFile = (_NotReadyFile_t *)QueueElement->tdata;
            NotReadyFile->cb(peer, NotReadyFile->infd, -ECANCELED, NotReadyFile->userdata);
            A_resize(QueueElement->tdata, 0);
            break;
          }
          case _ReadyFile_e:{
            _ReadyFile_t *ReadyFile = (_ReadyFile_t *)QueueElement->tdata;
            if(ReadyFile->io.tp.pool_node != -1){
              EV_io_pread_stop(peer->parent->listener, &ReadyFile->io);
            }
            break;
          }
          case _CloseIfGodFather_e:{
            break;
          }
        }
        NET_TCP_EXT_write_del(peer, QueuerReference);
      }
      return 0;
    }
  }
}

void NET_TCP_TLS_add(NET_TCP_t *tcp, TLS_ctx_t *ctx){
  NET_TCP_extid_t extid = NET_TCP_EXT_new(tcp, sizeof(_NET_TCP_TLS_sockdata_t), sizeof(_NET_TCP_TLS_peerdata_t));
  _NET_TCP_TLS_sockdata_t *sockdata = (_NET_TCP_TLS_sockdata_t *)NET_TCP_GetSockData(tcp, extid);

  NET_TCP_layer_state_open(tcp, extid, (NET_TCP_cb_state_t)_NET_TCP_TLS_state);
  sockdata->ReadLayerID = NET_TCP_layer_read_open(
    tcp,
    extid,
    (NET_TCP_cb_read_t)_NET_TCP_TLS_read,
    0,
    0,
    0
  );
  sockdata->WriteLayerID = NET_TCP_layer_write_open(
    tcp,
    extid,
    (NET_TCP_cb_write_t)_NET_TCP_TLS_write,
    0,
    0,
    _NET_TCP_TLS_CBWriteSignConsumed
  );

  sockdata->ctx = ctx;
}

#undef _SpecialPointer_e
#undef _NotReadyFile_e
#undef _ReadyFile_e
#undef _CloseIfGodFather_e

#undef _SpecialPointer_t
#undef _NotReadyFile_t
#undef _ReadyFile_t
