#include <stdio.h> // sprintf
#include <string.h> // strlen
#include <stdlib.h> // exit
#include <netinet/ip.h> //sockaddr_in, htonl, htons

typedef struct	server_s
{
	int					fd;
	struct sockaddr_in	addr;
}				server_t;


void	errmsg_exit(char *s, int i)
{
	sprintf(stderr, "%s", s);
	exit(1);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		errmsg_exit("Wrong number of arguments\n", 1);

	server_t	server;
	server.addr.sin_family = AF_INET;
	server.addr.sin_addr.s_addr = htonl(2130706433);
	server.addr.sin_port = htons(atoi(argv[1]));
	if (server.fd = socket(AF_INET, SOCK_STREAM, 0) < 0)
		errmsg_exit("Fatal error\n", 1);
	if (bind(server.fd, (struct sockaddr *)&server.addr, sizeof(server.addr)) < 0)
		errmsg_exit("Fatal error\n", 1);
	if (listen(server.fd, 128) < 0)
		errmsg_exit("Fatal error\n", 1);

	return (0);
}