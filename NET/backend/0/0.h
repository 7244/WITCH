#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(include/syscall.h)
#include _WITCH_PATH(STR/common/common.h)
#include _WITCH_PATH(STR/psh.h)

#define NET_INADDR_ANY ((uint32_t)0)

#define NET_PF_LOCAL 1
#define NET_PF_UNIX NET_PF_LOCAL
#define NET_PF_INET 2
#define NET_AF_AX25 3
#define NET_AF_IPX 4
#define NET_AF_APPLETALK 5
#define NET_AF_NETROM 6
#define NET_AF_BRIDGE 7
#define NET_AF_ATMPVC 8
#define NET_AF_X25 9
#define NET_AF_INET6 10
#define NET_AF_ROSE 11
#define NET_AF_DECnet 12
#define NET_AF_NETBEUI 13
#define NET_AF_SECURITY 14
#define NET_AF_KEY 15
#define NET_AF_NETLINK 1
#define NET_AF_ROUTE AF_NETLINK
#define NET_AF_PACKET 17
#define NET_AF_ASH 18
#define NET_AF_ECONET 19
#define NET_AF_ATMSVC 20
#define NET_AF_RDS 21
#define NET_AF_SNA 22
#define NET_AF_IRDA 23
#define NET_AF_PPPOX 24
#define NET_AF_WANPIPE 25
#define NET_AF_LLC 26
#define NET_AF_IB 27
#define NET_AF_MPLS 28
#define NET_AF_CAN 29
#define NET_AF_TIPC 30
#define NET_AF_BLUETOOTH 31
#define NET_AF_IUCV 32
#define NET_AF_RXRPC 33
#define NET_AF_ISDN 34
#define NET_AF_PHONET 35
#define NET_AF_IEEE802154 36
#define NET_AF_CAIF 37
#define NET_AF_ALG 38
#define NET_AF_NFC 39
#define NET_AF_VSOCK 40
#define NET_AF_KCM 41
#define NET_AF_QIPCRTR 42
#define NET_AF_SMC 43
#define NET_AF_XDP 44
#define NET_AF_MCTP 4

#define NET_AF_UNIX NET_PF_UNIX
#define NET_AF_INET NET_PF_INET

#define NET_SOL_SOCKET 1

#define NET_SOL_IP 0
#define NET_SOL_TCP 6
#define NET_SOL_UDP 17
#define NET_SOL_IPV6 41
#define NET_SOL_ICMPV6 58
#define NET_SOL_SCTP 132
#define NET_SOL_UDPLITE 136
#define NET_SOL_RAW 255
#define NET_SOL_IPX 256
#define NET_SOL_AX25 257
#define NET_SOL_ATALK 258
#define NET_SOL_NETROM 259
#define NET_SOL_ROSE 260
#define NET_SOL_DECNET 261
#define NET_SOL_X25 262
#define NET_SOL_PACKET 263
#define NET_SOL_ATM 264
#define NET_SOL_AAL 265
#define NET_SOL_IRDA 266
#define NET_SOL_NETBEUI 267
#define NET_SOL_LLC 268
#define NET_SOL_DCCP 269
#define NET_SOL_NETLINK 270
#define NET_SOL_TIPC 271
#define NET_SOL_RXRPC 272
#define NET_SOL_PPPOL2TP 273
#define NET_SOL_BLUETOOTH 274
#define NET_SOL_PNPIPE 275
#define NET_SOL_RDS 276
#define NET_SOL_IUCV 277
#define NET_SOL_CAIF 278
#define NET_SOL_ALG 279
#define NET_SOL_NFC 280
#define NET_SOL_KCM 281
#define NET_SOL_TLS 282
#define NET_SOL_XDP 283
#define NET_SOL_MPTCP 284
#define NET_SOL_MCTP 285
#define NET_SOL_SMC 286
#define NET_SOL_VSOCK 287

