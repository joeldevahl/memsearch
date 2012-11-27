#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED

struct network_address;

void client(struct network_address* addr, int command, const char* arg);

#endif // #ifndef CLIENT_H_INCLUDED
