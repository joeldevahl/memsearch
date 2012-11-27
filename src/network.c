#ifdef FAMILY_WINDOWS

#define WIN32_LEAN_AND_MEAN 
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define ssize_t SSIZE_T

void network_init()
{
	WSADATA wsa_data;
	int err = WSAStartup(MAKEWORD(1, 1), &wsa_data);
	ASSERT(err == 0, "network initialization failed.");
}

#else

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <netdb.h>      
#include <netinet/in.h>
#include <fcntl.h>

void network_init()
{
}

#endif

#include <network/network.h>
#include <core/assert.h>
#include <core/memory.h>

#define AS_PTR(i) ((struct network_socket*)((uintptr_t)i))
#define FROM_PTR(p) ((int)((uintptr_t)p))

static void netaddr_to_sockaddr(const struct network_address* src, struct sockaddr *dest)
{
	struct sockaddr_in *p = (struct sockaddr_in *)dest;
	memory_zero(p, sizeof(struct sockaddr_in));
	p->sin_family = AF_INET;
	p->sin_port = htons(src->port);
	p->sin_addr.s_addr = htonl(src->ip[0]<<24 | src->ip[1]<<16 | src->ip[2]<<8 | src->ip[3]);
}

static void sockaddr_to_netaddr(const struct sockaddr *src, struct network_address* dst)
{
	unsigned int ip = htonl(((struct sockaddr_in*)src)->sin_addr.s_addr);
	memory_zero(dst, sizeof(struct network_address));
	dst->port = htons(((struct sockaddr_in*)src)->sin_port);
	dst->ip[0] = (uint8_t)((ip>>24) & 0xFF);
	dst->ip[1] = (uint8_t)((ip>>16) & 0xFF);
	dst->ip[2] = (uint8_t)((ip>>8) & 0xFF);
	dst->ip[3] = (uint8_t)(ip & 0xFF);
}

enum network_result network_host_lookup(struct network_address* address, const char *hostname)
{
	struct addrinfo hints;
	struct addrinfo *result;
	int e;

	memory_zero(&hints, sizeof(hints));
	hints.ai_family = AF_INET;

	e = getaddrinfo(hostname, 0x0, &hints, &result);
	if(e != 0 || !result)
		return 0;

	sockaddr_to_netaddr(result->ai_addr, address);
	freeaddrinfo(result);
	address->port = 0;

	return 1;
}

struct network_socket* network_udp_create(const struct network_address* bind_address)
{
	struct sockaddr addr;
	unsigned long mode = 1;
	int broadcast = 1;

	int sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(sock < 0)
		return NULL;
	
	netaddr_to_sockaddr(bind_address, &addr);
	if(bind(sock, &addr, sizeof(addr)) != 0)
	{
		network_udp_destroy(AS_PTR(sock));
		return NULL;
	}
	
#if defined(CONF_FAMILY_WINDOWS)
	ioctlsocket(sock, FIONBIO, (unsigned long *)&mode);
#else
	ioctl(sock, FIONBIO, (unsigned long *)&mode);
#endif

	setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (const char*)&broadcast, sizeof(broadcast));
	
	return AS_PTR(sock);
}

void network_udp_destroy(struct network_socket* sock)
{
#if defined(CONF_FAMILY_WINDOWS)
	closesocket(sock);
#else
	close(FROM_PTR(sock));
#endif
}

enum network_result network_udp_send(struct network_socket* sock, const struct network_address* address, const void* data, uint num_bytes, uint *num_bytes_sent)
{
	struct sockaddr sa;
	int d;
	memory_zero(&sa, sizeof(sa));
	netaddr_to_sockaddr(address, &sa);
	d = sendto(FROM_PTR(sock), (const char*)data, num_bytes, 0, &sa, sizeof(sa));
	if(d > 0)
	{
		*num_bytes_sent = d;
		return NETWORK_RESULT_OK;
	}
	return NETWORK_RESULT_GENERAL_ERROR;
}