#define NET_SO_DEBUG 1
#define NET_SO_REUSEADDR 2
#define NET_SO_TYPE 3
#define NET_SO_ERROR 4
#define NET_SO_DONTROUTE 5
#define NET_SO_BROADCAST 6
#define NET_SO_SNDBUF 7
#define NET_SO_RCVBUF 8
#define NET_SO_SNDBUFFORCE 32
#define NET_SO_RCVBUFFORCE 33
#define NET_SO_KEEPALIVE 9
#define NET_SO_OOBINLINE 10
#define NET_SO_NO_CHECK 11
#define NET_SO_PRIORITY 12
#define NET_SO_LINGER 13
#define NET_SO_BSDCOMPAT 14
#define NET_SO_REUSEPORT 15

#define NET_ETH_P_802_3 0x0001
#define NET_ETH_P_AX25 0x0002
#define NET_ETH_P_ALL 0x0003
#define NET_ETH_P_802_2 0x0004
#define NET_ETH_P_SNAP 0x0005
#define NET_ETH_P_DDCMP 0x0006
#define NET_ETH_P_WAN_PPP 0x0007
#define NET_ETH_P_PPP_MP 0x0008
#define NET_ETH_P_LOCALTALK 0x0009
#define NET_ETH_P_CAN 0x000C
#define NET_ETH_P_CANFD 0x000D
#define NET_ETH_P_CANXL 0x000E
#define NET_ETH_P_PPPTALK 0x0010
#define NET_ETH_P_TR_802_2 0x0011
#define NET_ETH_P_MOBITEX 0x0015
#define NET_ETH_P_CONTROL 0x0016
#define NET_ETH_P_IRDA 0x0017
#define NET_ETH_P_ECONET 0x0018
#define NET_ETH_P_HDLC 0x0019
#define NET_ETH_P_ARCNET 0x001A
#define NET_ETH_P_DSA 0x001B
#define NET_ETH_P_TRAILER 0x001C
#define NET_ETH_P_PHONET 0x00F5
#define NET_ETH_P_IEEE802154 0x00F6
#define NET_ETH_P_CAIF 0x00F7
#define NET_ETH_P_XDSA 0x00F8
#define NET_ETH_P_MAP 0x00F9

#define NET_PACKET_ADD_MEMBERSHIP 1
#define NET_PACKET_DROP_MEMBERSHIP 2
#define NET_PACKET_RECV_OUTPUT 3
#define NET_PACKET_RX_RING 5
#define NET_PACKET_STATISTICS 6
#define NET_PACKET_COPY_THRESH 7
#define NET_PACKET_AUXDATA 8
#define NET_PACKET_ORIGDEV 9
#define NET_PACKET_VERSION 10
#define NET_PACKET_HDRLEN 11
#define NET_PACKET_RESERVE 12
#define NET_PACKET_TX_RING 13
#define NET_PACKET_LOSS 14
#define NET_PACKET_VNET_HDR 15
#define NET_PACKET_TX_TIMESTAMP 16
#define NET_PACKET_TIMESTAMP 17
#define NET_PACKET_FANOUT 18
#define NET_PACKET_TX_HAS_OFF 19
#define NET_PACKET_QDISC_BYPASS 20
#define NET_PACKET_ROLLOVER_STATS 21
#define NET_PACKET_FANOUT_DATA 22
#define NET_PACKET_IGNORE_OUTGOING 23
#define NET_PACKET_VNET_HDR_SZ 24

typedef enum{
  NET_TPACKET_V1,
  NET_TPACKET_V2,
  NET_TPACKET_V3
}NET_tpacket_versions_t;

enum{
  NET_SHUT_RD = 0,
  NET_SHUT_WR,
  NET_SHUT_RDWR
};

enum{
  NET_SOCK_STREAM = 1,
  NET_SOCK_DGRAM = 2,
  NET_SOCK_RAW = 3,
  NET_SOCK_RDM = 4,
  NET_SOCK_SEQPACKET = 5,
  NET_SOCK_DCCP = 6,
  NET_SOCK_PACKET = 10,
  NET_SOCK_CLOEXEC = 02000000,
  NET_SOCK_NONBLOCK = 00004000
};

