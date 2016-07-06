/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/



#ifndef _NET_HANDLE_
#define _NET_HANDLE_

#include <inttypes.h>
/*��ʼ��epoll*/
int32_t		init_net_handle(struct net_t* net);
/*�ͷ�epoll*/
void		uninit_net_handle(struct net_t* net);
/*Ͷ��һ��ͨ����epoll��*/
int32_t		do_deliver_socket(struct net_t* net, struct channel_t* channel);


/*�������ͨ����Դ*/
struct channel_t*	channel_malloc(struct net_t* net);

/*�ͷ�ͨ����Դ*/
void			channel_free(struct net_t* net, struct channel_t* channel);

/*��ʼ��ͨ��*/
void	channel_init(struct channel_t* channel, int32_t idx, struct net_t* net);

#endif
