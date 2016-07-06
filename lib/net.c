/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/



#include "net.h"
#include "net_define.h"
#include "net_handle.h"
#include "net_memory.h"

static struct net_config_t def_cfg = 
{
	1024,
	EPOLL_RCV_BUF_SIZE,
	MAX_WAIT_EVENT,
};

/*创建一个配置数据，并用默认配置初始化*/
struct net_config_t*	net_config_create()
{
	struct net_config_t* cfg = NET_NEW(struct net_config_t);
	UT_CHECK_TRUE(cfg != 0);

	*cfg = def_cfg;
	return cfg;

FAIL:
	if (cfg != 0)
	{
		NET_DELETE(cfg);
	}

	return 0;
}
/*删除配置数据*/
void	net_config_delete(struct net_config_t* cfg)
{
	UT_CHECK_TRUE(cfg != 0);
	NET_DELETE(cfg);
FAIL:
	return;
}

/*设置最大连接数*/
int32_t		net_config_set_connectionlimit(struct net_config_t* cfg, int32_t limit)
{
	UT_CHECK_TRUE(cfg != 0);
	UT_CHECK_TRUE(limit > 0 && limit <= 65536);

	cfg->num_connection_limit = limit;
	return 0;

FAIL:
	return -1;
}

/*创建网络句柄，所有的数据都在网络句柄中*/
struct net_t*	net_create(struct net_config_t* cfg,
	new_connection newfunc, lost_connection lostfunc, void* netdata)
{
	struct net_t* net = 0;
	int32_t i = 0;
	/*如果没有配置数据，就用默认配置数据*/
	if (cfg == 0)
	{
		cfg = &def_cfg;
	}
	/*参数检查*/
	UT_CHECK_TRUE(cfg->recv_buf_size > 0);
	UT_CHECK_TRUE(cfg->num_wait_event > 0);
	UT_CHECK_TRUE(cfg->num_connection_limit > 0);
	UT_CHECK_TRUE(newfunc != 0);
	UT_CHECK_TRUE(lostfunc != 0);
	/*申请网络句柄内存*/
	net = NET_NEW(struct net_t);
	UT_CHECK_TRUE(net != 0);
	/*保存数据*/
	net->cfg = *cfg;
	net->new_func = newfunc;
	net->lost_func = lostfunc;
	net->net_data = netdata;
	/*创建并初始化通道数据*/
	net->channels = NET_NEW_ARRAY(struct channel_t, cfg->num_connection_limit);
	UT_CHECK_TRUE(net->channels != 0);

	for (i = 0; i < cfg->num_connection_limit; i++)
	{
		struct channel_t* channel = &net->channels[i];
		channel_init(channel, i, net);
	}
	/*创建存储空闲通道的栈*/
	net->queue_free = NET_NEW_ARRAY(pchannel_t, cfg->num_connection_limit);
	UT_CHECK_TRUE(net->queue_free != 0);
	for (i = 0; i < cfg->num_connection_limit; i++)
	{
		net->queue_free[i] = &net->channels[i];
	}
	net->free_top = cfg->num_connection_limit-1;
	/*初始化epoll*/
	UT_VERIFY_TRUE(0 == init_net_handle(net));

	return net;

FAIL:
	/*如果创建过程中发生意外，则到这里释放资源*/
	net_delete(net);
	return 0;
}
/*删除网络句柄，绑定到网络句柄上的所有连接都会被关闭*/
void	net_delete(struct net_t* net)
{
	UT_VERIFY_TRUE(0 != net);
	/*关闭通道，并释放通道资源*/
	if (net->channels != 0)
	{
		int i = 0;
		for (i=0; i<net->cfg.num_connection_limit; i++)
		{
			struct channel_t* channel = &net->channels[i];
			if (channel->sock != -1)
			{
				sock_close(channel->sock);
				channel->sock = -1;
			}
		}
		NET_DELETE(net->channels);
	}
	
	NET_DELETE(net->queue_free);
	/*关闭epoll*/
	uninit_net_handle(net);
	// 释放资源
	NET_DELETE(net);

FAIL:
	return;
}
/*在一个通道上发送数据*/
int32_t		net_send(struct net_t* net, int32_t channel, const char* data, int32_t datalen)
{
	/*入参检查*/
	UT_CHECK_TRUE(net != 0);
	UT_CHECK_TRUE(channel >= 0 && channel < net->cfg.num_connection_limit);
	UT_VERIFY_TRUE(data != 0);
	UT_VERIFY_TRUE(datalen > 0);
	/*调用底层socket的send发送数据*/
	return sock_send(net->channels[channel].sock, data, datalen);

FAIL:
	return 0;
}

/*关闭一个连接通道*/
void	net_channel_close(struct net_t* net, int32_t channel)
{
	/*入参检查*/
	UT_CHECK_TRUE(net != 0);
	UT_CHECK_TRUE(channel >= 0 && channel < net->cfg.num_connection_limit);
	/*关闭socket，注意不是close是shutdown*/
	sock_shutdown(net->channels[channel].sock);

FAIL:
	return;
}

/*投递一个已经建立好的连接到网络中，channeldata是绑定到通道上的数据*/
int32_t	net_deliver_socket(struct net_t* net,
	int32_t sock, recv_data rcvfunc, void* channeldata)
{
	struct channel_t* channel = 0;
	/*入参检查*/
	UT_VERIFY_TRUE(net != 0);
	UT_VERIFY_TRUE(rcvfunc != 0);
	/*设置保活定时器*/
	UT_CHECK_TRUE(0 == sock_set_keepalive(sock));
	/*设置为非阻塞*/
	UT_CHECK_TRUE(0 == sock_set_nonblock(sock));

	/*申请空闲通道资源*/
	channel = channel_malloc(net);
	UT_CHECK_TRUE(channel != 0);
	/*初始化通道数据*/
	channel->sock = sock;
	channel->connection_data = rcvfunc;
	channel->channel_data = channeldata;

	UT_CHECK_TRUE(0 == do_deliver_socket(net, channel));
	/*回调连接建立函数*/
	net->new_func(net, channel->index, net->net_data, channel->channel_data);
	/*返回通道的id*/
	return channel->index;

FAIL:
	/*如果发生意外，到这里关闭套接字*/
	sock_close(sock);
	if (channel != 0)
	{
		/*释放通道*/
		channel_free(net, channel);
	}

	return -1;
}