enum{
  NET_IPPROTO_IP = 0,
  NET_IPPROTO_ICMP = 1,
  NET_IPPROTO_IGMP = 2,
  NET_IPPROTO_IPIP = 4,
  NET_IPPROTO_TCP = 6,
  NET_IPPROTO_EGP = 8,
  NET_IPPROTO_PUP = 12,
  NET_IPPROTO_UDP = 17,
  NET_IPPROTO_IDP = 22,
  NET_IPPROTO_TP = 29,
  NET_IPPROTO_DCCP = 33,
  NET_IPPROTO_IPV6 = 41,
  NET_IPPROTO_RSVP = 46,
  NET_IPPROTO_GRE = 47,
  NET_IPPROTO_ESP = 50,
  NET_IPPROTO_AH = 51,
  NET_IPPROTO_MTP = 92,
  NET_IPPROTO_BEETPH = 94,
  NET_IPPROTO_ENCAP = 98,
  NET_IPPROTO_PIM = 103,
  NET_IPPROTO_COMP = 108,
  NET_IPPROTO_L2TP = 115,
  NET_IPPROTO_SCTP = 132,
  NET_IPPROTO_UDPLITE = 136,
  NET_IPPROTO_MPLS = 137,
  NET_IPPROTO_ETHERNET = 143,
  NET_IPPROTO_RAW = 255,
  NET_IPPROTO_SMC = 256,
  NET_IPPROTO_MPTCP = 262,
  NET_IPPROTO_MAX
};

#define NET_TCP_NODELAY 1
#define NET_TCP_MAXSEG 2
#define NET_TCP_CORK 3
#define NET_TCP_KEEPIDLE 4
#define NET_TCP_KEEPINTVL 5
#define NET_TCP_KEEPCNT 6
#define NET_TCP_SYNCNT 7
#define NET_TCP_LINGER2 8
#define NET_TCP_DEFER_ACCEPT 9
#define NET_TCP_WINDOW_CLAMP 10
#define NET_TCP_INFO 11
#define NET_TCP_QUICKACK 12
#define NET_TCP_CONGESTION 13
#define NET_TCP_MD5SIG 14
#define NET_TCP_THIN_LINEAR_TIMEOUTS 16
#define NET_TCP_THIN_DUPACK 17
#define NET_TCP_USER_TIMEOUT 18
#define NET_TCP_REPAIR 19
#define NET_TCP_REPAIR_QUEUE 20
#define NET_TCP_QUEUE_SEQ 21
#define NET_TCP_REPAIR_OPTIONS 22
#define NET_TCP_FASTOPEN 23
#define NET_TCP_TIMESTAMP 24
#define NET_TCP_NOTSENT_LOWAT 25
#define NET_TCP_CC_INFO 26
#define NET_TCP_SAVE_SYN 27
#define NET_TCP_SAVED_SYN 28
#define NET_TCP_REPAIR_WINDOW 29
#define NET_TCP_FASTOPEN_CONNECT 30
#define NET_TCP_ULP 31
#define NET_TCP_MD5SIG_EXT 32
#define NET_TCP_FASTOPEN_KEY 33
#define NET_TCP_FASTOPEN_NO_COOKIE 34
#define NET_TCP_ZEROCOPY_RECEIVE 35
#define NET_TCP_INQ 36

#define NET_TCP_CM_INQ NET_TCP_INQ

#define NET_TCP_TX_DELAY 37

#define NET_TCP_AO_ADD_KEY 38
#define NET_TCP_AO_DEL_KEY 39
#define NET_TCP_AO_INFO 40
#define NET_TCP_AO_GET_KEYS 41
#define NET_TCP_AO_REPAIR 42

#define NET_TCP_IS_MPTCP 43

#define NET_TCP_REPAIR_ON 1
#define NET_TCP_REPAIR_OFF 0
#define NET_TCP_REPAIR_OFF_NO_WP -1

typedef struct{
  sint16_t sin_family;
  uint16_t sin_port;
  uint32_t sin_addr;
  uint8_t sin_zero[8];
}_NET_sockaddr_in_t;

typedef struct{
  uint16_t sa_family;
  union{
    sint8_t sa_data_min[14];
    struct{
      struct{}__empty_sa_data;
      char sa_data[];
    };
  };
}NET_sockaddr_t;

typedef struct{
  uint16_t sll_family;
  uint16_t sll_protocol;
  sint32_t sll_ifindex;
  uint16_t sll_hatype;
  uint8_t sll_pkttype;
  uint8_t sll_halen;
  uint8_t sll_addr[8];
}NET_sockaddr_ll_t;