enum network_result network_udp_receive(struct network_socket* sock, struct network_address* address, void* data, uint max_bytes, uint *num_bytes_recv)
{
	struct sockaddr from;
	int bytes;
	socklen_t fromlen = sizeof(struct sockaddr);
	bytes = recvfrom(FROM_PTR(sock), (char*)data, max_bytes, 0, &from, &fromlen);
	if(bytes > 0)
	{
		sockaddr_to_netaddr(&from, address);
		*num_bytes_recv = bytes;
		return NETWORK_RESULT_OK;
	}
	else if(bytes == 0)
	{
		*num_bytes_recv = bytes;
		return NETWORK_RESULT_OK;
	}

	return NETWORK_RESULT_GENERAL_ERROR;
}

struct network_socket* network_tcp_create(const struct network_address* address)
{
    struct sockaddr addr;

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock < 0)
        return NULL;

    netaddr_to_sockaddr(address, &addr);
    bind(sock, &addr, sizeof(addr));

    return AS_PTR(sock);
}

void network_tcp_destroy(struct network_socket* sock)
{
	close(FROM_PTR(sock)); 
}

void network_tcp_set_non_blocking(struct network_socket* sock)
{
	unsigned long mode = 1;
#if defined(CONF_FAMILY_WINDOWS)
	ioctlsocket(sock, FIONBIO, &mode);
#else
	ioctl(FROM_PTR(sock), FIONBIO, &mode);
#endif
}

void network_tcp_set_blocking(struct network_socket* sock)
{
	unsigned long mode = 0;
#if defined(CONF_FAMILY_WINDOWS)
	ioctlsocket(sock, FIONBIO, &mode);
#else
	ioctl(FROM_PTR(sock), FIONBIO, &mode);
#endif
}

void network_tcp_listen(struct network_socket* sock, int backlog)
{
	listen(FROM_PTR(sock), backlog);
}

enum network_result network_tcp_accept(struct network_socket* listen_sock, struct network_socket* *dst_sock, struct network_address* address)
{
	int s;
	socklen_t sockaddr_len;
	struct sockaddr addr;

	sockaddr_len = sizeof(addr);

	s = accept(FROM_PTR(listen_sock), &addr, &sockaddr_len);

	if (s != -1)
	{
		sockaddr_to_netaddr(&addr, address);
		*dst_sock = AS_PTR(s);
		return NETWORK_RESULT_OK;
	}

	return NETWORK_RESULT_GENERAL_ERROR;
}

enum network_result network_tcp_connect(struct network_socket* sock, const struct network_address* address)
{
  struct sockaddr addr;

  netaddr_to_sockaddr(address, &addr);
  return connect(FROM_PTR(sock), &addr, sizeof(addr)); 
}

enum network_result network_tcp_connect_non_blocking(struct network_socket* sock, const struct network_address* address)
{
	struct sockaddr addr;
	int res;

	netaddr_to_sockaddr(address, &addr);
	network_tcp_set_non_blocking(sock);
  	res = connect(FROM_PTR(sock), &addr, sizeof(addr));
	network_tcp_set_blocking(sock);

	return res;
}

enum network_result network_tcp_send(struct network_socket* sock, const void *data, uint num_bytes, uint *num_bytes_sent)
{
	ssize_t n;
	n = send(FROM_PTR(sock), (const char*)data, num_bytes, 0);
	if(n >= 0)
	{
		*num_bytes_sent = n;
		return NETWORK_RESULT_OK;
	}

	return NETWORK_RESULT_GENERAL_ERROR;
}

enum network_result network_tcp_receive(struct network_socket* sock, void *data, uint max_bytes, uint *num_bytes_recv)
{
	int n = recv(FROM_PTR(sock), (char*)data, max_bytes, 0);
	
	if(n == 0)
	{
		*num_bytes_recv = 0;
		return NETWORK_RESULT_CONNECTION_LOST;
	}
	else if(n != -1)
	{
		*num_bytes_recv = n;
		return NETWORK_RESULT_OK;
	}

	return NETWORK_RESULT_GENERAL_ERROR;
}
