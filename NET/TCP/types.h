#pragma once

#ifndef NET_TCP_set_PadSocket
  #define NET_TCP_set_PadSocket 1
#endif
#ifndef NET_TCP_set_PadPeer
  #define NET_TCP_set_PadPeer 1
#endif
#ifndef NET_TCP_set_Storesdstaddr
  #define NET_TCP_set_Storesdstaddr 1
#endif
#ifndef NET_TCP_set_type_extid
  #define NET_TCP_set_type_extid uint8_t
#endif
#ifndef NET_TCP_set_type_layerid
  #define NET_TCP_set_type_layerid uint8_t
#endif

/* extension id */
typedef NET_TCP_set_type_extid NET_TCP_extid_t;

/* layer id */
typedef NET_TCP_set_type_layerid NET_TCP_layerid_t;

/* layer flag */
/* NET_TCP_EXT_*_e */
typedef uint32_t NET_TCP_layerflag_t;

/* operation id */
typedef uint8_t NET_TCP_opid_t;

#include _WITCH_PATH(VEC/VEC.h)
#include _WITCH_PATH(VAS/VAS.h)

typedef struct{
  NET_TCP_layerid_t layerid;
  VAS_node_t QueueFirstNode;
  VAS_node_t QueueLastNode;
}NET_TCP_Queuer_t;
#define BLL_set_prefix _NET_TCP_QueuerList
#define BLL_set_NodeData NET_TCP_Queuer_t data;
#define BLL_set_IsNodeRecycled 1
#define BLL_set_Language 0
#include <BLL/BLL.h>
typedef _NET_TCP_QueuerList_NodeReference_t NET_TCP_QueuerReference_t;

enum{
  NET_TCP_opid_state_e,
  NET_TCP_opid_read_e,
  NET_TCP_opid_write_e,
  NET_TCP_opid_connect_e,
  NET_TCP_opid_e
};

/* NET_TCP_peer_t's flag */
enum{
  NET_TCP_PClient_e = 1
};

typedef struct NET_TCP_peer_t NET_TCP_peer_t;

typedef struct{
  uintptr_t SockDataOffset;
  uintptr_t PeerDataOffset;
}NET_TCP_extinfo_t;
typedef struct{
  NET_TCP_extid_t total;
  NET_TCP_extinfo_t *info;
}NET_TCP_ext_t;

typedef struct NET_TCP_t NET_TCP_t;

typedef void (*NET_TCP_SpecialPointer_cb)(NET_TCP_t *, NET_TCP_peer_t *, void *, uintptr_t);

#include _WITCH_PATH(NET/TCP/internal/Queue.h)

typedef NET_TCP_layerflag_t (*NET_TCP_cb_state_t)(
  NET_TCP_peer_t *,
  uint8_t *,
  uint8_t *,
  uint32_t
);
typedef NET_TCP_layerflag_t (*NET_TCP_cb_read_t)(
  NET_TCP_peer_t *,
  uint8_t *,
  uint8_t *,
  NET_TCP_QueuerReference_t,
  uint32_t *,
  NET_TCP_Queue_t *
);
typedef NET_TCP_layerflag_t (*NET_TCP_cb_write_t)(
  NET_TCP_peer_t *,
  uint8_t *,
  uint8_t *,
  NET_TCP_QueuerReference_t,
  uint32_t *,
  NET_TCP_Queue_t *
);
typedef NET_TCP_layerflag_t (*NET_TCP_cb_connect_t)(
  NET_TCP_peer_t *,
  uint8_t *,
  uint8_t *,
  NET_addr_t **
);

typedef void (*NET_TCP_CBReadSignConsumed)(NET_TCP_peer_t *, NET_TCP_Queuer_t *, NET_TCP_layerid_t, uint64_t);
typedef void (*NET_TCP_CBWriteSignConsumed)(NET_TCP_peer_t *, NET_TCP_Queuer_t *, NET_TCP_layerid_t, uint64_t);

