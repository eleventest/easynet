
#include <inttypes.h>

#include "net.h"
#include "net_make.h"
#include "net_thread.h"
#include "net_time.h"
#include "stdio.h"
#include "stdlib.h"

#include <inttypes.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <assert.h>


static const char* server_ip = "127.0.0.1";
static int32_t server_port = 9001;

static char buffer[64 * 1024] = { 0 };

static int32_t serv_recvbyte = 0;
static int32_t serv_recvpack = 0;
static int32_t serv_connnum = 0;

static int32_t cli_recvbyte = 0;
static int32_t cli_recvpack = 0;
static int32_t cli_connnum = 0;

/*������������Ϣ�ص�����*/
static void serv_new_connection(struct net_t* net, int32_t channel, void* netdata, void* channeldata)
{
	serv_connnum++;
}

static void serv_lost_connection(struct net_t* net, int32_t channel, void* netdata, void* channeldata)
{
	serv_connnum--;
	printf("lost\n");
}

static void serv_rcv_data(struct net_t* net, int32_t channel, const char* data
	, int32_t datalen, void* netdata, void* channeldata)
{
	serv_recvbyte += datalen;
	serv_recvpack++;
	/*ƹ�Ҳ��ԣ��յ����ݺ��������͸��Է�*/
	net_send(net, channel, data, datalen);
}
/*�������߳�*/
static void* server_thread(void* param)
{
	struct net_t* net;

	int64_t bt = 0;
	int64_t et = 0;
	int32_t		listener = 0;
	/*��һ��������*/
	listener = net_open_listener(server_ip, server_port);
	if (listener == -1)
	{
		printf("open listener error.\r\n");
		return 0;
	}
	/*����һ��������*/
	net = net_create(0, serv_new_connection, serv_lost_connection, 0);
	if (net == 0)
	{
		printf("open net failed.\r\n");
		return 0;
	}

	bt = net_gettickcount();
	et = bt;

	/*��ʼ���������¼�*/
	while (1)
	{
		int32_t new_sock = -1;
		/*�����첽io*/
		net_pool_once(net, 1024, 1);
		/*���������ӽ���*/
		while ((new_sock = net_accept_newsock(listener)) != -1)
		{
			if (0 > net_deliver_socket(net, new_sock, serv_rcv_data, 0))
			{
				/*������������أ���Ͷ��ʧ��*/
				printf("deliver socket failed.\r\n");
			}
		}

		et = net_gettickcount();
		if (et / 1000 != bt / 1000)
		{
			printf("server_connnum:%d\tspeed:%5.2fMBps\tpack num:%dpps.\r\n"
				, serv_connnum, serv_recvbyte / 1024.0f / 1024.0f, serv_recvpack);

			serv_recvbyte = 0;
			serv_recvpack = 0;
			bt = et;
		}
	}

	return 0;
}
/*�ͻ���������Ϣ�ص�����*/
static void cli_new_connection(struct net_t* net, int32_t channel, void* netdata, void* channeldata)
{
	cli_connnum++;
	/*ƹ�Ҳ��ԣ����ӽ���ʱ��Է�����1024�ֽڵ�����*/
	net_send(net, channel, buffer, 1024);
}

static void cli_lost_connection(struct net_t* net, int32_t channel, void* netdata, void* channeldata)
{
	cli_connnum--;
	printf("lost\n");
}

static void cli_rcv_data(struct net_t* net, int32_t channel, const char* data
	, int32_t datalen, void* netdata, void* channeldata)
{
	cli_recvbyte += datalen;
	cli_recvpack++;
	/*ƹ�Ҳ��ԣ��յ����ݺ��������͸��Է�*/
	net_send(net, channel, data, datalen);
}
/*�ͻ����߳�*/
static void* client_thread(void* param)
{
	int32_t i = 0;
	struct net_t* net;

	int64_t bt = 0;
	int64_t et = 0;
	/*����һ��������*/
	net = net_create(0, cli_new_connection, cli_lost_connection, 0);
	if (net == 0)
	{
		printf("open net failed.\r\n");
		return 0;
	}
	/*����1000�����������˵�����*/
	for (i = 0; i < 1000; i++)
	{
		int32_t sock = sock_tcp();
		if (sock == -1)
		{
			printf("create tcp sock failed.\r\n");
			continue;
		}
		int32_t err = sock_connect(sock, server_ip, server_port);
		if (err != 0)
		{
			printf("connect to server failed.%d\r\n", -err);
			sock_close(err);
			continue;
		}
		if (0 > net_deliver_socket(net, sock, cli_rcv_data, 0))
		{
			/*������������أ���Ͷ��ʧ��*/
			printf("deliver socket failed.\r\n");
		}
	}

	bt = net_gettickcount();
	et = bt;

	while (1)
	{
		/*��ʼ���������¼�*/
		net_pool_once(net, 1024, 1);

		et = net_gettickcount();
		if (et / 1000 != bt / 1000)
		{
			printf("server_connnum:%d\tspeed:%5.2fMBps\tpack num:%dpps.\r\n"
				, cli_connnum, cli_recvbyte / 1024.0f / 1024.0f, cli_recvpack);

			cli_recvbyte = 0;
			cli_recvpack = 0;
			bt = et;
		}
	}

	return 0;
}

int32_t net_test()
{
	ut_thread_t* cthread = 0;
	ut_thread_t* sthread = 0;
	
#ifdef __linux__
	struct rlimit rlim;
	rlim.rlim_max=65536;
	rlim.rlim_cur = 65536;
	if (0 == setrlimit(RLIMIT_NOFILE, &rlim))
	{
		printf("set limit file to 65536\r\n");
	}
#endif
	/*����һ���ͻ����̺߳�һ�����������߳�*/
	cthread = ut_thread_new(client_thread, 0);
	sthread = ut_thread_new(server_thread, 0);

	for (;;)
	{
		/*����һ����ѭ������*/
		ut_sleep(1);
	}	
	
	ut_thread_delete(cthread);
	ut_thread_delete(sthread);

	return 0;

}
