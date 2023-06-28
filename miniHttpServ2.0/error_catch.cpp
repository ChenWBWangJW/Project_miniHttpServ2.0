// Description: ´íÎó´¦Àíº¯Êý
#include "mainFunc.h"

static int debug = 1;

void not_found(int client_sock)
{
	const char* reply = "HTTP/1.0 404 NOT FOUND\r\n\
Content-Type: text/html\r\n\
\r\n\
<HTML lang=\"zh-CN\">\r\n\
<mate content=\"text/html; charset=utf-8\" http-equiv=\"content-Type\">\r\n\
<HEAD>\r\n\
<TITLE>NOT FOUND</TITLE>\r\n\
</HEAD>\r\n\
<BODY>\r\n\
	<P>File is inexistent!\r\n\
	<P>The server could not fulfill your request because the resource specified is unavaliable or nonexistent.\r\n\
</BODY/>\r\n\
</HTML>";

	int len = write(client_sock, reply, strlen(reply));
	if (debug) fprintf(stdout, reply);

	if (len <= 0)
	{
		fprintf(stderr, "send reply faild. reason: %s\n", strerror(errno));
	}
}


void unimplemented(int client_sock)
{
	const char* reply = "HTTP/1.0 501 Method Not Implemented\r\n\
Content-Type: text/html\r\n\
\r\n\
<HTML lang=\"zh-CN\">\r\n\
<mate content=\"text/html; charset=utf-8\" http-equiv=\"content-Type\">\r\n\
<HEAD>\r\n\
<TITLE>Method Not Implemented</TITLE>\r\n\
</HEAD>\r\n\
<BODY>\r\n\
	<P>HTTP request method not supported.\r\n\
</BODY/>\r\n\
</HTML>";

	int len = write(client_sock, reply, strlen(reply));
	if (debug) fprintf(stdout, reply);

	if (len <= 0)
	{
		fprintf(stderr, "send reply faild. reason: %s\n", strerror(errno));
	}
}


void bad_request(int client_sock)
{
	const char* reply = "HTTP/1.0 400 BAD REQUEST\r\n\
Content-Type: text/html\r\n\
\r\n\
<HTML lang=\"zh-CN\">\r\n\
<mate content=\"text/html; charset=utf-8\" http-equiv=\"content-Type\">\r\n\
<HEAD>\r\n\
<TITLE>BAD REQUEST</TITLE>\r\n\
</HEAD>\r\n\
<BODY>\r\n\
	<P>your browser sent a bad request.\r\n\
</BODY/>\r\n\
</HTML>";

	int len = write(client_sock, reply, strlen(reply));
	if (debug) fprintf(stdout, reply);

	if (len <= 0)
	{
		fprintf(stderr, "send reply faild. reason: %s\n", strerror(errno));
	}
}


void inner_error(int client_sock)
{
	const char* reply = "HTTP/1.0 500 NOT Internal Server Error\r\n\
Content-Type: text/html\r\n\
\r\n\
<HTML>\r\n\
<HEAD>\r\n\
<TITLE>Inner Error</TITLE>\r\n\
</HEAD>\r\n\
<BODY>\r\n\
	<P>Server Inner Error\r\n\
	<P>The server internal Error\r\n\
</BODY/>\r\n\
</HTML>";

	int len = write(client_sock, reply, strlen(reply));
	if (debug) fprintf(stdout, reply);

	if (len <= 0)
	{
		fprintf(stderr, "server internal error. reason: %s\n", strerror(errno));
	}
}

void cannot_execute(int client_sock)
{
	const char* reply = "HTTP/1.0 500 NOT Internal Server Error\r\n\
Content-Type: text/html\r\n\
\r\n\
<HTML>\r\n\
<HEAD>\r\n\
<TITLE>Inner Error</TITLE>\r\n\
</HEAD>\r\n\
<BODY>\r\n\
	<P>Error prohibited CGI execution.\r\n\
	<P>The server internal Error\r\n\
</BODY/>\r\n\
</HTML>";

	int len = write(client_sock, reply, strlen(reply));
	if (debug) fprintf(stdout, reply);

	if (len <= 0)
	{
		fprintf(stderr, "server internal error. reason: %s\n", strerror(errno));
	}
}