#include "mainFunc.h"
#include "create_connect.h"

static int debug = 1;	//设置一个开关，项目实战经常怎么玩

void* do_http_request(void* pclient_sock)
{
	int len = 0;
	char buf[SIZE_OF_BUF];
	char method[SIZE_OF_METHOD];
	char url[SIZE_OF_URL];
	char http_verson[SIZE_OF_HTTP_VERSON];
	char path[SIZE_OF_PATH];
	int client_sock = *(int*)pclient_sock;
	char *query_string = NULL;
	int cgi = 0;		//需要执行cgi程序是设置ture

	struct stat st;

	/* 读取客户端发送http请求 */


	//1.读取请求行

	len = get_line(client_sock, buf, sizeof(buf));

	if (len > 0)	//读到请求行GET
	{
		int i = 0, j = 0;

		while (!isspace(buf[j]) && (i < sizeof(method) - 1))
		{
			method[i] = buf[j];
			i++;
			j++;
		}

		method[i] = '\0';
		if (debug) printf("Request method: %s\n", method);

		if ((strncasecmp(method, "GET", i) == 0) || (strncasecmp(method, "POST", i) == 0))	//比较后确认是GTE方法
		{
			if (debug) printf("Method = %s\n", method);

			//获取url
			while (isspace(buf[j++])); //跳过空格
			i = 0;

			while (!isspace(buf[j]) && (i < sizeof(url) - 1))
			{
				url[i] = buf[j];
				i++;
				j++;
			}

			url[i] = '\0';

			if (debug) printf("URL = %s\n", url);

			//获取http版本
			while (isspace(buf[j++])); //跳过空格
			i = 0;

			while (!isspace(buf[j - 1]) && (i < sizeof(http_verson) - 1))
			{
				http_verson[i] = buf[j - 1];
				i++;
				j++;
			}

			http_verson[i] = '\0';

			if (debug) printf("Version = %s\n", http_verson);

			//继续读取http头部GTET后剩余行
			if ((strncasecmp(method, "GET", strlen(method)) == 0))
			{
				do
				{
					len = get_line(client_sock, buf, sizeof(buf));
					if (debug) printf("%s\n", buf);
				} while (len > 0);
			}

			//获取url地址？后的参数
			
			//处理get请求时的cgi参数
			if ((strncasecmp(method, "GET", strlen(method)) == 0))
			{
				query_string = url;
				while ((*query_string != '?') && (*query_string != '\0'))
				{
					query_string++;
				}
				if (*query_string == '?')
				{
					cgi = 1;
					*query_string = '\0';
					query_string++;
				}
			}
			
			//处理url中的问号
			/*
			pos = strchr(url, '?');	//单个字符对比替换方法，返回地址或0；
			if (*pos == '?')
			{
				cgi = 1;
				*pos = '\0';
				pos++;
			}
			*/

			printf("real url: %s\n", url);
			//路径映射，定位服务器本地的html本地文件
			//格式化路径并将用户所需路径写入缓冲path内
			sprintf(path, "./html_docs/%s", url);
			if (debug) printf("path: %s\n", path);


			//执行http 响应客户端

			//判断文件是否存在，响应200 OK，同时发送响应的html文件，不存在响应404NOT FOUND
			if (stat(path, &st) == -1)	//成功返回0，文件不存在或出错，返回-1
			{
				fprintf(stderr, "stat %s failed. reason: %s\n", path, strerror(errno));
				not_found(client_sock);
			}
			else 
			{	//文件存在
				if (S_ISDIR(st.st_mode))
				{
					strcat(path, "/index.html");
					
				}

				//判断文件是否可执行，如果可执行，设置cgi为真
				if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
				{
					cgi = 1;
					if (debug) printf("cgi = %d\n", cgi);
				}
				
				//如果cgi为真，执行cgi程序，否则直接响应客户端
				if (!cgi)
				{
					do_http_response(client_sock, path);
				}
				else
				{
					execute_cgi(client_sock, path, method, query_string);
				}
				//do_http_response(client_sock, path);
			}
		}
		else	//非GET或POST请求，读取所有HTTP头部并响应客户端501:Method Not Implemented
		{
			fprintf(stderr, "warning! other request[%s]\n", method);
			do
			{
				len = get_line(client_sock, buf, sizeof(buf));
				if (debug) printf("read: %s\n", buf);

			} while (len > 0);

			unimplemented(client_sock);	//请求未实现,500:unimplement
		}
	}
	else	//请求格式有问题，出错处理，响应客户端400:Bad Request
	{
		bad_request(client_sock);
	}
	close(client_sock);
	if (pclient_sock) free(pclient_sock);	//释放动态分配的内存
	return NULL;
}


