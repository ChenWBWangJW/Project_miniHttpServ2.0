#include "mainFunc.h"
#include "create_connect.h"

static int debug = 1;	//����һ�����أ���Ŀʵս������ô��

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
	int cgi = 0;		//��Ҫִ��cgi����������ture

	struct stat st;

	/* ��ȡ�ͻ��˷���http���� */


	//1.��ȡ������

	len = get_line(client_sock, buf, sizeof(buf));

	if (len > 0)	//����������GET
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

		if ((strncasecmp(method, "GET", i) == 0) || (strncasecmp(method, "POST", i) == 0))	//�ȽϺ�ȷ����GTE����
		{
			if (debug) printf("Method = %s\n", method);

			//��ȡurl
			while (isspace(buf[j++])); //�����ո�
			i = 0;

			while (!isspace(buf[j]) && (i < sizeof(url) - 1))
			{
				url[i] = buf[j];
				i++;
				j++;
			}

			url[i] = '\0';

			if (debug) printf("URL = %s\n", url);

			//��ȡhttp�汾
			while (isspace(buf[j++])); //�����ո�
			i = 0;

			while (!isspace(buf[j - 1]) && (i < sizeof(http_verson) - 1))
			{
				http_verson[i] = buf[j - 1];
				i++;
				j++;
			}

			http_verson[i] = '\0';

			if (debug) printf("Version = %s\n", http_verson);

			//������ȡhttpͷ��GTET��ʣ����
			if ((strncasecmp(method, "GET", strlen(method)) == 0))
			{
				do
				{
					len = get_line(client_sock, buf, sizeof(buf));
					if (debug) printf("%s\n", buf);
				} while (len > 0);
			}

			//��ȡurl��ַ����Ĳ���
			
			//����get����ʱ��cgi����
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
			
			//����url�е��ʺ�
			/*
			pos = strchr(url, '?');	//�����ַ��Ա��滻���������ص�ַ��0��
			if (*pos == '?')
			{
				cgi = 1;
				*pos = '\0';
				pos++;
			}
			*/

			printf("real url: %s\n", url);
			//·��ӳ�䣬��λ���������ص�html�����ļ�
			//��ʽ��·�������û�����·��д�뻺��path��
			sprintf(path, "./html_docs/%s", url);
			if (debug) printf("path: %s\n", path);


			//ִ��http ��Ӧ�ͻ���

			//�ж��ļ��Ƿ���ڣ���Ӧ200 OK��ͬʱ������Ӧ��html�ļ�����������Ӧ404NOT FOUND
			if (stat(path, &st) == -1)	//�ɹ�����0���ļ������ڻ��������-1
			{
				fprintf(stderr, "stat %s failed. reason: %s\n", path, strerror(errno));
				not_found(client_sock);
			}
			else 
			{	//�ļ�����
				if (S_ISDIR(st.st_mode))
				{
					strcat(path, "/index.html");
					
				}

				//�ж��ļ��Ƿ��ִ�У������ִ�У�����cgiΪ��
				if ((st.st_mode & S_IXUSR) || (st.st_mode & S_IXGRP) || (st.st_mode & S_IXOTH))
				{
					cgi = 1;
					if (debug) printf("cgi = %d\n", cgi);
				}
				
				//���cgiΪ�棬ִ��cgi���򣬷���ֱ����Ӧ�ͻ���
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
		else	//��GET��POST���󣬶�ȡ����HTTPͷ������Ӧ�ͻ���501:Method Not Implemented
		{
			fprintf(stderr, "warning! other request[%s]\n", method);
			do
			{
				len = get_line(client_sock, buf, sizeof(buf));
				if (debug) printf("read: %s\n", buf);

			} while (len > 0);

			unimplemented(client_sock);	//����δʵ��,500:unimplement
		}
	}
	else	//�����ʽ�����⣬��������Ӧ�ͻ���400:Bad Request
	{
		bad_request(client_sock);
	}
	close(client_sock);
	if (pclient_sock) free(pclient_sock);	//�ͷŶ�̬������ڴ�
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

	//1.����httpͷ��
	ret = headers(client_sock, resource);

	//2.����httpbody����
	if (!ret)
	{
		cat(client_sock, resource);
	}

	fclose(resource);
}


/*************************************
 *���ع�����Ӧ�ļ���Ϣ��httpͷ��
 *���룺
 *		client_sock - �ͻ��˵�sock���
 *		resourse	- �ļ��ľ��
 *����ֵ���ɹ�����0��ʧ�ܷ���-1
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

	if (fstat(fileid, &st) == -1)	//���ڷ������ڲ�����
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
 *˵����ʵ�ֽ�html�ļ������ݰ���
		��ȡ�����͸��ͻ���
********************************/
void cat(int client_sock, FILE* resource)
{
	char buf[1024];

	fgets(buf, sizeof(buf), resource);

	while (!feof(resource))		//feofΪ�ļ���������
	{
		int len = write(client_sock, buf, strlen(buf));

		if (len < 0)		//����body�Ĺ����г�������,1.����
		{
			fprintf(stderr, "send body error. reason: %s\n", strerror(errno));
			break;
		}

		if (debug) fprintf(stdout, "%s", buf);

		fgets(buf, sizeof(buf), resource);


	}}


