#pragma once
#ifndef MAIN_FUNC_H
#define MAIN_FUNC_H

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <sys/wait.h>

#define SIZE_OF_BUF 1024
#define SIZE_OF_METHOD 64
#define SIZE_OF_URL 256
#define SIZE_OF_HTTP_VERSON 64
#define SIZE_OF_PATH 256
#define SERV_PORT 80

//1.function
void* do_http_request(void* pclient_sock);
void do_http_response(int client_sock, const char* path);
int headers(int client_sock, FILE* resourse);
void cat(int client_sock, FILE* resourse);
int get_line(int sock, char* buf, int size);
void execute_cgi(int client, const char* path, const char* method, const char* query_string);

//2.error_catch
void not_found(int client_sock);
void unimplemented(int client_sock);
void bad_request(int client_sock);
void inner_error(int client_sock);
void cannot_execute(int client_sock);

#endif // !1
