#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <core/allocator.h>
#include <core/assert.h>
#include <core/file.h>
#include <network/network.h>

#include "getopt.h"
#include "memsearch.h"
#include "server.h"
#include "client.h"

void print_help(struct getopt* getopt)
{
	char buffer[2048];
	printf("usage: memsearch [options]\n\n");
	printf("%s", getopt_create_help_string(getopt, buffer, 2048));
}

int main(int argc, char** argv)
{
	static const struct getopt_option opts[] =
	{
		{"help",    'h', GETOPT_OPTION_TYPE_NO_ARG,   0x0, 'h', "displays this help-message"},
		{"server",  's', GETOPT_OPTION_TYPE_OPTIONAL, 0x0, 's', "run as server"},
		{"connect", 'c', GETOPT_OPTION_TYPE_OPTIONAL, 0x0, 'c', "connect to remote memsearch server"},
		{"address", 'a', GETOPT_OPTION_TYPE_OPTIONAL, 0x0, 'a', "address to bind to if server, address to connect to if client"},
		{"port",    'p', GETOPT_OPTION_TYPE_OPTIONAL, 0x0, 'p', "port to bind to if server, port to connect to if client"},
		{"quit",    'q', GETOPT_OPTION_TYPE_OPTIONAL, 0x0, 'q', "message the server to exit"},
		{"find",    'f', GETOPT_OPTION_TYPE_OPTIONAL, 0x0, 'f', "message the server to add file"},
		{"dir",     'd', GETOPT_OPTION_TYPE_OPTIONAL, 0x0, 'd', "message the server to add dir recursivly"},
		{0}
	};
	struct getopt getopt;
	int opt;
	int mode = -1;
	int command = -1;
	const char* arg = NULL;
	const char* port = "8880";
	const char* address = "localhost";
	struct network_address addr;
	getopt_create(&getopt, argc, argv, opts);

	while((opt = getopt_parse(&getopt)) != -1)
	{
		switch(opt)
		{
			case 'h':
				print_help(&getopt);
				return 0;
			case 's':
				if(mode != -1)
				{
					printf("Cannot specify both server and client at once.\n");
					return 1;
				}
				mode = 1;
				break;
			case 'c':
				if(mode != -1)
				{
					printf("Cannot specify both server and client at once.\n");
					return 1;
				}
				mode = 2;
				break;
			case 'a':
				address = getopt.current_opt_arg;
				break;
			case 'p':
				port = getopt.current_opt_arg;
				break;
			case 'f':
			case 'd':
				arg = getopt.current_opt_arg;
				puts(getopt.current_opt_arg);
			case 'q':
				if(command != -1)
				{
					printf("Cannot specify multiple commands. Tried to specify -%c, -%c was already given\n", opt, command);
					return 1;
				}
				command = opt;
				break;
		}
	}
	if(mode == -1 || (mode == 2 && command == -1))
	{
		print_help(&getopt);
		return 0;
	}

	network_init();
	network_host_lookup(&addr, address);
	addr.port = atoi(port);
	if(mode == 1) // server
	{
		server(&addr);
	}
	else // client
	{
		client(&addr, command, arg);
	}

	return 0;
}