static void _NET_sockaddr_in_TO_addr(_NET_sockaddr_in_t *src, NET_addr_t *dst){
  dst->ip = e0swap32(src->sin_addr);
  dst->port = e0swap16(src->sin_port);
}
static void _NET_addr_TO_sockaddr_in(const NET_addr_t *src, _NET_sockaddr_in_t *dst){
  dst->sin_family = NET_AF_INET;
  dst->sin_port = e0swap16(src->port);
  dst->sin_addr = e0swap32(src->ip);
  *(uint64_t *)dst->sin_zero = 0;
}

typedef struct{
  void *iov_base;
  uintptr_t iov_len;
}NET_iovec_t;

typedef struct{
  void *msg_name;
  sint32_t msg_namelen;
  NET_iovec_t *msg_iov;
  uintptr_t msg_iovlen;
  void *msg_control;
  uintptr_t msg_controllen;
  uint32_t msg_flags;
}NET_msghdr_t;

typedef struct{
  NET_msghdr_t msg_hdr;
  uint32_t msg_len;
}NET_mmsghdr_t;

#define NET_IFNAMSIZ 16

#define NET_SIOCADDRT 0x890B
#define NET_SIOCDELRT 0x890C
#define NET_SIOCRTMSG 0x890D
#define NET_SIOCGIFNAME 0x8910
#define NET_SIOCSIFLINK 0x8911
#define NET_SIOCGIFCONF 0x8912
#define NET_SIOCGIFFLAGS 0x8913
#define NET_SIOCSIFFLAGS 0x8914
#define NET_SIOCGIFADDR 0x8915
#define NET_SIOCSIFADDR 0x8916
#define NET_SIOCGIFDSTADDR 0x8917
#define NET_SIOCSIFDSTADDR 0x8918
#define NET_SIOCGIFBRDADDR 0x8919
#define NET_SIOCSIFBRDADDR 0x891a
#define NET_SIOCGIFNETMASK 0x891b
#define NET_SIOCSIFNETMASK 0x891c
#define NET_SIOCGIFMETRIC 0x891d
#define NET_SIOCSIFMETRIC 0x891e
#define NET_SIOCGIFMEM 0x891f
#define NET_SIOCSIFMEM 0x8920
#define NET_SIOCGIFMTU 0x8921
#define NET_SIOCSIFMTU 0x8922
#define NET_SIOCSIFNAME 0x8923
#define NET_SIOCSIFHWADDR 0x8924
#define NET_SIOCGIFENCAP 0x8925
#define NET_SIOCSIFENCAP 0x8926
#define NET_SIOCGIFHWADDR 0x8927
#define NET_SIOCGIFSLAVE 0x8929
#define NET_SIOCSIFSLAVE 0x893
#define NET_SIOCADDMULTI 0x8931
#define NET_SIOCDELMULTI 0x893
#define NET_SIOCGIFINDEX 0x8933
#define NET_SIOGIFINDEX SIOCGIFINDEX
#define NET_SIOCSIFPFLAGS 0x8934
#define NET_SIOCGIFPFLAGS 0x893
#define NET_SIOCDIFADDR 0x8936
#define NET_SIOCSIFHWBROADCAST 0x8937
#define NET_SIOCGIFCOUNT 0x8938
#define NET_SIOCGIFBR 0x8940
#define NET_SIOCSIFBR 0x8941
#define NET_SIOCGIFTXQLEN 0x8942
#define NET_SIOCSIFTXQLEN 0x8943
#define NET_SIOCETHTOOL 0x8946
#define NET_SIOCGMIIPHY 0x8947
#define NET_SIOCGMIIREG 0x8948
#define NET_SIOCSMIIREG 0x8949
#define NET_SIOCWANDEV 0x894A
#define NET_SIOCOUTQNSD 0x894B
#define NET_SIOCGSKNS 0x894C
#define NET_SIOCDARP 0x8953
#define NET_SIOCGARP 0x8954
#define NET_SIOCSARP 0x8955
#define NET_SIOCDRARP 0x8960
#define NET_SIOCGRARP 0x8961
#define NET_SIOCSRARP 0x8962
#define NET_SIOCGIFMAP 0x8970
#define NET_SIOCSIFMAP 0x8971
#define NET_SIOCADDDLCI 0x8980
#define NET_SIOCDELDLCI 0x8981
#define NET_SIOCGIFVLAN 0x8982
#define NET_SIOCSIFVLAN 0x8983
#define NET_SIOCBONDENSLAVE 0x8990
#define NET_SIOCBONDRELEASE 0x8991
#define NET_SIOCBONDSETHWADDR 0x8992
#define NET_SIOCBONDSLAVEINFOQUERY 0x8993
#define NET_SIOCBONDINFOQUERY 0x8994
#define NET_SIOCBONDCHANGEACTIVE 0x8995
#define NET_SIOCBRADDBR 0x89a0
#define NET_SIOCBRDELBR 0x89a1
#define NET_SIOCBRADDIF 0x89a2
#define NET_SIOCBRDELIF 0x89a3
#define NET_SIOCSHWTSTAMP 0x89b0
#define NET_SIOCGHWTSTAMP 0x89b1
#define NET_SIOCDEVPRIVATE 0x89F0
#define NET_SIOCPROTOPRIVATE 0x89E0

