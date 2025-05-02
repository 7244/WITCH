#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(include/syscall.h)

#define	NET_INADDR_ANY ((uint32_t)0)

#define NET_PF_LOCAL 1
#define NET_PF_UNIX NET_PF_LOCAL
#define NET_PF_INET 2

#define NET_AF_UNIX NET_PF_UNIX
#define NET_AF_INET NET_PF_INET

#define NET_SOL_SOCKET 1

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

void _NET_sockaddr_in_TO_addr(_NET_sockaddr_in_t *src, NET_addr_t *dst){
  dst->ip = e0swap32(src->sin_addr);
  dst->port = e0swap16(src->sin_port);
}
void _NET_addr_TO_sockaddr_in(const NET_addr_t *src, _NET_sockaddr_in_t *dst){
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

typedef struct{
  IO_fd_t fd;
}NET_socket_t;

sint32_t NET_shutdown(const NET_socket_t *sock){
  return syscall2(__NR_shutdown, sock->fd.fd, NET_SHUT_RDWR);
}
void NET_close(const NET_socket_t *sock){
  IO_close(&sock->fd);
}

sint32_t NET_accept(const NET_socket_t *ssock, NET_addr_t *addr, uint32_t flag, NET_socket_t *csock){
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

sint32_t NET_bind(const NET_socket_t *sock, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  return syscall3(__NR_bind, sock->fd.fd, (uintptr_t)&rawaddr, sizeof(_NET_sockaddr_in_t));
}

sint32_t NET_connect(const NET_socket_t *sock, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  return syscall3(__NR_connect, sock->fd.fd, (uintptr_t)&rawaddr, sizeof(_NET_sockaddr_in_t));
}

sint32_t NET_socket2(uint32_t domain, uint32_t type, uint32_t protocol, NET_socket_t *sock){
  sint32_t err = syscall3(__NR_socket, domain, type, protocol);
  if(err >= 0){
    sock->fd.fd = err;
    return 0;
  }
  return err;
}

IO_ssize_t NET_sendto(const NET_socket_t *sock, const void *data, IO_size_t size, const NET_addr_t *addr){
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

sintptr_t NET_sendmmsg(const NET_socket_t *sock, struct mmsghdr *msgvec, uint32_t vlen, uint32_t flags){
  return syscall4(__NR_sendmmsg, sock->fd.fd, (uintptr_t)msgvec, vlen, flags);
}

IO_ssize_t NET_recvfrom(const NET_socket_t *sock, void *data, IO_size_t size, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  sint32_t addrlen = sizeof(rawaddr);
  IO_ssize_t len = syscall6(__NR_recvfrom, sock->fd.fd, (uintptr_t)data, size, 0, (uintptr_t)&rawaddr, (uintptr_t)&addrlen);
  if(len < 0){
    return len;
  }
  _NET_sockaddr_in_TO_addr(&rawaddr, addr);
  return len;
}

sint32_t NET_listen(const NET_socket_t *sock){
  return syscall2(__NR_listen, sock->fd.fd, 0x80000000 - 0x40);
}

sint32_t NET_setsockopt(const NET_socket_t *sock, sint32_t level, sint32_t optname, sint32_t value){
  return syscall5(__NR_setsockopt, sock->fd.fd, level, optname, (uintptr_t)&value, sizeof(sint32_t));
}

sint32_t NET_getsockopt(const NET_socket_t *sock, sint32_t level, sint32_t optname, void *value){
  sint32_t optlen = sizeof(sint32_t);
  return syscall5(__NR_getsockopt, sock->fd.fd, level, optname, (uintptr_t)value, (uintptr_t)&optlen);
}

sint32_t NET_sockpair(NET_socket_t *socks){
  return syscall4(__NR_socketpair, NET_AF_UNIX, NET_SOCK_STREAM, 0, (uintptr_t)socks);
}
