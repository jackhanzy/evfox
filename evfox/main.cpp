#include <iostream>
#include <event.h>

int main(int argc, char* argv[])
{
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
	struct event_base* base = event_base_new();
	WSACleanup();
	return 0;

}