typedef struct{
  NET_sockaddr_t arp_pa;
  NET_sockaddr_t arp_ha;
  uint32_t arp_flags;
  NET_sockaddr_t arp_netmask;
  sint8_t arp_dev[NET_IFNAMSIZ];
}NET_arpreq_t;

typedef struct{
  uintptr_t mem_start;
  uintptr_t mem_end;
  uint16_t base_addr;
  uint8_t irq;
  uint8_t dma;
  uint8_t port;
}NET_ifmap_t;

typedef struct{
  sint8_t ifr_name[NET_IFNAMSIZ];
  union{
    NET_sockaddr_t ifr_addr;
    NET_sockaddr_t ifr_dstaddr;
    NET_sockaddr_t ifr_broadaddr;
    NET_sockaddr_t ifr_netmask;
    NET_sockaddr_t ifr_hwaddr;
    sint16_t ifr_flags;
    sint32_t ifr_ifindex;
    sint32_t ifr_metric;
    sint32_t ifr_mtu;
    NET_ifmap_t ifr_map;
    sint8_t ifr_slave[NET_IFNAMSIZ];
    sint8_t ifr_newname[NET_IFNAMSIZ];
    sint8_t *ifr_data;
  };
}NET_ifreq_t;

#define NET_TP_STATUS_AVAILABLE 0
#define NET_TP_STATUS_SEND_REQUEST (1 << 0)
#define NET_TP_STATUS_SENDING (1 << 1)
#define NET_TP_STATUS_WRONG_FORMAT (1 << 2)

#define NET_TPACKET_ALIGNMENT 16
#define NET_TPACKET_ALIGN(x) (((x)+NET_TPACKET_ALIGNMENT-1)&~(NET_TPACKET_ALIGNMENT-1))
#define NET_TPACKET_HDRLEN (NET_TPACKET_ALIGN(sizeof(NET_tpacket_hdr_t)) + sizeof(NET_sockaddr_ll_t))

typedef struct{
  uint32_t tp_block_size;
  uint32_t tp_block_nr;
  uint32_t tp_frame_size;
  uint32_t tp_frame_nr;
}NET_tpacket_req_t;

typedef struct{
  uint32_t tp_status;
  uint32_t tp_len;
  uint32_t tp_snaplen;
  uint16_t tp_mac;
  uint16_t tp_net;
  uint32_t tp_sec;
  uint32_t tp_nsec;
  uint16_t tp_vlan_tci;
  uint16_t tp_vlan_tpid;
  uint8_t tp_padding[4];
}NET_tpacket2_hdr_t;

typedef struct{
  IO_fd_t fd;
}NET_socket_t;

static sint32_t NET_shutdown(const NET_socket_t *sock){
  return syscall2(__NR_shutdown, sock->fd.fd, NET_SHUT_RDWR);
}
static void NET_close(const NET_socket_t *sock){
  IO_close(&sock->fd);
}

