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

/*����һ���������ݣ�����Ĭ�����ó�ʼ��*/
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
/*ɾ����������*/
void	net_config_delete(struct net_config_t* cfg)
{
	UT_CHECK_TRUE(cfg != 0);
	NET_DELETE(cfg);
FAIL:
	return;
}

/*�������������*/
int32_t		net_config_set_connectionlimit(struct net_config_t* cfg, int32_t limit)
{
	UT_CHECK_TRUE(cfg != 0);
	UT_CHECK_TRUE(limit > 0 && limit <= 65536);

	cfg->num_connection_limit = limit;
	return 0;

FAIL:
	return -1;
}

/*���������������е����ݶ�����������*/
struct net_t*	net_create(struct net_config_t* cfg,
	new_connection newfunc, lost_connection lostfunc, void* netdata)
{
	struct net_t* net = 0;
	int32_t i = 0;
	/*���û���������ݣ�����Ĭ����������*/
	if (cfg == 0)
	{
		cfg = &def_cfg;
	}
	/*�������*/
	UT_CHECK_TRUE(cfg->recv_buf_size > 0);
	UT_CHECK_TRUE(cfg->num_wait_event > 0);
	UT_CHECK_TRUE(cfg->num_connection_limit > 0);
	UT_CHECK_TRUE(newfunc != 0);
	UT_CHECK_TRUE(lostfunc != 0);
	/*�����������ڴ�*/
	net = NET_NEW(struct net_t);
	UT_CHECK_TRUE(net != 0);
	/*��������*/
	net->cfg = *cfg;
	net->new_func = newfunc;
	net->lost_func = lostfunc;
	net->net_data = netdata;
	/*��������ʼ��ͨ������*/
	net->channels = NET_NEW_ARRAY(struct channel_t, cfg->num_connection_limit);
	UT_CHECK_TRUE(net->channels != 0);

	for (i = 0; i < cfg->num_connection_limit; i++)
	{
		struct channel_t* channel = &net->channels[i];
		channel_init(channel, i, net);
	}
	/*�����洢����ͨ����ջ*/
	net->queue_free = NET_NEW_ARRAY(pchannel_t, cfg->num_connection_limit);
	UT_CHECK_TRUE(net->queue_free != 0);
	for (i = 0; i < cfg->num_connection_limit; i++)
	{
		net->queue_free[i] = &net->channels[i];
	}
	net->free_top = cfg->num_connection_limit-1;
	/*��ʼ��epoll*/
	UT_VERIFY_TRUE(0 == init_net_handle(net));

	return net;

FAIL:
	/*������������з������⣬�������ͷ���Դ*/
	net_delete(net);
	return 0;
}
/*ɾ�����������󶨵��������ϵ��������Ӷ��ᱻ�ر�*/
void	net_delete(struct net_t* net)
{
	UT_VERIFY_TRUE(0 != net);
	/*�ر�ͨ�������ͷ�ͨ����Դ*/
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
	/*�ر�epoll*/
	uninit_net_handle(net);
	// �ͷ���Դ
	NET_DELETE(net);

FAIL:
	return;
}
/*��һ��ͨ���Ϸ�������*/
int32_t		net_send(struct net_t* net, int32_t channel, const char* data, int32_t datalen)
{
	/*��μ��*/
	UT_CHECK_TRUE(net != 0);
	UT_CHECK_TRUE(channel >= 0 && channel < net->cfg.num_connection_limit);
	UT_VERIFY_TRUE(data != 0);
	UT_VERIFY_TRUE(datalen > 0);
	/*���õײ�socket��send��������*/
	return sock_send(net->channels[channel].sock, data, datalen);

FAIL:
	return 0;
}

/*�ر�һ������ͨ��*/
void	net_channel_close(struct net_t* net, int32_t channel)
{
	/*��μ��*/
	UT_CHECK_TRUE(net != 0);
	UT_CHECK_TRUE(channel >= 0 && channel < net->cfg.num_connection_limit);
	/*�ر�socket��ע�ⲻ��close��shutdown*/
	sock_shutdown(net->channels[channel].sock);

FAIL:
	return;
}

/*Ͷ��һ���Ѿ������õ����ӵ������У�channeldata�ǰ󶨵�ͨ���ϵ�����*/
int32_t	net_deliver_socket(struct net_t* net,
	int32_t sock, recv_data rcvfunc, void* channeldata)
{
	struct channel_t* channel = 0;
	/*��μ��*/
	UT_VERIFY_TRUE(net != 0);
	UT_VERIFY_TRUE(rcvfunc != 0);
	/*���ñ��ʱ��*/
	UT_CHECK_TRUE(0 == sock_set_keepalive(sock));
	/*����Ϊ������*/
	UT_CHECK_TRUE(0 == sock_set_nonblock(sock));

	/*�������ͨ����Դ*/
	channel = channel_malloc(net);
	UT_CHECK_TRUE(channel != 0);
	/*��ʼ��ͨ������*/
	channel->sock = sock;
	channel->connection_data = rcvfunc;
	channel->channel_data = channeldata;

	UT_CHECK_TRUE(0 == do_deliver_socket(net, channel));
	/*�ص����ӽ�������*/
	net->new_func(net, channel->index, net->net_data, channel->channel_data);
	/*����ͨ����id*/
	return channel->index;

FAIL:
	/*����������⣬������ر��׽���*/
	sock_close(sock);
	if (channel != 0)
	{
		/*�ͷ�ͨ��*/
		channel_free(net, channel);
	}

	return -1;
}