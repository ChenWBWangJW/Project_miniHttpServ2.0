#include "thread_create.h"
#include "mainFunc.h"

int createThread(int client_sock, void *do_http_request(void*))	//�����do_http_request��һ������ָ�룬ָ��do_http_request����
{
	int res;
	pthread_t id;
	pthread_attr_t thread_attr;
	int* pclient_sock = NULL;

	//�����̴߳���http����
	pclient_sock = (int*)malloc(sizeof(int));
	*pclient_sock = client_sock;

	res = pthread_attr_init(&thread_attr);
	if (res)
	{
		//perrorText("thread_init");
		return -1;
	}

	res = pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);	//���÷���ģʽ
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