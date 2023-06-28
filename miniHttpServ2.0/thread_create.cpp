#include "thread_create.h"
#include "mainFunc.h"

int createThread(int client_sock, void *do_http_request(void*))	//这里的do_http_request是一个函数指针，指向do_http_request函数
{
	int res;
	pthread_t id;
	pthread_attr_t thread_attr;
	int* pclient_sock = NULL;

	//启动线程处理http请求
	pclient_sock = (int*)malloc(sizeof(int));
	*pclient_sock = client_sock;

	res = pthread_attr_init(&thread_attr);
	if (res)
	{
		//perrorText("thread_init");
		return -1;
	}

	res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);	//设置分离模式
	if (res)
	{
		//perrorText("thread_setdetachstate");
		return -1;
	}

	res = pthread_create(&id, &thread_attr, do_http_request, pclient_sock);
	if (res)
	{
		//perrorText("detach_thread_create");
		return -1;
	}

	return 0;
}