void do_http_response(int client_sock, const char* path)
{
	int ret = 0;
	FILE* resource = NULL;
	resource = fopen(path, "r");

	if (resource == NULL)
	{
		not_found(client_sock);
		return;
	}

	//1.发送http头部
	ret = headers(client_sock, resource);

	//2.发送httpbody部分
	if (!ret)
	{
		cat(client_sock, resource);
	}

	fclose(resource);
}


/*************************************
 *返回关于响应文件信息的http头部
 *输入：
 *		client_sock - 客户端的sock句柄
 *		resourse	- 文件的句柄
 *返回值：成功返回0，失败返回-1
**************************************/
int headers(int client_sock, FILE* resource)
{
	struct stat st;
	int fileid = 0;
	char tmp[64];
	char buf[1024] = { 0 };
	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	strcat(buf, "Server: Chen Server\r\n");
	strcat(buf, "Content-Type: text/html\r\n");
	strcat(buf, "Connection: Close\r\n");

	fileid = fileno(resource);

	if (fstat(fileid, &st) == -1)	//属于服务器内部出错
	{
		inner_error(client_sock);
		return -1;
	}

	snprintf(tmp, 64, "Content_Length: %ld\r\n\r\n", st.st_size);
	strcat(buf, tmp);

	if (debug) fprintf(stdout, "......Reply......\nHeader: %s\n", buf);

	if (write(client_sock, buf, strlen(buf)) < 0)
	{
		fprintf(stderr, "send failed. data: %s, reason: %s\n", buf, strerror(errno));
		return -1;
	}
	return 0;

}


/*******************************
 *说明：实现将html文件的内容按行
		读取并发送给客户端
********************************/
void cat(int client_sock, FILE* resource)
{
	char buf[1024];

	fgets(buf, sizeof(buf), resource);

	while (!feof(resource))		//feof为文件结束代码
	{
		int len = write(client_sock, buf, strlen(buf));

		if (len < 0)		//发送body的过程中出现问题,1.重试
		{
			fprintf(stderr, "send body error. reason: %s\n", strerror(errno));
			break;
		}

		if (debug) fprintf(stdout, "%s", buf);

		fgets(buf, sizeof(buf), resource);


	}}


//return -1 表示读取出错，等于0表示读到空行，大于0表示读取字符数
int get_line(int sock, char* buf, int size)	//单个字符读取
{

	int count = 0;	//字符串读取计数
	char ch = '\0';	//ch字符作为读缓冲，正常字符写入buf，回车则停止并填入字符串结束符号'\0'
	int len = 0;	//读取成功为1，错误为0

	while (count < size - 1 && ch != '\n')
	{
		len = recv(sock, &ch, 1, 0);

		if (len == 1)
		{
			if (ch == '\r')	//'\r'回车符忽略，continue跳出if大循环继续下一循环
			{
				continue;
			}
			else if (ch == '\n')	//'\n'换行符break结束循环
			{
				//buf[count] = '\0';
				break;
			}

			//处理一般字符
			buf[count] = ch;
			count++;

		}
		else if (len == -1)	//读取出错
		{
			perror("read failed\n");
			break;
		}
		else	//read返回0，意指客户端关闭sock连接
		{
			fprintf(stderr, "client close\n");
			count = -1;
			break;
		}
	}

	if (count >= 0)
	{
		buf[count] = '\0';
	}

	return count;
}


