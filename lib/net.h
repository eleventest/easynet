/*
The MIT License (MIT)

Copyright (c) 2016 eleven

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


#ifndef _NET_LIB_
#define _NET_LIB_

#include <inttypes.h>
/*�������������*/
struct net_config_t;

/*������*/
struct net_t;

/*����ͨ��*/
struct channel_t;

/*���ӽ����ص�������net����������channel���½���������ͨ����id��netdata�ǰ󶨵�net�ϵ����ݣ�channeldata�ǰ󶨵�channel�ϵ�����*/
typedef void(*new_connection)	(struct net_t* net, int32_t channel, void* netdata, void* channeldata);

/*���ӶϿ��ص�����*/
typedef void(*lost_connection)	(struct net_t* net, int32_t channel, void* netdata, void* channeldata);

/*�����������ݵ���Ļص�������data�����ݻ�������ʼ��ַ��datalen����Ч���ݳ���*/
typedef void(*recv_data)		(struct net_t* net, int32_t channel, const char* data
	, int32_t datalen, void* netdata, void* channeldata);

/*�������������*/
struct net_config_t*	net_config_create();
void					net_config_delete(struct net_config_t* cfg);

/*���������*/
int32_t		net_config_set_connectionlimit(struct net_config_t* cfg, int32_t limit);

/*����һ����������netdata�ǰ󶨵��������ϵ��û�����*/
struct net_t*		net_create(struct net_config_t* cfg, new_connection newfunc, 
										lost_connection lostfunc, void* netdata);

/*ɾ�����������󶨵��������ϵ��������Ӷ��ᱻ�ر�*/
void		net_delete(struct net_t* net);

/*��һ��ͨ���Ϸ�������*/
int32_t		net_send(struct net_t* net, int32_t channel, const char* data, int32_t datalen);

/*�������ݴ���������յ����ݣ��ص������ᱻ����*/
int32_t		net_pool_once(struct net_t* net, int32_t weight, int32_t waittime);

/*�ر�һ������ͨ��*/
void		net_channel_close(struct net_t* net, int32_t channel);

/*Ͷ��һ���Ѿ������õ����ӵ������У�channeldata�ǰ󶨵�ͨ���ϵ�����*/
int32_t	net_deliver_socket(struct net_t* net, int32_t sock,
						recv_data rcvfunc, void* channeldata);


#endif
