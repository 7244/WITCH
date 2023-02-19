#if defined(WL_CPP)
	extern "C"{
#endif
#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/socket.h>
#include <linux/slab.h>
#if defined(WL_CPP)
	}
#endif

void _NET_sockaddr_in_TO_addr(uint16_t af, struct sockaddr_in *src, void *dst){
	NET_addr_t *addr = (NET_addr_t *)src;
	addr->ip = e0swap32(src->sin_addr.s_addr);
	addr->port = e0swap16(src->sin_port);
}
void _NET_addr_TO_sockaddr_in(uint16_t af, const void *src, struct sockaddr_in *dst){
	NET_addr_t *addr = (NET_addr_t *)src;
	dst->sin_family = af;
	dst->sin_port = e0swap16(addr->port);
	dst->sin_addr.s_addr = e0swap32(addr->ip);
	*(uint64_t *)dst->sin_zero = 0;
}

typedef struct{
	struct socket *ptr;
}NET_socket_t;
#define NET_reuseport SO_REUSEPORT

sint32_t NET_shutdown(const NET_socket_t *sock){
	return sock->ptr->ops->shutdown(sock->ptr, SHUT_RDWR);
}
void NET_close(const NET_socket_t *sock){
	sock_release(sock->ptr);
}

#if 0
sint32_t NET_socketnb(const NET_socket_t *sock, bool state){
	sint32_t flag = state;
	return syscall3(__NR_ioctl, sock->fd.fd, FIONBIO, (uintptr_t)&flag);
}

sint32_t NET_accept(const NET_socket_t *ssock, NET_addr_t *addr, uint32_t flag, NET_socket_t *csock){
	struct sockaddr_in rawaddr;
	socklen_t op_size = sizeof(struct sockaddr_in);
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
#endif

#if 0
sint32_t NET_bind(const NET_socket_t *sock, NET_addr_t *addr){
	struct sockaddr_in rawaddr;
	_NET_addr_TO_sockaddr_in(addr, &rawaddr);
	return syscall3(__NR_bind, sock->fd.fd, (uintptr_t)&rawaddr, sizeof(struct sockaddr_in));
}
#endif

sint32_t NET_connect(const NET_socket_t *sock, uint16_t af, void *addr){
	struct sockaddr_in rawaddr;
	_NET_addr_TO_sockaddr_in(af, addr, &rawaddr);
	return sock->ptr->ops->connect(sock->ptr, (struct sockaddr *)&rawaddr, sizeof(rawaddr), O_RDWR | SOCK_NONBLOCK);
}

sint32_t NET_socket(uint32_t domain, uint32_t type, uint32_t protocol, NET_socket_t *sock){
	struct socket *socket_ptr;
	int ret = sock_create(domain, type, protocol, &socket_ptr);
	if(ret){
		return ret;
	}
	sock->ptr = socket_ptr;
	return 0;
}

#if 0
IO_ssize_t NET_sendto(const NET_socket_t *sock, const void *data, IO_size_t size, const NET_addr_t *addr){
	struct sockaddr_in rawaddr;
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
	struct sockaddr_in rawaddr;
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
#endif
