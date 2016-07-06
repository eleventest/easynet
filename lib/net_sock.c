/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/

#include <memory.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>

#include "net_sock.h"

int32_t	sock_tcp()
{
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

int32_t sock_close(int32_t sock)
{
	return close(sock);
}

int32_t sock_shutdown(int32_t sock)
{
	return shutdown(sock, SHUT_RDWR);
}

int32_t sock_set_nonblock(int32_t sock)
{
	int32_t err = -1;
	err = fcntl(sock, F_GETFL, 0);
	if (err == -1)
	{
		return -1;
	}
	err = fcntl(sock, F_SETFL, err | O_NONBLOCK);

	return err;
}

int32_t	 ut_net_ip(const char* ip)
{
	if (ip == 0)
	{
		return 0;
	}
	else
	{
		return (int32_t)inet_addr(ip);
	}
}

int16_t	ut_net_port(int16_t port)
{
	return htons(port);
}

struct sockaddr_in ut_ip4_addr(const char* ip, int32_t port)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = ut_net_port((int16_t)port);
	addr.sin_addr.s_addr = ut_net_ip(ip);
	
	return addr;
}

int32_t sock_bind(int32_t sock, const char* ip, int32_t port)
{
	struct sockaddr_in addr = ut_ip4_addr(ip, port);;
	return bind(sock, (struct sockaddr*)&addr, sizeof(addr));
}

int32_t sock_connect(int32_t sock, const char* ip, int32_t port)
{
	struct sockaddr_in addr = ut_ip4_addr(ip, port);
	return connect(sock, (struct sockaddr*)&addr, sizeof(addr));
}

int32_t sock_listen(int32_t sock, int32_t que)
{
	return listen(sock, que);
}

int32_t sock_set_sendbuf_size(int32_t sock, int32_t size)
{
	int32_t sizelen = sizeof(size);
	return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char*)&size, sizelen);
}

int32_t sock_set_recvbuf_size(int32_t sock, int32_t size)
{
	int32_t sizelen = sizeof(size);
	return setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char*)&size, sizelen);
}

int32_t sock_set_keepalive(int32_t sock)
{
	// 设置保活定时器
	int32_t keepalive = 1;
	int32_t keepidle = 120;
	int32_t keepinterval = 5;
	int32_t keepcount = 4;
	int32_t err = 0;

	err = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (void*)&keepalive, sizeof(keepalive));
	if (0 != err) return err;
	err = setsockopt(sock, SOL_TCP, TCP_KEEPIDLE, (void*)&keepidle, sizeof(keepidle));
	if (0 != err) return err;
	err = setsockopt(sock, SOL_TCP, TCP_KEEPINTVL, (void*)&keepinterval, sizeof(keepinterval));
	if (0 != err) return err;
	err = setsockopt(sock, SOL_TCP, TCP_KEEPCNT, (void*)&keepcount, sizeof(keepcount));
	if (0 != err) return err;

	int32_t on=1;

	return setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
}

int32_t sock_send(int32_t sock, const char* data, int32_t size)
{
	return send(sock, data, size, MSG_NOSIGNAL);
}

int32_t sock_recv(int32_t sock, char* data, int32_t size)
{
	return recv(sock, data, size, 0);
}

int32_t sock_reuseaddr(int32_t sock)
{
	int32_t reuseaddr = 1;
	return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuseaddr, sizeof(reuseaddr));
}
