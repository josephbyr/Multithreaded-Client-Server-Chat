#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100
#define NAME_SIZE 20

void *recv_msg(void *arg);
void error_handling(char *msg);

char msg[BUF_SIZE];

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in serv_addr;
	pthread_t  rcv_thread;
	void *thread_return;
	char msg_out[NAME_SIZE + BUF_SIZE];
	char name[NAME_SIZE];

	if (argc != 4)
	{
		printf("Usage : %s <IP> <port> <name>\n", argv[0]);
		exit(1);
	}

	strcpy(name, argv[3]);
	sock = socket(PF_INET, SOCK_STREAM, 0);

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
		error_handling("connect() error");

	pthread_create(&rcv_thread, NULL, recv_msg, (void *)&sock);

	while (fgets(msg, BUF_SIZE, stdin) > 0)
	{
		if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n"))
		{
			close(sock);
			exit(0);
		}
		else if (!strcmp(msg, "cls\n")) {
			system("clear");
		}
		else {
			strcpy(msg_out, name);
			strcat(msg_out, ": ");
			strcat(msg_out, msg);
			write(sock, msg_out, strlen(msg_out));
		}
	}

	pthread_join(rcv_thread, &thread_return);
	close(sock);
	return 0;
}

void *recv_msg(void *arg)
{
	int sock = *((int *)arg);
	char msg_in[NAME_SIZE + BUF_SIZE];
	int str_len;
	while (1)
	{
		str_len = read(sock, msg_in, NAME_SIZE + BUF_SIZE - 1);
		if (str_len < 0)
		{
			error_handling("Error receiving data");
		}
		else
		{
			msg_in[str_len] = 0;
			fputs(msg_in, stdout);
		}
	}
}

void error_handling(char *msg)
{
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}
