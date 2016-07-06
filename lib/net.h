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
/*网络库配置数据*/
struct net_config_t;

/*网络句柄*/
struct net_t;

/*连接通道*/
struct channel_t;

/*连接建立回调函数，net是网络句柄，channel是新建立的连接通道的id，netdata是绑定到net上的数据，channeldata是绑定到channel上的数据*/
typedef void(*new_connection)	(struct net_t* net, int32_t channel, void* netdata, void* channeldata);

/*连接断开回调函数*/
typedef void(*lost_connection)	(struct net_t* net, int32_t channel, void* netdata, void* channeldata);

/*连接上有数据到达的回调函数，data是数据缓冲区起始地址，datalen是有效数据长度*/
typedef void(*recv_data)		(struct net_t* net, int32_t channel, const char* data
	, int32_t datalen, void* netdata, void* channeldata);

/*网络库配置数据*/
struct net_config_t*	net_config_create();
void					net_config_delete(struct net_config_t* cfg);

/*最大连接数*/
int32_t		net_config_set_connectionlimit(struct net_config_t* cfg, int32_t limit);

/*创建一个网络句柄，netdata是绑定到网络句柄上的用户数据*/
struct net_t*		net_create(struct net_config_t* cfg, new_connection newfunc, 
										lost_connection lostfunc, void* netdata);

/*删除网络句柄，绑定到网络句柄上的所有连接都会被关闭*/
void		net_delete(struct net_t* net);

/*在一个通道上发送数据*/
int32_t		net_send(struct net_t* net, int32_t channel, const char* data, int32_t datalen);

/*接收数据处理，如果接收到数据，回调函数会被调用*/
int32_t		net_pool_once(struct net_t* net, int32_t weight, int32_t waittime);

/*关闭一个连接通道*/
void		net_channel_close(struct net_t* net, int32_t channel);

/*投递一个已经建立好的连接到网络中，channeldata是绑定到通道上的数据*/
int32_t	net_deliver_socket(struct net_t* net, int32_t sock,
						recv_data rcvfunc, void* channeldata);


#endif