typedef struct{
  NET_TCP_extid_t extid;
  NET_TCP_cb_state_t func;
}NET_TCP_layer_state_t;
typedef struct{
  NET_TCP_extid_t extid;
  NET_TCP_cb_read_t func;
  A_resize_t resize_SpecialPointer;
  A_resize_t resize_SignedSpecialPointer;
  NET_TCP_CBReadSignConsumed CBReadSignConsumed;
}NET_TCP_layer_read_t;
typedef struct{
  NET_TCP_extid_t extid;
  NET_TCP_cb_write_t func;
  A_resize_t resize_SpecialPointer;
  A_resize_t resize_SignedSpecialPointer;
  NET_TCP_CBWriteSignConsumed CBWriteSignConsumed;
}NET_TCP_layer_write_t;
typedef struct{
  NET_TCP_extid_t extid;
  NET_TCP_cb_connect_t func;
}NET_TCP_layer_connect_t;

typedef struct{
  NET_TCP_layerid_t total;

  /* NET_TCP_layer_*_t */
  uint8_t *layer;
}NET_TCP_op_t;

typedef struct{
  uint64_t NextTime;
  NET_TCP_peer_t *peer;
  /* this is useful since extensions doesnt know who started timer */
  /* so extension layer simply check extid with itself's extid to see is it started by self or not */
  /* and VAS have a bit node overhead so we dont lose much ram space */
  NET_TCP_extid_t extid;
}_NET_TCP_PeerTimer_t;

typedef struct{
  uint64_t NextTime;
  NET_TCP_peer_t *peer;
}_NET_TCP_ConnectTimer_t;

#if !NET_TCP_set_PadSocket
  #pragma pack(push, 1)
#endif
struct NET_TCP_t{
  EV_event_t ev; /* .sock */

  EV_t *listener;

  /* default is 60000000000 */
  uint64_t PeerTimeoutTime;
  /* _NET_TCP_PeerTimer_t */
  VAS_t PeerTimerList;
  /* aims first timer in PeerTimerList  */
  EV_timer_t EVPeerTimer;

  /* default is 5000000000 */
  uint64_t ConnectTimeoutTime;
  /* _NET_TCP_ConnectTimer_t */
  VAS_t ConnectTimerList;
  /* aims first timer in ConnectTimerList */
  EV_timer_t EVConnectTimer;

  NET_TCP_ext_t ext;
  NET_TCP_op_t op[NET_TCP_opid_e];

  NET_TCP_extid_t GodFatherextid;

  NET_TCP_layerid_t GodFatherReadFirst;
  NET_TCP_layerid_t GodFatherReadLast;
  NET_TCP_layerid_t GodFatherWriteFirst;
  NET_TCP_layerid_t GodFatherWriteLast;

  /* this will be used for generating unique numbers to peer needs. */
  /* we dont store this in NET_TCP_peer_t because it would eat 8 byte per peer. */
  /* 2^64 is pretty big anyway so its very unlikely to have overflowed UniqueNumber. */
  uint64_t UniqueNumber;

  uint8_t *SockData;
  uintptr_t SockDataSize;
  uintptr_t PeerDataSize;

  NET_socket_t sock;
  NET_addr_t ssrcaddr;
};
#if !NET_TCP_set_PadSocket
  #pragma pack(pop)
#endif

#if !NET_TCP_set_PadPeer
  #pragma pack(push, 1)
#endif
struct NET_TCP_peer_t{
  EV_event_t event;
  VAS_node_t TimerNode;

  NET_TCP_t *parent;

  _NET_TCP_QueuerList_t ReadQueuerList;
  _NET_TCP_QueuerList_t WriteQueuerList;

  /* NET_TCP_QueueElement_t */
  VAS_t queue;

  #if NET_TCP_set_Storesdstaddr
    NET_addr_t sdstaddr;
  #endif

  NET_TCP_layerid_t StateOffset;

  uint8_t flag;
};
#if !NET_TCP_set_PadPeer
  #pragma pack(pop)
#endif

typedef struct{
  uint8_t t;
  uint8_t *tdata;
}NET_TCP_QueueElement_t;

enum{
  NET_TCP_state_succ_e = 0x01,
  NET_TCP_state_init_e = 0x02
};

enum{
  NET_TCP_EXT_pass_e = 0x00,
  NET_TCP_EXT_dontgo_e = 0x01,
  NET_TCP_EXT_PeerIsClosed_e = 0x02
};

/* close peer hardly. */
/* write layer must close itself then return 0 */
void NET_TCP_CloseHard(NET_TCP_peer_t *);
