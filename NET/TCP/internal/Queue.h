enum{
	/*
		dynamic ptr. will be changed.
		layer must copy it to somewhere. or send it to next layer.
	*/
	NET_TCP_QueueType_DynamicPointer_e,

	/*
		special ptr. special ptr that allocated by next layer alloc function.
		this is very resource friendly because
		next layer able to add extra space to begin and end
		which can be used to add bookkeeping without new allocation.
	*/
	NET_TCP_QueueType_SpecialPointer_e,

	/*
		same with specialptr but it has QueuerReference.
		when its consumed signer must be notified.
	*/
	NET_TCP_QueueType_SignedSpecialPointer_e,

	/*
		its only for read layer.
		if read layer receives this request it needs to deal with peer->ev by itself.
		i cant think any other FAST way to implement read speed limiter in extension side
		if we give socket to extension it needs to allocate own EV_event_t
		and imagine that first extensions always deconstructs itself very fast (like socks4)
		that would cause alot allocate for per peer.
	*/
	NET_TCP_QueueType_PeerEvent_e,

	/*
		file description. cb needs to be called after file transfer is done.
	*/
	NET_TCP_QueueType_File_e,

	/*
		CloseHard will be called if it goes to GodFather
		used for close peer after data is sent etc
	*/
	NET_TCP_QueueType_CloseIfGodFather_e,

	/*
		close connection hardly.
		layer needs to close/free what it holds. then send it to next layer.
		sent from NET_TCP_CloseHard()
	*/
	NET_TCP_QueueType_CloseHard_e
};

typedef void (*NET_TCP_Queue_File_cb_t)(NET_TCP_peer_t *, IO_fd_t, sint32_t /* err */, void */* userdata */);

typedef union{
	struct{
		void *ptr;
		uintptr_t size;
	}DynamicPointer;
	struct{
		void *ptr;
		uintptr_t size;
	}SpecialPointer;
	struct{
		void *ptr;
		uintptr_t size;
		NET_TCP_QueuerReference_t SignerNode;
		uint64_t UniqueNumber;
	}SignedSpecialPointer;
	struct{
		IO_fd_t infd;
		IO_off_t isize;
		IO_off_t nsize;
		NET_TCP_Queue_File_cb_t cb;
		void *userdata;
	}File;
}NET_TCP_Queue_t;
