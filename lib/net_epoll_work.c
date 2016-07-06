/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/


#include <sys/resource.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>

#include "net_define.h"
#include "net.h"
#include "net_handle.h"
#include "net_memory.h"

int32_t init_net_handle(struct net_t* net)
{
	net->epoll_handle = -1;
	/*创建epoll_event缓存*/
	net->wait_event = NET_NEW_ARRAY(struct epoll_event, net->cfg.num_wait_event);
	UT_VERIFY_TRUE(net->wait_event != 0);

	net->rcv_buf = NET_NEW_ARRAY(char, net->cfg.recv_buf_size);
	UT_VERIFY_TRUE(net->rcv_buf != 0);

	/*创建epoll句柄*/
	net->epoll_handle = epoll_create(65536);

	return net->epoll_handle != -1?0:-1;

FAIL:
	if (net->wait_event != 0)
	{
		NET_DELETE(net->wait_event);
		net->wait_event = 0;
	}
	if (net->rcv_buf != 0)
	{
		NET_DELETE(net->rcv_buf);
		net->rcv_buf = 0;
	}

	return -1;
}

void uninit_net_handle(struct net_t* net)
{
	UT_CHECK_TRUE (net != 0);
	/*释放epoll*/
	close(net->epoll_handle);
	net->epoll_handle = -1;
	/*释放epoll_event*/
	NET_DELETE(net->wait_event);
	net->wait_event = 0;

FAIL:
	return;
}

int32_t do_deliver_socket(struct net_t* net, struct channel_t* channel)
{
	int32_t err = 0;

	struct epoll_event eevent;
	eevent.data.ptr = channel;
	eevent.events = EPOLLIN;
	/*投递socket到epoll中，使用水平触发方式，只有接收数据事件，发送不需要事件通知*/
	return epoll_ctl(net->epoll_handle, EPOLL_CTL_ADD, channel->sock, &eevent);
}

static void do_lost_connection(struct channel_t* channel)
{
	UT_VERIFY_TRUE(0 != channel);
	UT_VERIFY_TRUE(0 != channel->net);
	/*通知回调函数，连接断开*/
	channel->net->lost_func(channel->net, channel->index, channel->net->net_data, channel->channel_data);
	/*关闭socket句柄*/
	sock_close(channel->sock);
	channel->sock = -1;
	/*释放通道资源*/
	channel_free(channel->net, channel);
	/*从epoll中删除*/
	epoll_ctl(channel->net->epoll_handle, EPOLL_CTL_DEL, channel->sock, 0);

FAIL:
	return;
}

int32_t net_pool_once(struct net_t* net, int32_t weight, int32_t waittime)
{
	int32_t i = 0;
	int32_t wait_num = 0;
	/*入参范围调整*/
	if (weight < 1)
	{
		weight = 1;
	}
	if (weight > net->cfg.num_wait_event)
	{
		weight = net->cfg.num_wait_event;
	}
	/*查看是否有事件发生*/
	wait_num = epoll_wait(net->epoll_handle, net->wait_event
		, weight, waittime);

	for ( ; i < wait_num; i++)
	{
		struct channel_t* channel = (struct channel_t*)net->wait_event[i].data.ptr;
		
		if ((net->wait_event[i].events & ~EPOLLIN) == 0)
		{
			// 处理接收事件
			int32_t nrcvl = sock_recv(channel->sock, net->rcv_buf, 
				net->cfg.recv_buf_size);
			if (nrcvl > 0)
			{
				/*如果收到数据则调用数据接收回调函数*/
				channel->connection_data(net, channel->index, net->rcv_buf, 
					nrcvl, net->net_data, channel->channel_data);
			}
			else if (nrcvl == 0 || (nrcvl != EINTR && nrcvl != EAGAIN))
			{
				/*接收错误，或对方关闭则关闭此通道*/
				do_lost_connection(channel);
			}
		}
		else
		{
			/*处理错误，关闭通道*/
			do_lost_connection(channel);
		}
	}

	return wait_num;
}