static sint32_t NET_accept(const NET_socket_t *ssock, NET_addr_t *addr, uint32_t flag, NET_socket_t *csock){
  _NET_sockaddr_in_t rawaddr;
  uint32_t op_size = sizeof(_NET_sockaddr_in_t);
  sint32_t err;
  if(flag){
    err = syscall4(__NR_accept4, ssock->fd.fd, (uintptr_t)&rawaddr, (uintptr_t)&op_size, flag);
  }
  else{
    /* no idea about this check */
    /* i dont have __NR_accept if i compile with -m32 */
    #ifdef __NR_accept
      err = syscall3(__NR_accept, ssock->fd.fd, (uintptr_t)&rawaddr, (uintptr_t)&op_size);
    #else
      err = syscall4(__NR_accept4, ssock->fd.fd, (uintptr_t)&rawaddr, (uintptr_t)&op_size, 0);
    #endif
  }
  if(err >= 0){
    csock->fd.fd = err;
    _NET_sockaddr_in_TO_addr(&rawaddr, addr);
    return 0;
  }
  return err;
}

static sintptr_t NET_bind_raw(const NET_socket_t *sock, void *addr, uintptr_t size){
  return syscall3(__NR_bind, sock->fd.fd, (uintptr_t)addr, size);
}

static sint32_t NET_bind(const NET_socket_t *sock, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  return syscall3(__NR_bind, sock->fd.fd, (uintptr_t)&rawaddr, sizeof(_NET_sockaddr_in_t));
}

static sint32_t NET_connect(const NET_socket_t *sock, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  return syscall3(__NR_connect, sock->fd.fd, (uintptr_t)&rawaddr, sizeof(_NET_sockaddr_in_t));
}

static sint32_t NET_socket2(uint32_t domain, uint32_t type, uint32_t protocol, NET_socket_t *sock){
  sint32_t err = syscall3(__NR_socket, domain, type, protocol);
  if(err >= 0){
    sock->fd.fd = err;
    return 0;
  }
  return err;
}

static IO_ssize_t NET_sendto(const NET_socket_t *sock, const void *data, IO_size_t size, const NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  IO_ssize_t len = syscall6(__NR_sendto, sock->fd.fd, (uintptr_t)data, size, 0, (uintptr_t)&rawaddr, sizeof(rawaddr));
  if(len < 0){
    if(len == -EAGAIN){
      return 0;
    }
  }
  return len;
}

static sintptr_t NET_sendmmsg(const NET_socket_t *sock, NET_mmsghdr_t *msgvec, uint32_t vlen, uint32_t flags){
  return syscall4(__NR_sendmmsg, sock->fd.fd, (uintptr_t)msgvec, vlen, flags);
}

static IO_ssize_t NET_recvfrom(const NET_socket_t *sock, void *data, IO_size_t size, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  sint32_t addrlen = sizeof(rawaddr);
  IO_ssize_t len = syscall6(__NR_recvfrom, sock->fd.fd, (uintptr_t)data, size, 0, (uintptr_t)&rawaddr, (uintptr_t)&addrlen);
  if(len < 0){
    return len;
  }
  _NET_sockaddr_in_TO_addr(&rawaddr, addr);
  return len;
}

static sint32_t NET_listen(const NET_socket_t *sock){
  return syscall2(__NR_listen, sock->fd.fd, 0x80000000 - 0x40);
}

static sint32_t NET_setsockopt_raw(const NET_socket_t *sock, sint32_t level, sint32_t optname, void *value, uintptr_t value_size){
  return syscall5(__NR_setsockopt, sock->fd.fd, level, optname, (uintptr_t)value, value_size);
}

static sint32_t NET_setsockopt(const NET_socket_t *sock, sint32_t level, sint32_t optname, sint32_t value){
  return syscall5(__NR_setsockopt, sock->fd.fd, level, optname, (uintptr_t)&value, sizeof(sint32_t));
}

static sint32_t NET_getsockopt(const NET_socket_t *sock, sint32_t level, sint32_t optname, void *value){
  sint32_t optlen = sizeof(sint32_t);
  return syscall5(__NR_getsockopt, sock->fd.fd, level, optname, (uintptr_t)value, (uintptr_t)&optlen);
}