/*******************************
 *说明：添加执行cgi脚本函数，通过
 *		创建子进程来执行cgi脚本并
 *		通过管道进行进程间通信，
 *		父进程读取子进程的输出并
 *		发送给客户端，子进程读取
 *		客户端的输入并传递给cgi
*		脚本
********************************/
void execute_cgi(int client, const char* path, const char* method, const char* query_string)
{
	char buf[SIZE_OF_BUF];		//缓冲区
	int cgi_output[2];			//管道输出，存放两个文件描述符，0读，1写
	int cgi_input[2];			//管道输入，存放两个文件描述符，0读，1写
	pid_t pid;
	int status, i;
	char c;
	int numchars = 1;			//读取字符数
	int content_length = -1;	//HTTP的content_length


	//1.只处理POST方法
	buf[0] = 'A';
	buf[1] = '\0';

	if (strcasecmp(method, "POST") == 0)//POST方法，需要获取content_length
	{
		numchars = get_line(client, buf, sizeof(buf));		//读取的为头部后剩余数据，且只能读取一次后sock文件内自动清空

		/*
		* POST请求，需要获取content_length，因为POST请求的body部分长度是由content_length指定的
		*/
		while (numchars > 0 && strcmp("\n", buf))
		{
			buf[15] = '\0';
			if ((strcasecmp(buf, "Content-Length:") == 0))
			{
				content_length = atoi(&buf[16]);	//获取content_length的值,内存第17位开始为长度值，转换成整数
			}
			numchars = get_line(client, buf, sizeof(buf));
		}
		if (content_length == -1)
		{
			bad_request(client);
			return;
		}
	}

	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	printf("......Reply......\nHeader: %s\n", buf);
	send(client, buf, strlen(buf), 0);
	//2.创建两个管道，用于两个进程通信
	if (pipe(cgi_output) < 0)
	{
		cannot_execute(client);
		return;
	}

	if (pipe(cgi_input) < 0)
	{
		cannot_execute(client);
		return;
	}
	//		fork后管道都复制了一份
	//		子进程关闭2个无用管道端口
	//		x<---------------------->1	output
	//		0<---------------------->x	input

	//		父进程关闭2个无用管道端口
	//		0<---------------------->x	output
	//		x<---------------------->1	input
	//		此时父子进程都有2个管道端口，一个用于读，一个用于写，可以进行通信

	//fork进程，子进程用于执行CGI脚本
	//父进程用于读取CGI脚本的输出或子进程处理的回复数据
	if ((pid = fork()) < 0)
	{
		cannot_execute(client);
		return;
	}
	if (pid == 0)		//子进程，CGI执行
	{
		char meth_env[255];
		char query_env[255];
		char length_env[255];

		//单向管道，子进程关闭无用端口
		//子进程将标准输出重定向到cgi_output的管道写端
		dup2(cgi_output[1], 1);
		//子进程将标准输入重定向到cgi_input的管道读端
		dup2(cgi_input[0], 0);

		//关闭无用管道端口
		close(cgi_output[0]);
		close(cgi_input[1]);
		//cgi脚本环境变量设置
		sprintf(meth_env, "REQUEST_METHOD=%s", method);
		putenv(meth_env);
		if (strcasecmp(method, "GET") == 0)
		{
			sprintf(query_env, "QUERY_STRING=%s", query_string);
			putenv(query_env);
		}
		else		//POST
		{
			sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
			putenv(length_env);
		}

		//替换子进程，执行CGI脚本
		execl(path, path, NULL);

		exit(0);
	}
	else		//父进程
	{
		//父进程关闭无用管道端口
		close(cgi_output[1]);
		close(cgi_input[0]);

		//如果是POST方法，父进程需要读取content_length字节的数据，作为CGI脚本的输入
		if (strcasecmp(method, "POST") == 0)
		{
			for (i = 0; i < content_length; i++)
			{
				recv(client, &c, 1, 0);
				write(cgi_input[1], &c, 1);
			}
		}

		//读取cgi脚本返回数据，发送给客户端
		while (read(cgi_output[0], &c, 1) > 0)
		{
			send(client, &c, 1, 0);
		}

		//关闭管道
		close(cgi_output[0]);
		close(cgi_input[1]);

		//等待子进程结束
		waitpid(pid, &status, 0);
	}
}
