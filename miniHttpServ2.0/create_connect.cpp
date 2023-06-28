#include "mainFunc.h"
#include "create_connect.h"

int startUp(u_short* port)
{
	int sock = -1;
	int ret;
	SAIN server_addr;
	int on = 1;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("create socket");
	}

	//bzero(&server_addr, sizeof(server_addr));
	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(*port);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   //可改为SERV_IP

	ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));
	if (ret == -1)
	{
		perror("setsockopt");
		exit(-1);
	}

	ret = bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if (ret == -1)
	{
		perror("bind");
		exit(-1);
	}

	//提供随机端口号
	if (*port == 0)
	{
		socklen_t namelen = sizeof(server_addr);
		if(getsockname(sock, (struct sockaddr*)&server_addr, &namelen) == -1)
		{
			perror("getsockname");
			exit(-1);
		}
		*port = ntohs(server_addr.sin_port);
	}

	ret = listen(sock, 2048);
	if (ret == -1)
	{
		perror("listen");
		exit(-1);
	}

	printf("wait for client......port:%d\n", server_addr.sin_port);
	return sock;
}

int clientStartup(int* sock)
{
	struct sockaddr_in client;
	int client_sock;
	char client_ip[INET_ADDRSTRLEN];

	socklen_t client_addr_len;
	client_addr_len = sizeof(client);

	client_sock = accept(*sock, (struct sockaddr*)&client, &client_addr_len);

	printf("Client IP :%s\tPort :%d\n", inet_ntop(AF_INET, &client.sin_addr.s_addr, client_ip, sizeof(client_ip)), ntohs(client.sin_port));

	return client_sock;
}