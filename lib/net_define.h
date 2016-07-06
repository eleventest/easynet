/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/



#ifndef _NET_DEFINE_
#define _NET_DEFINE_

#include <sys/epoll.h>
#include <assert.h>

#define MAX_WAIT_EVENT		128
#define EPOLL_RCV_BUF_SIZE	1024*128

/*������������*/
struct net_config_t
{
	/*�������������*/
	int32_t		num_connection_limit;
	/*�û�̬���ջ�������С*/
	int32_t		recv_buf_size;
	/*epoll��epoll_event�Ĵ�С*/
	int32_t		num_wait_event;
};

struct net_t;
struct channel_t
{
	/*����������Ҳ���û��������ӵ�Ψһ��ʶ*/
	int32_t				index;
	/*��������ģ��*/
	struct net_t*	net;
	/*�׽��־��*/
	int32_t				sock;			// �����׽���
	/*���ݽ��ջص�����*/
	void(*connection_data)(struct net_t* net, int32_t channel, const char* data
		, int32_t datalen, void* netdata, void* channeldata);
	/*�󶨵��û�����*/
	void*	channel_data;
};
typedef struct channel_t* pchannel_t;

/*����ģ������*/
struct net_t
{
	/*�û���������*/
	struct net_config_t			cfg;
	/*epoll ���*/
	int32_t				epoll_handle;
	/*epoll�¼�����*/
	struct epoll_event*	wait_event;	
	/*���ջ���*/
	char*				rcv_buf;
	/*��������*/
	struct channel_t*	channels;
	/*������������ջ*/
	pchannel_t*			queue_free;
	int32_t				free_top;
	/*�������ӻص�����*/
	void(*new_func)(struct net_t* net, int32_t channel, void* netdata, void* channeldata);
	/*��ʧ���ӻص�����*/
	void(*lost_func)(struct net_t* net, int32_t channel, void* netdata, void* channeldata);
	/*�û�������*/
	void*	net_data;
};

/*��������ݺ�*/
#define UT_ASSERT_R(x)		do{if (!(x)){assert(x);return;}}while(0)
#define UT_ASSERT_RV(x,v)	do{if (!(x)){assert(x);return v;}}while(0)
#define UT_CHECKT_R(x)		do{if (!(x)){return;}}while(0)
#define UT_CHECK_RV(x,v)	do{if (!(x)){return v;}}while(0)

#define UT_CHECK_TRUE(con)		do{ if(!(con)){goto FAIL;} } while (0)
#define UT_VERIFY_TRUE(con)		do{ if(!(con)){assert(!#con);goto FAIL;} } while (0)

#define UT_ASSERT_TRUE(con)		do{ if(!(con)){assert(!#con);} } while (0)

#endif