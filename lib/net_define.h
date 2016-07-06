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

/*网络设置数据*/
struct net_config_t
{
	/*最大连接数限制*/
	int32_t		num_connection_limit;
	/*用户态接收缓冲区大小*/
	int32_t		recv_buf_size;
	/*epoll的epoll_event的大小*/
	int32_t		num_wait_event;
};

struct net_t;
struct channel_t
{
	/*加速索引，也可用户网络连接的唯一标识*/
	int32_t				index;
	/*所属网络模块*/
	struct net_t*	net;
	/*套接字句柄*/
	int32_t				sock;			// 网络套接字
	/*数据接收回调函数*/
	void(*connection_data)(struct net_t* net, int32_t channel, const char* data
		, int32_t datalen, void* netdata, void* channeldata);
	/*绑定的用户数据*/
	void*	channel_data;
};
typedef struct channel_t* pchannel_t;

/*网络模块数据*/
struct net_t
{
	/*用户配置数据*/
	struct net_config_t			cfg;
	/*epoll 句柄*/
	int32_t				epoll_handle;
	/*epoll事件缓存*/
	struct epoll_event*	wait_event;	
	/*接收缓存*/
	char*				rcv_buf;
	/*连接数据*/
	struct channel_t*	channels;
	/*空闲连接所在栈*/
	pchannel_t*			queue_free;
	int32_t				free_top;
	/*建立连接回调函数*/
	void(*new_func)(struct net_t* net, int32_t channel, void* netdata, void* channeldata);
	/*丢失连接回调函数*/
	void(*lost_func)(struct net_t* net, int32_t channel, void* netdata, void* channeldata);
	/*用户绑定数据*/
	void*	net_data;
};

/*参数检查快捷宏*/
#define UT_ASSERT_R(x)		do{if (!(x)){assert(x);return;}}while(0)
#define UT_ASSERT_RV(x,v)	do{if (!(x)){assert(x);return v;}}while(0)
#define UT_CHECKT_R(x)		do{if (!(x)){return;}}while(0)
#define UT_CHECK_RV(x,v)	do{if (!(x)){return v;}}while(0)

#define UT_CHECK_TRUE(con)		do{ if(!(con)){goto FAIL;} } while (0)
#define UT_VERIFY_TRUE(con)		do{ if(!(con)){assert(!#con);goto FAIL;} } while (0)

#define UT_ASSERT_TRUE(con)		do{ if(!(con)){assert(!#con);} } while (0)

#endif