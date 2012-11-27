#ifndef NETWORK_NETWORK_H
#define NETWORK_NETWORK_H

#include <core/types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum network_result
{
	NETWORK_RESULT_OK = 0,
	NETWORK_RESULT_CONNECTION_LOST,
	NETWORK_RESULT_GENERAL_ERROR,
};

struct network_address
{
	uint8_t ip[4];
	uint16_t port;
};

void network_init();

enum network_result network_host_lookup(struct network_address* address, const char *hostname);

struct network_socket* network_udp_create(const struct network_address* bind_address);
void network_udp_destroy(struct network_socket* sock);
enum network_result network_udp_send(struct network_socket* sock, const struct network_address* address, const void* data, uint num_bytes, uint *num_bytes_sent);
enum network_result network_udp_receive(struct network_socket* sock, struct network_address* address, void* data, uint max_bytes, uint *num_bytes_recv);

struct network_socket* network_tcp_create(const struct network_address* address);
void network_tcp_destroy(struct network_socket* sock);
void network_tcp_set_non_blocking(struct network_socket* sock);
void network_tcp_set_blocking(struct network_socket* sock);
void network_tcp_listen(struct network_socket* sock, int32_t backlog);
enum network_result network_tcp_accept(struct network_socket* listen_sock, struct network_socket** dst_sock, struct network_address* address);
enum network_result network_tcp_connect(struct network_socket* sock, const struct network_address* address);
enum network_result network_tcp_connect_non_blocking(struct network_socket* sock, const struct network_address* address);
enum network_result network_tcp_send(struct network_socket* sock, const void *data, uint num_bytes, uint *num_bytes_sent);
enum network_result network_tcp_receive(struct network_socket* sock, void *data, uint max_bytes, uint *num_bytes_recv);

#ifdef __cplusplus
}
#endif

#endif // NETWORK_NETWORK_H
