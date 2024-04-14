#include _WITCH_PATH(NET/TCP/internal/read/godfather.h)
#include _WITCH_PATH(NET/TCP/internal/write/godfather.h)

NET_TCP_layerflag_t _NET_TCP_GodFatherStateFirst(
  NET_TCP_peer_t *peer,
  uint8_t *SockData,
  uint8_t *PeerData,
  uint32_t flag
){
  if(flag & NET_TCP_state_succ_e){
    EV_t *listener = peer->parent->listener;
    EV_event_start(listener, &peer->event);
    NET_TCP_StartPeerTimer(listener, peer->parent, peer, 0);
  }
  else do{
    if(!(flag & NET_TCP_state_init_e)){
      break;
    }
  }while(0);
  return 0;
}
