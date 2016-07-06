/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/


#include "net_define.h"
#include "net_handle.h"

struct channel_t*	channel_malloc(struct net_t* net)
{
	struct channel_t* new_channel = 0;
	if (net->free_top<0)
	{
		return 0;
	}
	new_channel = net->queue_free[net->free_top];
	net->free_top--;
	return new_channel;
}

/*释放通道内部socket连接*/
static void channel_finalize(struct channel_t* channel)
{
	UT_VERIFY_TRUE(channel != 0);
	channel->connection_data = 0;
	channel->channel_data = 0;
	if (channel->sock != -1)
	{
		sock_close(channel->sock);
		channel->sock = -1;
	}

FAIL:
	return;
}

void channel_free(struct net_t* net, struct channel_t* channel)
{
	UT_CHECK_TRUE(channel != 0);
	UT_VERIFY_TRUE(net != 0);
	UT_VERIFY_TRUE(net->free_top + 1 < net->cfg.num_connection_limit);

	channel_finalize(channel);
	net->free_top++;
	net->queue_free[net->free_top] = channel;

FAIL:
	return;
}


void channel_init(struct channel_t* channel, int32_t idx, struct net_t* net)
{
	// 参数有效性检查
	UT_ASSERT_TRUE(idx >= 0);
	UT_ASSERT_TRUE(net != 0);
	UT_ASSERT_TRUE(channel != 0);

	channel->sock = -1;
	channel->index = idx;
	channel->net = net;
}
