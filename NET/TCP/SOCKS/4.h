#pragma once

#include _WITCH_PATH(NET/TCP/TCP.h)

typedef struct{
	uint8_t ver;
	uint8_t cmd;
	uint16_t dstport;
	uint32_t dstip;
}NET_TCP_SOCKS4_REQ_t;

typedef struct{
	uint8_t vn;
	uint8_t rep;
	uint16_t dstport;
	uint32_t dstip;
}NET_TCP_SOCKS4_RES_t;

typedef struct NET_TCP_SOCKS4_sockdata_t NET_TCP_SOCKS4_sockdata_t;
typedef struct NET_TCP_SOCKS4_peerdata_t NET_TCP_SOCKS4_peerdata_t;

typedef uint8_t (*NET_TCP_SOCKS4_infofunc_t)(NET_TCP_peer_t *, NET_TCP_SOCKS4_sockdata_t *, NET_TCP_SOCKS4_peerdata_t *, uint8_t);

struct NET_TCP_SOCKS4_sockdata_t{
	NET_addr_t addr;
	NET_TCP_SOCKS4_infofunc_t infofunc;
};
struct NET_TCP_SOCKS4_peerdata_t{
	NET_addr_t addr;
};

typedef struct{
	uintptr_t EXTid;
	NET_TCP_SOCKS4_sockdata_t *sockdata;
}NET_TCP_SOCKS4_t;

enum{
	/* flag & 3 */
	NET_TCP_SOCKS4_connect_e = 1,
	NET_TCP_SOCKS4_invalid_res_e = 2,
	NET_TCP_SOCKS4_reply_e = 3,

	/* flag >> 2 */
	NET_TCP_SOCKS4_reply_reject_e = (1 << 2),
	NET_TCP_SOCKS4_reply_unknown_e = (2 << 2)
};
#define NET_TCP_SOCKS4_flag(flag_m, name_m) \
	(!!((flag_m) & CONCAT3(NET_TCP_SOCKS4_,name_m,_e)))

void _NET_TCP_SOCKS4_raise_layer(NET_TCP_peer_t *peer){
	peer->cb_index.istate++;
	peer->cb_index.iread++;
	NET_TCP_EXTsigstate(peer, NET_TCP_state_succ_e);
}

#define _NET_TCP_SOCKS4_call_cb(flag_m){ \
	if(sockdata->infofunc(peer, sockdata, peerdata, flag_m)) \
		return NET_TCP_EXT_abconn_e; \
	else{ \
		_NET_TCP_SOCKS4_raise_layer(peer); \
		return 1; \
	} \
}

uint8_t _NET_TCP_SOCKS4_state(NET_TCP_peer_t *peer, NET_TCP_SOCKS4_sockdata_t *sockdata, NET_TCP_SOCKS4_peerdata_t *peerdata, uint8_t flag){
	if(!(flag & NET_TCP_state_succ_e)){
		sockdata->infofunc(peer, sockdata, peerdata, NET_TCP_SOCKS4_connect_e);
		return NET_TCP_EXT_abconn_e;
	}
	NET_addr_t *addr = (NET_addr_t *)peerdata;
	uint8_t buf[sizeof(NET_TCP_SOCKS4_REQ_t) + 1];
	NET_TCP_SOCKS4_REQ_t *req = (NET_TCP_SOCKS4_REQ_t *)buf;
	req->ver = 0x04;
	req->cmd = 0x01;
	req->dstport = SwapEndian2(addr->port);
	req->dstip = SwapEndian4(addr->ip);
	buf[sizeof(NET_TCP_SOCKS4_REQ_t)] = 0;
	NET_TCP_write_ptr(peer, req, sizeof(*req) + 1);
	return NET_TCP_EXT_dontgo_e;
}

uint8_t _NET_TCP_SOCKS4_read(NET_TCP_peer_t *peer, NET_TCP_SOCKS4_sockdata_t *sockdata, NET_TCP_SOCKS4_peerdata_t *peerdata, uint8_t *data, uintptr_t size){
	if(size == sizeof(NET_TCP_SOCKS4_RES_t)){
		NET_TCP_SOCKS4_RES_t *res = (NET_TCP_SOCKS4_RES_t *)data;
		if(res->rep == 0x5a){
			_NET_TCP_SOCKS4_call_cb(0);
			uint8_t flag = _NET_TCP_SOCKS4_raise_layer(peer);
			if(flag & NET_TCP_EXT_abconn_e)
				return NET_TCP_EXT_abconn_e;
		}
		else if(res->rep == 0x5b) _NET_TCP_SOCKS4_call_cb(NET_TCP_SOCKS4_reply_e | NET_TCP_SOCKS4_reply_reject_e)
		else _NET_TCP_SOCKS4_call_cb(NET_TCP_SOCKS4_reply_e | NET_TCP_SOCKS4_reply_unknown_e)
	}
	else if(size == 0)
		sockdata->infofunc(peer, sockdata, peerdata, NET_TCP_SOCKS4_connect_e);
	else
		_NET_TCP_SOCKS4_call_cb(NET_TCP_SOCKS4_invalid_res_e)
	return NET_TCP_EXT_dontgo_e;
}

void _NET_TCP_SOCKS4_connect(NET_TCP_peer_t *peer, NET_TCP_SOCKS4_sockdata_t *sockdata, NET_TCP_SOCKS4_peerdata_t *peerdata, NET_addr_t **addr){
	peerdata->addr = **addr;
	**addr = sockdata->addr;
	*addr = &peerdata->addr;
}

uint8_t _NET_TCP_SOCKS4_cb_default(NET_TCP_peer_t *peer, NET_TCP_SOCKS4_sockdata_t *sockdata, NET_TCP_SOCKS4_peerdata_t *peerdata, uint8_t flag){
	return !!flag;
}

typedef struct{
	NET_addr_t addr;
	NET_TCP_SOCKS4_infofunc_t infofunc;
	uintptr_t sockdatasize;
	uintptr_t peerdatasize;
}NET_TCP_SOCKS_add_arg_t;

NET_TCP_SOCKS4_t NET_TCP_SOCKS4_add(NET_TCP_socket_t *tcpsock, NET_TCP_SOCKS_add_arg_t arg){
	NET_TCP_SOCKS4_t r;
	r.EXTid = NET_TCP_EXT_new(tcpsock, sizeof(NET_TCP_SOCKS4_sockdata_t) + arg.sockdatasize, sizeof(NET_TCP_SOCKS4_peerdata_t) + arg.peerdatasize);

	r.sockdata = (NET_TCP_SOCKS4_sockdata_t *)NET_TCP_EXT_get_sockdata(tcpsock, r.EXTid);
	r.sockdata->addr = arg.addr;
	r.sockdata->infofunc = arg.infofunc ? arg.infofunc : _NET_TCP_SOCKS4_cb_default;

	NET_TCP_EXTcbadd(tcpsock, NET_TCP_cb_state_e, r.EXTid, _NET_TCP_SOCKS4_state);
	NET_TCP_EXTcbadd(tcpsock, NET_TCP_cb_read_e, r.EXTid, _NET_TCP_SOCKS4_read);
	NET_TCP_EXTcbadd(tcpsock, NET_TCP_cb_connect_e, r.EXTid, _NET_TCP_SOCKS4_connect);

	return r;
}
#define NET_TCP_SOCKS4_add(tcpsock_m, arg_m) \
	NET_TCP_SOCKS4_add(tcpsock_m, WITCH_c(NET_TCP_SOCKS_add_arg_t)arg_m)
