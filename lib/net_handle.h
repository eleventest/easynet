/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/



#ifndef _NET_HANDLE_
#define _NET_HANDLE_

#include <inttypes.h>
/*初始化epoll*/
int32_t		init_net_handle(struct net_t* net);
/*释放epoll*/
void		uninit_net_handle(struct net_t* net);
/*投递一个通道到epoll中*/
int32_t		do_deliver_socket(struct net_t* net, struct channel_t* channel);


/*申请空闲通道资源*/
struct channel_t*	channel_malloc(struct net_t* net);

/*释放通道资源*/
void			channel_free(struct net_t* net, struct channel_t* channel);

/*初始化通道*/
void	channel_init(struct channel_t* channel, int32_t idx, struct net_t* net);

#endif
