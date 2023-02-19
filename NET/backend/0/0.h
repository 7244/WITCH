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

typedef struct{
	IO_fd_t fd;
}NET_socket_t;
#define NET_reuseport SO_REUSEPORT

sint32_t NET_shutdown(const NET_socket_t *sock){
	return syscall2(__NR_shutdown, sock->fd.fd, SHUT_RDWR);
}
void NET_close(const NET_socket_t *sock){
	IO_close(&sock->fd);
}

sint32_t NET_accept(const NET_socket_t *ssock, NET_addr_t *addr, uint32_t flag, NET_socket_t *csock){
	_NET_sockaddr_in_t rawaddr;
	socklen_t op_size = sizeof(_NET_sockaddr_in_t);
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

sint32_t NET_socket(uint32_t domain, uint32_t type, uint32_t protocol, NET_socket_t *sock){
	sint32_t err = syscall3(__NR_socket, domain, type | SOCK_NONBLOCK, protocol);
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
	return syscall4(__NR_socketpair, AF_UNIX, SOCK_STREAM, 0, (uintptr_t)socks);
}
