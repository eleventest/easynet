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
/*����tcp�׽���*/
int32_t	sock_tcp();
/*�ر��׽���*/
int32_t sock_close(int32_t sock);
/*��ַ����*/
int32_t sock_reuseaddr(int32_t sock);
/*�󶨵�ַ*/
int32_t sock_bind(int32_t sock, const char* ip, int32_t port);
/*��������*/
int32_t sock_connect(int32_t sock, const char* ip, int32_t port);
/*���ñ��ʱ��*/
int32_t sock_set_keepalive(int32_t sock);
/*����Ϊ������*/
int32_t sock_set_nonblock(int32_t sock);
/*�ر�����*/
int32_t sock_shutdown(int32_t sock);
/*��ʼ����*/
int32_t sock_listen(int32_t sock, int32_t que);
/*���÷��ͻ�����*/
int32_t sock_set_sendbuf_size(int32_t sock, int32_t size);
/*���ý��ջ�����*/
int32_t sock_set_recvbuf_size(int32_t sock, int32_t size);
/*��������*/
int32_t sock_send(int32_t sock, const char* data, int32_t size);
/*��������*/
int32_t sock_recv(int32_t sock, char* data, int32_t size);


#endif