//return -1 ��ʾ��ȡ��������0��ʾ�������У�����0��ʾ��ȡ�ַ���
int get_line(int sock, char* buf, int size)	//�����ַ���ȡ
{

	int count = 0;	//�ַ�����ȡ����
	char ch = '\0';	//ch�ַ���Ϊ�����壬�����ַ�д��buf���س���ֹͣ�������ַ�����������'\0'
	int len = 0;	//��ȡ�ɹ�Ϊ1������Ϊ0

	while (count < size - 1 && ch != '\n')
	{
		len = recv(sock, &ch, 1, 0);

		if (len == 1)
		{
			if (ch == '\r')	//'\r'�س������ԣ�continue����if��ѭ��������һѭ��
			{
				continue;
			}
			else if (ch == '\n')	//'\n'���з�break����ѭ��
			{
				//buf[count] = '\0';
				break;
			}

			//����һ���ַ�
			buf[count] = ch;
			count++;

		}
		else if (len == -1)	//��ȡ����
		{
			perror("read failed\n");
			break;
		}
		else	//read����0����ָ�ͻ��˹ر�sock����
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
 *˵�������ִ��cgi�ű�������ͨ��
 *		�����ӽ�����ִ��cgi�ű���
 *		ͨ���ܵ����н��̼�ͨ�ţ�
 *		�����̶�ȡ�ӽ��̵������
 *		���͸��ͻ��ˣ��ӽ��̶�ȡ
 *		�ͻ��˵����벢���ݸ�cgi
*		�ű�
********************************/
void execute_cgi(int client, const char* path, const char* method, const char* query_string)
{
	char buf[SIZE_OF_BUF];		//������
	int cgi_output[2];			//�ܵ��������������ļ���������0����1д
	int cgi_input[2];			//�ܵ����룬��������ļ���������0����1д
	pid_t pid;
	int status, i;
	char c;
	int numchars = 1;			//��ȡ�ַ���
	int content_length = -1;	//HTTP��content_length


	//1.ֻ����POST����
	buf[0] = 'A';
	buf[1] = '\0';

	if (strcasecmp(method, "POST") == 0)//POST��������Ҫ��ȡcontent_length
	{
		numchars = get_line(client, buf, sizeof(buf));		//��ȡ��Ϊͷ����ʣ�����ݣ���ֻ�ܶ�ȡһ�κ�sock�ļ����Զ����

		/*
		* POST������Ҫ��ȡcontent_length����ΪPOST�����body���ֳ�������content_lengthָ����
		*/
		while (numchars > 0 && strcmp("\n", buf))
		{
			buf[15] = '\0';
			if ((strcasecmp(buf, "Content-Length:") == 0))
			{
				content_length = atoi(&buf[16]);	//��ȡcontent_length��ֵ,�ڴ��17λ��ʼΪ����ֵ��ת��������
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
	//2.���������ܵ���������������ͨ��
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
	//		fork��ܵ���������һ��
	//		�ӽ��̹ر�2�����ùܵ��˿�
	//		x<---------------------->1	output
	//		0<---------------------->x	input

	//		�����̹ر�2�����ùܵ��˿�
	//		0<---------------------->x	output
	//		x<---------------------->1	input
	//		��ʱ���ӽ��̶���2���ܵ��˿ڣ�һ�����ڶ���һ������д�����Խ���ͨ��

	//fork���̣��ӽ�������ִ��CGI�ű�
	//���������ڶ�ȡCGI�ű���������ӽ��̴���Ļظ�����
	if ((pid = fork()) < 0)
	{
		cannot_execute(client);
		return;
	}
	if (pid == 0)		//�ӽ��̣�CGIִ��
	{
		char meth_env[255];
		char query_env[255];
		char length_env[255];

		//����ܵ����ӽ��̹ر����ö˿�
		//�ӽ��̽���׼����ض���cgi_output�Ĺܵ�д��
		dup2(cgi_output[1], 1);
		//�ӽ��̽���׼�����ض���cgi_input�Ĺܵ�����
		dup2(cgi_input[0], 0);

		//�ر����ùܵ��˿�
		close(cgi_output[0]);
		close(cgi_input[1]);
		//cgi�ű�������������
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

		//�滻�ӽ��̣�ִ��CGI�ű�
		execl(path, path, NULL);

		exit(0);
	}
	else		//������
	{
		//�����̹ر����ùܵ��˿�
		close(cgi_output[1]);
		close(cgi_input[0]);

		//�����POST��������������Ҫ��ȡcontent_length�ֽڵ����ݣ���ΪCGI�ű�������
		if (strcasecmp(method, "POST") == 0)
		{
			for (i = 0; i < content_length; i++)
			{
				recv(client, &c, 1, 0);
				write(cgi_input[1], &c, 1);
			}
		}

		//��ȡcgi�ű��������ݣ����͸��ͻ���
		while (read(cgi_output[0], &c, 1) > 0)
		{
			send(client, &c, 1, 0);
		}

		//�رչܵ�
		close(cgi_output[0]);
		close(cgi_input[1]);

		//�ȴ��ӽ��̽���
		waitpid(pid, &status, 0);
	}
}
