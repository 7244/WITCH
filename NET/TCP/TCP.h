#pragma once

#include _WITCH_PATH(MEM/MEM.h)
#include _WITCH_PATH(A/A.h)
#include _WITCH_PATH(EV/EV.h)
#include _WITCH_PATH(NET/NET.h)

#include _WITCH_PATH(NET/TCP/types.h)

bool NET_TCP_IsPeerClient(NET_TCP_peer_t *peer){
	return !!(peer->flag & NET_TCP_PClient_e);
}
bool NET_TCP_IsPeerServer(NET_TCP_peer_t *peer){
	return !(peer->flag & NET_TCP_PClient_e);
}

#include _WITCH_PATH(NET/TCP/internal/PeerConnection.h)
#include _WITCH_PATH(NET/TCP/internal/read/read.h)
#include _WITCH_PATH(NET/TCP/internal/write/write.h)


uint64_t NET_TCP_GetUniqueNumber(NET_TCP_t *tcp){
	return tcp->UniqueNumber++;
}

#include _WITCH_PATH(NET/TCP/internal/state/godfather.h)
/* read godfather included by state godfather */
/* write godfather included by state godfather */

NET_TCP_extid_t NET_TCP_EXT_new(NET_TCP_t *tcp, uintptr_t SockDataSize, uintptr_t PeerDataSize){
	tcp->ext.info = (NET_TCP_extinfo_t *)A_resize(tcp->ext.info, sizeof(NET_TCP_extinfo_t) * (tcp->ext.total + 1));
	tcp->ext.info[tcp->ext.total].SockDataOffset = tcp->SockDataSize;
	tcp->SockDataSize += SockDataSize;
	if(SockDataSize){
		tcp->SockData = A_resize(tcp->SockData, tcp->SockDataSize);
	}
	tcp->ext.info[tcp->ext.total].PeerDataOffset = tcp->PeerDataSize;
	tcp->PeerDataSize += PeerDataSize;
	return tcp->ext.total++;
}

