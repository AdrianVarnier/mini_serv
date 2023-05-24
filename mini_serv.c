#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct	s_client
{
	int		id;
	char	msg[1024];
}				t_client;

t_client	clients[1024];
fd_set		write_fds, read_fds, fds;
int			fd_max = 0, id = 0;
char		buffer_read[120000], buffer_write[120000];

static void ft_error(char *str)
{
	if (str != NULL)
		write(2, str, strlen(str));
	else
		write(2, "Fatal error", strlen("Fatal error"));
	write(2, "\n", 1);
	exit(1);
}

static void ft_send(int sender)
{
	for (int fd = 0; fd <= fd_max; fd++)
		if (FD_ISSET(fd, &write_fds) && fd != sender)
			send(fd, buffer_write, strlen(buffer_write), 0);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		ft_error("Wrong number of arguments");

	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		ft_error(NULL);
	
	FD_ZERO(&fds);
	bzero(&clients, sizeof(clients));
	fd_max = server_fd;
	FD_SET(server_fd, &fds);

	struct sockaddr_in	server_addr;
	socklen_t			server_len;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(2130706433);
	server_addr.sin_port = htons(atoi(argv[1]));

	if (bind(server_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		ft_error(NULL);
	if (listen(server_fd, 10) < 0)
		ft_error(NULL);

	while (1)
	{
		read_fds = write_fds = fds;
		
		if (select(fd_max + 1, &read_fds, &write_fds, NULL, NULL) < 0)
			continue;

		for (int fd = 0; fd <= fd_max; fd++)
		{
			if (FD_ISSET(fd, &read_fds) && fd == server_fd)
			{
				int new_fd = accept(server_fd, (struct sockaddr *)&server_addr, &server_len);
				if (new_fd < 0)
					continue;
				if (fd_max < new_fd)
					fd_max = new_fd;
				clients[new_fd].id = id++;
				FD_SET(new_fd, &fds);
				sprintf(buffer_write, "server: client %d just arrived\n", clients[new_fd].id);
				ft_send(new_fd);
			}

			if (FD_ISSET(fd, &read_fds) && fd != server_fd)
			{
				int bytes = recv(fd, buffer_read, 65536, 0);

				if (bytes <= 0)
				{
					sprintf(buffer_write, "server: client %d just left\n", clients[fd].id);
					ft_send(fd);
					FD_CLR(fd, &fds);
					close(fd);
					break;
				}
				else
				{
					for (int i = 0, j = strlen(clients[fd].msg); i < bytes; i++, j++)
					{
						clients[fd].msg[j] = buffer_read[i];
						if (clients[fd].msg[j] == '\n')
						{
							clients[fd].msg[j] = '\0';
							sprintf(buffer_write, "client %d: %s\n", clients[fd].id, clients[fd].msg);
							ft_send(fd);
							bzero(&clients[fd].msg, strlen(clients[fd].msg));
							j--;
						}
					}
					break ;
				}
			}
		}
	}
}