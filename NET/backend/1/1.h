#include _WITCH_PATH(IO/IO.h)
#include _WITCH_PATH(include/windows/windows.h)
#include _WITCH_PATH(include/windows/winsock.h)

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
  *(uint64_t *)dst->sin_zero = 0; /* probably faster than memcpy */
}

typedef struct{
  IO_fd_t fd;
}NET_socket_t;
#define NET_reuseport SO_REUSEADDR

sint32_t NET_shutdown(const NET_socket_t *sock){
  if(shutdown(sock->fd.fd, SD_BOTH) == SOCKET_ERROR){
    int err = WSAGetLastError();
    if(err == WSAENOTCONN){
      return 0;
    }
    return -1;
  };
  return 0;
}

void NET_close(const NET_socket_t *sock){
  if(closesocket(sock->fd.fd) == SOCKET_ERROR){
    PR_abort();
  }
  _IO_assign_fd(&sock->fd, _IO_fd_unknown_e);
}

sint32_t NET_socketnb(const NET_socket_t *sock, bool state){
  u_long mode = state;
  if(ioctlsocket(sock->fd.fd, FIONBIO, &mode) == SOCKET_ERROR){
    return -1;
  }
  return 0;
}

sint32_t NET_accept(const NET_socket_t *ssock, NET_addr_t *addr, uint32_t flag, NET_socket_t *csock){
  _NET_sockaddr_in_t rawaddr;
  socklen_t op_size = sizeof(_NET_sockaddr_in_t);
  NET_socket_t tsock;
  sint32_t err;
  if(flag & SOCK_NONBLOCK){
    err = accept(ssock->fd.fd, (struct sockaddr *)&rawaddr, &op_size);
    if(err == -1){
      return -1;
    }
    tsock.fd.fd = err;
    if(NET_socketnb(&tsock, 1)){
      NET_close(&tsock);
      return -1;
    }
  }
  else{
    err = accept(ssock->fd.fd, (struct sockaddr *)&rawaddr, &op_size);
    tsock.fd.fd = err;
  }
  if(err >= 0){
    _IO_assign_fd(&tsock.fd, _IO_fd_socket_e);
    _NET_sockaddr_in_TO_addr(&rawaddr, addr);
  }
  *csock = tsock;
  return 0;
}

sint32_t NET_bind(const NET_socket_t *sock, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  if(bind(sock->fd.fd, (const struct sockaddr *)&rawaddr, (socklen_t)sizeof(_NET_sockaddr_in_t)) == SOCKET_ERROR){
    return -1;
  }
  return 0;
}

sint32_t NET_connect(const NET_socket_t *sock, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  int ret = connect(sock->fd.fd, (const struct sockaddr *)&rawaddr, (socklen_t)sizeof(_NET_sockaddr_in_t));
  if(ret == SOCKET_ERROR){
    int err = WSAGetLastError();
    switch(err){
      case WSAEWOULDBLOCK:{
        return -EINPROGRESS;
      }
      case WSAENETUNREACH:{
        return -ENETUNREACH;
      }
      default:{
        PR_abort();
      }
    }
  }
  return ret;
}

sint32_t NET_socket(uint32_t domain, uint32_t type, uint32_t protocol, NET_socket_t *sock){
  NET_socket_t tsock;
  sint32_t err;
  if(type & SOCK_NONBLOCK){
    type ^= SOCK_NONBLOCK;
    err = socket(domain, type, protocol);
    if(err == INVALID_SOCKET){
      return -1;
    }
    tsock.fd.fd = err;
    if(NET_socketnb(&tsock, 1)){
      NET_close(&tsock);
      return -1;
    }
  }
  else{
    err = socket(domain, type, protocol);
    if(err == INVALID_SOCKET){
      return -1;
    }
    tsock.fd.fd = err;
  }
  *sock = tsock;
  _IO_assign_fd(&sock->fd, _IO_fd_socket_e);
  return 0;
}

