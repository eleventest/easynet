/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/


#ifndef _NET_MAKE_
#define _NET_MAKE_

#include <inttypes.h>

/*��һ�������׽���*/
int32_t		net_open_listener(const char* ip, int32_t port);
/*�رռ����׽���*/
void		net_close_listener(int32_t listener);
/*�����µ�����*/
int32_t		net_accept_newsock(int32_t listener);
/*������Զ��Ŀ���������*/
int32_t		net_connect(const char* ip, int32_t port);

#endif

