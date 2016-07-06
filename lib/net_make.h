/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/


#ifndef _NET_MAKE_
#define _NET_MAKE_

#include <inttypes.h>

/*打开一个监听套接字*/
int32_t		net_open_listener(const char* ip, int32_t port);
/*关闭监听套接字*/
void		net_close_listener(int32_t listener);
/*接收新的连接*/
int32_t		net_accept_newsock(int32_t listener);
/*创建到远程目标机的连接*/
int32_t		net_connect(const char* ip, int32_t port);

#endif

