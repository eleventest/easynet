/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/


#include "net_make.h"
#include "net_define.h"
#include "net_sock.h"

/*打开一个监听套接字*/
int32_t net_open_listener(const char* ip, int32_t port)
{
	/*创建tcp套接字*/
	int32_t listener = sock_tcp();
	UT_CHECK_TRUE(listener != -1);
	/*设置可复用地址*/
	UT_VERIFY_TRUE(sock_reuseaddr(listener) == 0);
	/*绑定地址*/
	UT_CHECK_TRUE(sock_bind(listener, ip, port) == 0);
	/*设置为非阻塞*/
	UT_CHECK_TRUE(sock_set_nonblock(listener) == 0);
	/*开始监听*/
	UT_CHECK_TRUE(sock_listen(listener, 128) == 0);

	return listener;

FAIL:
	/*如果创建过程发生错误，则跳到这里关闭套接字*/
	net_close_listener(listener);
	return -1;
}

/*关闭监听套接字*/
void net_close_listener(int32_t listener)
{
	sock_close(listener);
}

/*接收新的连接*/
int32_t net_accept_newsock(int32_t listener)
{
	return accept(listener, 0, 0);
}

/*创建到远程目标机的连接*/
int32_t net_connect(const char* ip, int32_t port)
{
	/*创建tcp套接字*/
	int32_t target = sock_tcp();
	UT_CHECK_TRUE(target != -1);
	/*连接到目标机器*/
	UT_CHECK_TRUE(sock_connect(target, ip, port) == 0);

	return target;

FAIL:
	/*如果创建过程发生错误，则跳到这里关闭套接字*/
	sock_close(target);
	return -1;
}