IO_ssize_t NET_sendto(const NET_socket_t *sock, const void *data, IO_size_t size, const NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  _NET_addr_TO_sockaddr_in(addr, &rawaddr);
  int len = sendto((SOCKET)sock->fd.fd, (const char *)data, size, 0, (const struct sockaddr *)&rawaddr, sizeof(rawaddr));
  if(len == SOCKET_ERROR){
    int err = WSAGetLastError();
    if(err == WSAEWOULDBLOCK){
      return 0;
    }
    return -1;
  }
  return len;
}

IO_ssize_t NET_recvfrom(const NET_socket_t *sock, void *data, IO_size_t size, NET_addr_t *addr){
  _NET_sockaddr_in_t rawaddr;
  int addrlen = sizeof(rawaddr);
  int len = recvfrom((SOCKET)sock->fd.fd, (char *)data, (int)size, 0, (struct sockaddr *)&rawaddr, &addrlen);
  if(len == SOCKET_ERROR){
    int err = WSAGetLastError();
    switch(err){
      case WSAEWOULDBLOCK:
      case WSAECONNRESET:{
        return -EAGAIN;
      }
      default:{
        return -1;
      }
    }
  }
  _NET_sockaddr_in_TO_addr(&rawaddr, addr);
  return len;
}

sint32_t NET_listen(const NET_socket_t *sock){
  if(listen(sock->fd.fd, 0x80000000 - 0x40) == SOCKET_ERROR){
    return -1;
  }
  return 0;
}

sint32_t NET_setsockopt(const NET_socket_t *sock, sint32_t level, sint32_t optname, sint32_t value){
  int size = sizeof(int);
  switch(optname){
    case TCP_NODELAY:{
      size = 1;
      break;
    }
  }
  if(setsockopt(sock->fd.fd, level, optname, (const char *)&value, size) == SOCKET_ERROR){
    return -1;
  }
  return 0;
}

sint32_t NET_getsockopt(const NET_socket_t *sock, sint32_t level, sint32_t optname, void *value){
  sint32_t optlen = sizeof(sint32_t);
  if(getsockopt(sock->fd.fd, level, optname, (char *)value, &optlen) == SOCKET_ERROR){
    return -1;
  }
  return 0;
}

sint32_t NET_sockpair(NET_socket_t *socks){
  union{
    struct sockaddr_in inaddr;
    struct sockaddr addr;
  }a;
  SOCKET listener;
  int e;
  socklen_t addrlen = sizeof(a.inaddr);
  int reuse = 1;

  listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if(listener == INVALID_SOCKET){
    return -1;
  }

  memset(&a, 0, sizeof(a));
  a.inaddr.sin_family = AF_INET;
  a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  a.inaddr.sin_port = 0; 

  socks[0].fd.fd = INVALID_SOCKET;
  socks[1].fd.fd = INVALID_SOCKET;
  do{
    if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, (socklen_t)sizeof(reuse)) == SOCKET_ERROR){
      break;
    }
    if(bind(listener, &a.addr, sizeof(a.inaddr)) == SOCKET_ERROR){
      break;
    }
    if(getsockname(listener, &a.addr, &addrlen) == SOCKET_ERROR){
      break;
    }
    if(listen(listener, 1) == SOCKET_ERROR){
      break;
    }
    socks[0].fd.fd = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(socks[0].fd.fd == INVALID_SOCKET){
      break;
    }
    if(connect(socks[0].fd.fd, &a.addr, sizeof(a.inaddr)) == SOCKET_ERROR){
      break;
    }
    socks[1].fd.fd = accept(listener, NULL, NULL);
    if(socks[1].fd.fd == INVALID_SOCKET){
      break;
    }
    _IO_assign_fd(&socks[0].fd, _IO_fd_socket_e);
    _IO_assign_fd(&socks[1].fd, _IO_fd_socket_e);

    closesocket(listener);
    return 0;
  }while(0);

  e = WSAGetLastError();
  closesocket(listener);
  closesocket(socks[0].fd.fd);
  closesocket(socks[1].fd.fd);
  WSASetLastError(e);
  return -1;
}