static sint32_t NET_sockpair(NET_socket_t *socks){
  return syscall4(__NR_socketpair, NET_AF_UNIX, NET_SOCK_STREAM, 0, (uintptr_t)socks);
}

static sintptr_t NET_ctl3(NET_socket_t *sock, uint32_t op, void *val){
  return IO_ctl3(&sock->fd, op, val);
}
static sintptr_t NET_ctl2(NET_socket_t *sock, uint32_t op){
  return IO_ctl2(&sock->fd, op);
}

static sint32_t NET_GetDefaultGateway32_ifname_cstr(uint32_t *gateway32, const void *ifname_cstr){
  IO_fd_t fd;

  sint32_t err = IO_open("/proc/net/route", O_RDONLY, &fd);
  if(err){
    return err;
  }

  uint8_t buff[2048];

  IO_ssize_t read_size = IO_read(&fd, buff, sizeof(buff));
  if(read_size < 0){
    goto gt_fail_after_fd_open;
  }

  uintptr_t index = 0;
  while(1){
    if(STR_GetIndexAfterSkipNXCharacters_safe(buff, &index, read_size, 1, '\n')){
      /* TODO read more */
      goto gt_fail_after_fd_open;
    }

    uintptr_t ifname_index = index;

    if(STR_FindCharacterIndexN_safe(buff, &index, read_size, 1, '\t')){
      /* TODO read more */
      goto gt_fail_after_fd_open;
    }

    if(STR_n0ncmp(ifname_cstr, &buff[ifname_index], index - ifname_index)){
      continue;
    }

    index++;

    uintptr_t destination_index = index;

    if(STR_FindCharacterIndexN_safe(buff, &index, read_size, 1, '\t')){
      /* TODO read more */
      goto gt_fail_after_fd_open;
    }

    uint32_t dstip32 = STR_psh32_digit(&buff[destination_index], index - destination_index);

    if(dstip32 != 0){
      continue;
    }

    index++;

    uintptr_t gateway_index = index;

    if(STR_FindCharacterIndexN_safe(buff, &index, read_size, 1, '\t')){
      /* TODO read more */
      goto gt_fail_after_fd_open;
    }

    *gateway32 = STR_psh32_digit(&buff[gateway_index], index - gateway_index);

    break;

    gt_fail_after_fd_open:;
    err = -1;

    break;
  }

  IO_close(&fd);

  return err;
}


static sint32_t NET_GetMacAddressByGateway32_ifname_cstr(
  void *mac_addr,
  uint32_t gateway32,
  const void *ifname_cstr
){
  NET_arpreq_t areq = {};
  __builtin_memcpy(areq.arp_dev, ifname_cstr, MEM_cstreu(ifname_cstr) + 1);

  _NET_sockaddr_in_t *sin = (_NET_sockaddr_in_t *)&areq.arp_pa;
  sin->sin_family = NET_AF_INET;
  sin->sin_addr = NET_hton32(gateway32);

  {
    NET_socket_t sock;
    if(NET_socket2(NET_AF_INET, NET_SOCK_DGRAM, 0, &sock) != 0){
      __abort();
    }
  
    sint32_t err = NET_ctl3(&sock, NET_SIOCGARP, &areq);
  
    NET_close(&sock);
  
    if(err != 0){
      return err;
    }
  }

  __builtin_memcpy(mac_addr, areq.arp_ha.sa_data, 6);

  return 0;
}

static sint32_t NET_GetDefaultRouteMacAddress_ifname_cstr(void *mac_addr, const void *ifname_cstr){
  uint32_t gateway32;
  sint32_t errs32 = NET_GetDefaultGateway32_ifname_cstr(&gateway32, ifname_cstr);
  if(errs32){
    return errs32;
  }

  errs32 = NET_GetMacAddressByGateway32_ifname_cstr(mac_addr, gateway32, ifname_cstr);
  if(errs32 != 0){
    if(STR_n0cmp("lo", ifname_cstr)){
      return errs32;
    }

    __builtin_memset(mac_addr, 0, 6);

    return 0;
  }

  return errs32;
}
