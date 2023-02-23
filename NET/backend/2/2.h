#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(include/syscall.h)
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

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
  dst->sin_family = AF_INET;
  dst->sin_port = e0swap16(src->port);
  dst->sin_addr = e0swap32(src->ip);
  *(uint64_t *)dst->sin_zero = 0;
}

typedef IO_fd_t NET_socket_t;
#define NET_reuseport SO_REUSEPORT

sint32_t NET_sockclose(NET_socket_t sock){
  sint32_t err;
  err = -syscall2_noerr(SYS_shutdown, sock, SHUT_RDWR);
  if(err){
    if(err != -ENOTCONN){
      return err;
    }
  }
  return IO_close(sock);
}

sint32_t NET_socketnb(NET_socket_t sock, bool state){
  sint32_t flag = state;
  bool carry;
  sint32_t r = syscall3_carry(&carry, SYS_ioctl, sock, FIONBIO, &flag);
  if(carry){
    return -r;
  }
  return r;
}

NET_socket_t NET_accept(NET_socket_t sock, NET_addr_t *addr, uint32_t flag){
  _NET_sockaddr_in_t rawaddr;
  socklen_t op_size = sizeof(_NET_sockaddr_in_t);
  NET_socket_t csock;
  bool carry;
  if(flag){
    csock = syscall4_carry(&carry, SYS_accept4, sock, &rawaddr, &op_size, flag);
  }
  else{
    csock = syscall3_carry(&carry, SYS_accept, sock, &rawaddr, &op_size);
  }
  if(carry){
    return -csock;
  }
  _NET_sockaddr_in_TO_addr(&rawaddr, addr);
  return csock;
}

sint32_t NET_bind(NET_socket_t sock, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  return -syscall3_noerr(SYS_bind, sock, &rawaddr, sizeof(_NET_sockaddr_in_t));
}

sint32_t NET_connect(NET_socket_t sock, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  bool carry;
  sint32_t r = syscall3_carry(&carry, SYS_connect, sock, &rawaddr, sizeof(_NET_sockaddr_in_t));
  if(carry){
    return -r;
  }
  return r;
}

NET_socket_t NET_socket(uint32_t domain, uint32_t type, uint32_t protocol){
  bool carry;
  NET_socket_t r = syscall3_carry(&carry, SYS_socket, domain, type, protocol);
  if(carry){
    return -r;
  }
  return r;
}

IO_ssize_t NET_sendto(NET_socket_t sock, const void *data, IO_size_t size, const NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  bool carry;
  IO_ssize_t len = syscall6_carry(&carry, SYS_sendto, sock, data, size, 0, &rawaddr, sizeof(rawaddr));
  if(carry){
    if(len == EAGAIN){
      return 0;
    }
    return -len;
  }
  return len;
}

IO_ssize_t NET_recvfrom(NET_socket_t sock, void *data, IO_size_t size, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  sint32_t addrlen = sizeof(rawaddr);
  bool carry;
  IO_ssize_t len = syscall6_carry(&carry, SYS_recvfrom, sock, data, size, 0, &rawaddr, &addrlen);
  if(carry){
    return -len;
  }
  _NET_sockaddr_in_TO_addr(&rawaddr, addr);
  return len;
}

sint32_t NET_listen(NET_socket_t sock){
  return -syscall2_noerr(SYS_listen, sock, 0x80000000 - 0x40);
}

sint32_t NET_setsockopt(NET_socket_t sock, sint32_t level, sint32_t optname, sint32_t value){
  return -syscall5_noerr(SYS_setsockopt, sock, level, optname, &value, sizeof(sint32_t));
}

sint32_t NET_getsockopt(NET_socket_t sock, sint32_t level, sint32_t optname, void *value){
  sint32_t optlen = sizeof(sint32_t);
  return -syscall5_noerr(SYS_getsockopt, sock, level, optname, value, &optlen);
}

sint32_t NET_sockpair(NET_socket_t *socks){
  return -syscall4_noerr(SYS_socketpair, AF_UNIX, SOCK_STREAM, 0, socks);
}
