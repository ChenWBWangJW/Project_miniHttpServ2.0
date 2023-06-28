#pragma once
#ifndef THREAD_CREATE_H
#define THREAD_CREATE_H

#include <pthread.h>
#include <malloc.h>

int createThread(int client_sock, void* do_http_request(void*));

#endif // !1

