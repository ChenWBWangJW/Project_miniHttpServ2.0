#include "create_connect.h"
#include "mainFunc.h"
#include "thread_create.h"

int main(void)
{
	//1.创建套接字并绑定地址
	u_short port = SERV_PORT;
	int sock = -1;
	if ((sock = startUp(&port)) == -1)
	{
		perror("create socket");
		exit(-1);
	}

	//2.开始循环接收客户端连接
	int done = 1;
	while(done)
	{
		//2.1.接收客户端连接
		int client_sock = -1;
		if ((client_sock = clientStartup(&sock)) == -1)
		{
			//perrorText("accept");
			perror("accept");
			exit(-1);
		}
		
		//do_http_request((void*)&client_sock);
		
		//2.2.创建线程处理http请求
		int res = -1;
		if ((res = createThread(client_sock, do_http_request)) == -1)
		{
			//perrorText("createThread");
			perror("createThread");
			exit(-1);
		}
		
	}
	pthread_exit(NULL);
	close(sock);
	return 0;
}