NET_TCP_layerid_t NET_TCP_layer_state_open(
	NET_TCP_t *tcp,
	NET_TCP_extid_t extid,
	NET_TCP_cb_state_t func
){
	NET_TCP_op_t *op = &tcp->op[NET_TCP_opid_state_e];
	op->layer = A_resize(op->layer, sizeof(NET_TCP_layer_state_t) * (op->total + 1));
	NET_TCP_layer_state_t *layer_state = &((NET_TCP_layer_state_t *)op->layer)[op->total];
	layer_state->extid = extid;
	layer_state->func = func;
	return op->total++;
}
void NET_TCP_layer_state_close(NET_TCP_t *tcp, NET_TCP_layerid_t layerid){
	/* TODO */
	PR_abort();
}
NET_TCP_layerid_t NET_TCP_layer_read_open(
	NET_TCP_t *tcp,
	NET_TCP_extid_t extid,
	NET_TCP_cb_read_t func,
	A_resize_t resize_SpecialPointer,
	A_resize_t resize_SignedSpecialPointer,
	NET_TCP_CBReadSignConsumed CBReadSignConsumed
){
	NET_TCP_op_t *op = &tcp->op[NET_TCP_opid_read_e];
	op->layer = A_resize(op->layer, sizeof(NET_TCP_layer_read_t) * (op->total + 1));
	NET_TCP_layer_read_t *layer_read = &((NET_TCP_layer_read_t *)op->layer)[op->total];
	layer_read->extid = extid;
	layer_read->func = func;
	layer_read->resize_SpecialPointer = resize_SpecialPointer;
	layer_read->resize_SignedSpecialPointer = resize_SignedSpecialPointer;
	layer_read->CBReadSignConsumed = CBReadSignConsumed;
	return op->total++;
}
void NET_TCP_layer_read_close(NET_TCP_t *tcp, NET_TCP_layerid_t layerid){
	/* TODO */
	PR_abort();
}
NET_TCP_layerid_t NET_TCP_layer_write_open(
	NET_TCP_t *tcp,
	NET_TCP_extid_t extid,
	NET_TCP_cb_write_t func,
	A_resize_t resize_SpecialPointer,
	A_resize_t resize_SignedSpecialPointer,
	NET_TCP_CBWriteSignConsumed CBWriteSignConsumed
){
	NET_TCP_op_t *op = &tcp->op[NET_TCP_opid_write_e];
	op->layer = A_resize(op->layer, sizeof(NET_TCP_layer_write_t) * (op->total + 1));
	NET_TCP_layer_write_t *layer_write = &((NET_TCP_layer_write_t *)op->layer)[op->total];
	layer_write->extid = extid;
	layer_write->func = func;
	layer_write->resize_SpecialPointer = resize_SpecialPointer;
	layer_write->resize_SignedSpecialPointer = resize_SignedSpecialPointer;
	layer_write->CBWriteSignConsumed = CBWriteSignConsumed;
	return op->total++;
}
void NET_TCP_layer_write_close(NET_TCP_t *tcp, NET_TCP_layerid_t layerid){
	/* TODO */
	PR_abort();
}
NET_TCP_layerid_t NET_TCP_layer_connect_open(
	NET_TCP_t *tcp,
	NET_TCP_extid_t extid,
	NET_TCP_cb_connect_t func
){
	NET_TCP_op_t *op = &tcp->op[NET_TCP_opid_connect_e];
	op->layer = A_resize(op->layer, sizeof(NET_TCP_layer_connect_t) * (op->total + 1));
	NET_TCP_layer_connect_t *layer_connect = &((NET_TCP_layer_connect_t *)op->layer)[op->total];
	layer_connect->extid = extid;
	layer_connect->func = func;
	return op->total++;
}
void NET_TCP_layer_connect_close(NET_TCP_t *tcp, NET_TCP_layerid_t layerid){
	/* TODO */
	PR_abort();
}

uint8_t *NET_TCP_GetSockData(NET_TCP_t *tcp, NET_TCP_extid_t extid){
	return &tcp->SockData[tcp->ext.info[extid].SockDataOffset];
}
uint8_t *NET_TCP_GetPeerData(NET_TCP_peer_t *peer, NET_TCP_extid_t extid){
	return (uint8_t *)peer + sizeof(NET_TCP_peer_t) + peer->parent->ext.info[extid].PeerDataOffset;
}

NET_TCP_QueuerReference_t NET_TCP_StartReadLayer(NET_TCP_peer_t *peer, NET_TCP_layerid_t layerid){
	NET_TCP_QueuerReference_t QueuerReference = _NET_TCP_QueuerList_NewNode(&peer->ReadQueuerList);
	_NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
		&peer->ReadQueuerList,
		QueuerReference);
	NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
	Queuer->layerid = layerid;
	Queuer->QueueFirstNode = -1;
	_NET_TCP_QueuerList_linkPrev(
		&peer->ReadQueuerList,
		_NET_TCP_QueuerList_GetNodeLast(&peer->ReadQueuerList),
		QueuerReference);
	return QueuerReference;
}
void NET_TCP_StopReadLayer(NET_TCP_peer_t *peer, NET_TCP_QueuerReference_t QueuerReference){
	_NET_TCP_QueuerList_Unlink(&peer->ReadQueuerList, QueuerReference);
	_NET_TCP_QueuerList_Recycle(&peer->ReadQueuerList, QueuerReference);
}

