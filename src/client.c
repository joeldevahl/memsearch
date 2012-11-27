#include <string.h>
#include <stdio.h>

#include <network/network.h>

#include "client.h"
/*
static void strip_newline(char* str)
{
	char* dst = str;
	char* src = str;
	while(*src != '\0')
	{
		if(*src != '\n' && *src != '\r')
		{
			*dst = *src;
			++dst;
		}
		++src;
	}
}

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
*/
static enum network_result network_tcp_send_all(struct network_socket* sock, void* data, uint num_bytes)
{
	enum network_result res;
	uint num_bytes_sent_total = 0;
	do
	{
		uint num_bytes_sent = 0;
		res = network_tcp_send(sock, data, num_bytes, &num_bytes_sent);
		num_bytes_sent_total += num_bytes_sent;
	} while(res == NETWORK_RESULT_OK && num_bytes_sent_total < num_bytes);
	return res;
}

void client(struct network_address* addr, int command, const char* arg)
{
	char buffer[1024] = {0};
	size_t data_len = 0;
	struct network_socket* sock = network_tcp_create(addr);
	enum network_result res;
	network_tcp_connect(sock, addr);

	switch(command)
	{
		case 'q':
			strncpy(buffer, "QUIT\n\n", sizeof(buffer));
			break;
		case 'd':
			snprintf(buffer, sizeof(buffer), "DIR %s\n\n", arg);
			break;
		case 'f':
			snprintf(buffer, sizeof(buffer), "FIND %s\n\n", arg);
			break;
	}

	data_len = strlen(buffer);
	if(data_len)
	{
		network_tcp_send_all(sock, buffer, data_len);
		do
		{
			uint num_recv = 0;
			res = network_tcp_receive(sock, buffer, sizeof(buffer)-1, &num_recv);
			buffer[num_recv]= '\0';
			puts(buffer);
		} while(res == NETWORK_RESULT_OK);
	}
	network_tcp_destroy(sock);
}
