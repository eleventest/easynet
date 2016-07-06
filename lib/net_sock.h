/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/



 
#ifndef _NET_SOCK_H_
#define _NET_SOCK_H_

#include <inttypes.h>

int32_t			ut_net_ip(const char* ip);
int16_t			ut_net_port(int16_t port);
/*创建tcp套接字*/
int32_t	sock_tcp();
/*关闭套接字*/
int32_t sock_close(int32_t sock);
/*地址复用*/
int32_t sock_reuseaddr(int32_t sock);
/*绑定地址*/
int32_t sock_bind(int32_t sock, const char* ip, int32_t port);
/*建立连接*/
int32_t sock_connect(int32_t sock, const char* ip, int32_t port);
/*设置保活定时器*/
int32_t sock_set_keepalive(int32_t sock);
/*设置为非阻塞*/
int32_t sock_set_nonblock(int32_t sock);
/*关闭连接*/
int32_t sock_shutdown(int32_t sock);
/*开始监听*/
int32_t sock_listen(int32_t sock, int32_t que);
/*设置发送缓冲区*/
int32_t sock_set_sendbuf_size(int32_t sock, int32_t size);
/*设置接收缓冲区*/
int32_t sock_set_recvbuf_size(int32_t sock, int32_t size);
/*发送数据*/
int32_t sock_send(int32_t sock, const char* data, int32_t size);
/*接收数据*/
int32_t sock_recv(int32_t sock, char* data, int32_t size);


#endif