NET_TCP_QueuerReference_t NET_TCP_StartWriteLayer(NET_TCP_peer_t *peer, NET_TCP_layerid_t layerid){
	NET_TCP_QueuerReference_t QueuerReference = _NET_TCP_QueuerList_NewNode(&peer->WriteQueuerList);
	_NET_TCP_QueuerList_Node_t *QueuerNode = _NET_TCP_QueuerList_GetNodeByReference(
		&peer->WriteQueuerList,
		QueuerReference);
	NET_TCP_Queuer_t *Queuer = &QueuerNode->data.data;
	Queuer->layerid = layerid;
	Queuer->QueueFirstNode = -1;
	_NET_TCP_QueuerList_linkPrev(
		&peer->WriteQueuerList,
		_NET_TCP_QueuerList_GetNodeLast(&peer->WriteQueuerList),
		QueuerReference);
	return QueuerReference;
}
void NET_TCP_StopWriteLayer(NET_TCP_peer_t *peer, NET_TCP_QueuerReference_t QueuerReference){
	_NET_TCP_QueuerList_Unlink(&peer->WriteQueuerList, QueuerReference);
	_NET_TCP_QueuerList_Recycle(&peer->WriteQueuerList, QueuerReference);
}

/*
	allocates tcp then returns pointer.
	user can configure tcp then call NET_TCP_open.
	source ip and port will be asigned random as default.
	@param EV_t *listener
*/
NET_TCP_t *NET_TCP_alloc(EV_t *listener){
	NET_TCP_t *tcp = (NET_TCP_t *)A_resize(0, sizeof(NET_TCP_t));

	tcp->listener = listener;

	tcp->PeerTimeoutTime = 60000000000;
	VAS_open(&tcp->PeerTimerList, sizeof(_NET_TCP_PeerTimer_t));

	tcp->ConnectTimeoutTime = 5000000000;
	VAS_open(&tcp->ConnectTimerList, sizeof(_NET_TCP_ConnectTimer_t));

	tcp->ssrcaddr.ip = INADDR_ANY;
	tcp->ssrcaddr.port = 0;

	tcp->ext.total = 0;
	tcp->ext.info = 0;

	tcp->SockData = 0;
	tcp->SockDataSize = 0;
	tcp->PeerDataSize = 0;

	MEM_set(0, tcp->op, sizeof(tcp->op));

	#ifndef WITCH_set_UseUninitialisedValues
		tcp->UniqueNumber = 0;
	#endif

	tcp->GodFatherextid = NET_TCP_EXT_new(tcp, 0, 0);
	NET_TCP_layer_state_open(tcp, tcp->GodFatherextid, _NET_TCP_GodFatherStateFirst);

	return tcp;
}
/*
	@param NET_TCP_t *tcp
*/
void NET_TCP_free(NET_TCP_t *tcp){
	A_resize(tcp, 0);
}

/*
	must be called after all settings are done in tcp
	must no any extension or layer function called after this
	@param NET_TCP_t *tcp
*/
void NET_TCP_open(NET_TCP_t *tcp){
	tcp->GodFatherReadFirst = NET_TCP_layer_read_open(
		tcp,
		tcp->GodFatherextid,
		_NET_TCP_GodFatherReadFirst,
		0,
		0,
		0
	);
	tcp->GodFatherReadLast = NET_TCP_layer_read_open(
		tcp,
		tcp->GodFatherextid,
		_NET_TCP_GodFatherReadLast,
		A_resize,
		0,
		0
	);
	tcp->GodFatherWriteFirst = NET_TCP_layer_write_open(
		tcp,
		tcp->GodFatherextid,
		_NET_TCP_GodFatherWriteFirst,
		0,
		0,
		0
	);
	tcp->GodFatherWriteLast = NET_TCP_layer_write_open(
		tcp,
		tcp->GodFatherextid,
		_NET_TCP_GodFatherWriteLast,
		_NET_TCP_write_godfather_resize_SpecialPointer,
		_NET_TCP_write_godfather_resize_SignedSpecialPointer,
		0
	);
}
/*
	must be called before free if its opened.
	it will allow user to change settings of socket then user can reopen it without free.
	@param NET_TCP_t *tcp
*/
void NET_TCP_close(NET_TCP_t *tcp, uint32_t flag){
	
}
