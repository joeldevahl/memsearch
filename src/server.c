#include <string.h>
#include <stdio.h>

#include "server.h"
#include "memsearch.h"

#include <core/allocator.h>
#include <core/assert.h>
#include <network/network.h>

static void cut_at_newline(char* str)
{
	while(*str != '\0')
	{
		if(*str == '\n' || *str == '\r')
		{
			*str = '\0';
			return;
		}
		++str;
	}
}

static void find_callback(void* ctx, const char* file, const char* str, uint line, uint col)
{
	uint num_bytes_sent = 0;
	enum network_result res;
	char buffer[1024]; // TODO: better sized buffer or atleast a check for overflow
	struct network_socket* sock = (struct network_socket*)ctx;
	int line_len = strcspn(str, "\n\r");
	snprintf(buffer, sizeof(buffer), "%s(%d): %.*s\n\n", file, line, line_len, str);
	res = network_tcp_send(sock, buffer, strlen(buffer), &num_bytes_sent);
	ASSERT(res == NETWORK_RESULT_OK, "accept failed\n");
}

static void add_callback(void* ctx, const char* path)
{
	uint num_bytes_sent = 0;
	enum network_result res;
	char buffer[1024]; // TODO: better sized buffer or atleast a check for overflow
	struct network_socket* sock = (struct network_socket*)ctx;
	snprintf(buffer, sizeof(buffer), "%s\n", path);
	res = network_tcp_send(sock, buffer, strlen(buffer), &num_bytes_sent);
	ASSERT(res == NETWORK_RESULT_OK, "accept failed\n");
}

void server(struct network_address* addr)
{
	int done = 0;
	struct memsearch* memsearch = memsearch_create(&allocator_default);
	struct network_socket* listen = network_tcp_create(addr);
	network_tcp_listen(listen, 10);
	while(!done)
	{
		uint num_recv_total = 0;
		char buffer[1024];
		struct network_socket* sock;
		struct network_address sock_addr;
		enum network_result res;
		
		res = network_tcp_accept(listen, &sock, &sock_addr);
		if(res != NETWORK_RESULT_OK)
			continue;

		do
		{
			uint num_recv = 0;
			res = network_tcp_receive(sock, buffer, sizeof(buffer) - 1 - num_recv_total, &num_recv);
			num_recv_total += num_recv;
		} while(res == NETWORK_RESULT_OK && buffer[num_recv_total - 1] != '\n' && num_recv_total < sizeof(buffer));
		buffer[num_recv_total]= '\0';
		cut_at_newline(buffer);

		if(strncmp(buffer, "FIND ", 5) == 0)
		{
			if(strlen(buffer) > 5)
				memsearch_find(memsearch, buffer + 5, find_callback, (void*)sock);
		}
		else if(strncmp(buffer, "QUIT", 4) == 0)
		{
			done = 1;
		}
		else if(strncmp(buffer, "DIR ", 4) == 0)
		{
			const char* exts[] =
			{
				"c",
				"h",
				"lua"
			};
			if(strlen(buffer) > 4)
				memsearch_add_dir(memsearch, buffer + 4, exts, CARRAY_LENGTH(exts), add_callback, (void*)sock);
		}
		network_tcp_destroy(sock);
	}
	network_tcp_destroy(listen);
	memsearch_destroy(memsearch);
}
