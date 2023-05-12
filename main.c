#include <stdio.h> //sprintf
#include <unistd.h> // write
#include <string.h> // strlen
#include <stdlib.h> // exit
#include <netinet/ip.h> // sockaddr_in, htonl, htons
#include <sys/select.h> // select

typedef struct	client_s
{
	int				fd;
	int				id;
	struct client_s	*next;
}				client_t;

typedef struct	server_s
{
	int					fd;
	int					id;
	fd_set				set;
	fd_set				rset;
	fd_set				wset;
	struct sockaddr_in	addr;
	client_t*			clients;
}				server_t;

void	errmsg_exit(char *s, int i)
{
	write(2, s, strlen(s));
	exit(1);
}

int	greatest_fd(server_t server)
{
	int fd = server.fd;
	while (server.clients != NULL)
	{
		if (server.clients->fd > fd)
			fd = server.clients->fd;
		server.clients = server.clients->next;
	}
	return (fd);
}

void	init_server(server_t *server, char *port)
{
	server->id = 0;
	server->clients = NULL;
	server->addr.sin_family = AF_INET;
	server->addr.sin_addr.s_addr = htonl(2130706433);
	server->addr.sin_port = htons(atoi(port));
	server->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server->fd < 0)
		errmsg_exit("Fatal error\n", 1);
	if (bind(server->fd, (struct sockaddr *)&server->addr, sizeof(server->addr)) < 0)
		errmsg_exit("Fatal error\n", 1);
	if (listen(server->fd, 128) < 0)
		errmsg_exit("Fatal error\n", 1);
	FD_ZERO(&server->set);
	FD_SET(server->fd, &server->set);
}

size_t	ilength(int n)
{
	size_t size = 0;
	if (n == 0)
		return (1);
	while (n > 0)
	{
		n = n/10;
		size++;
	}
	return (size);
}

void	add_client(server_t *server)
{
	struct sockaddr_in	addr;
	socklen_t			len;

	int	fd = accept(server->fd, (struct sockaddr *)&addr, &len);
	if (fd < 0)
		errmsg_exit("Fatal error\n", 1);
	FD_SET(fd, &server->set);
	client_t*	new_client = calloc(sizeof(client_t), 1);
	if (new_client == NULL)
		errmsg_exit("Fatal error\n", 1);
	new_client->fd = fd;
	new_client->id = server->id++;
	new_client->next = NULL;
	if (server->clients == NULL)
		server->clients = new_client;
	else
	{
		client_t* index = server->clients;
		while (index->next != NULL)
			index = index->next;
		index->next = new_client;
	}
	for (client_t* index = server->clients; index != NULL; index = index->next)
	{
		if (index->fd != new_client->fd && FD_ISSET(index->fd, &server->wset) != 0)
		{
			size_t len = strlen("server: client  just arrived\n") + ilength(index->id);
			char msg[len];
			sprintf(msg, "server: client %d just arrived\n", new_client->id);
			if (send(index->fd, msg, strlen(msg), 0) < 0)
				errmsg_exit("Fatal error\n", 1);
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
		errmsg_exit("Wrong number of arguments\n", 1);

	server_t	server;
	init_server(&server, argv[1]);

	while(1)
	{
		server.rset = server.set;
		server.wset = server.set;
		int n = select(greatest_fd(server) + 1, &server.rset, &server.wset, NULL, NULL);
		if (n < 0)
			errmsg_exit("Fatal error\n", 1);
		if (n == 0)
			continue;
		for (int fd = 0; fd < greatest_fd(server) + 1; fd++)
		{
			if (FD_ISSET(fd, &server.rset) != 0)
			{
				if (fd == server.fd)
					add_client(&server);
				else
				{
				}
			}
		}
	}

	return (0);
}