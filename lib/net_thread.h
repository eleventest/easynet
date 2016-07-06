/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/



#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_

#include <inttypes.h>

typedef struct ut_thread		ut_thread_t;
typedef unsigned long			thread_id_t;
typedef void* thread_handle;

/* �߳�ִ�к�����.*/
typedef void*(*ut_thread_func_t)(void*);

/*����һ���߳�ִ����*/
ut_thread_t*	ut_thread_new(ut_thread_func_t thread_func, void* arg);

thread_handle	ut_thread_handle(ut_thread_t* thread);

/*ɾ��һ���߳�*/
void	ut_thread_delete(ut_thread_t* a_thread);

/*�ȴ�һ���̵߳Ľ���*/
void	ut_thread_join(ut_thread_t* a_thread);

/*��ȡ��ǰ�߳�id*/
thread_id_t ut_thread_self(void);

/*��ǰ�߳̽�������״̬*/
void	ut_sleep(int32_t milliseconds);


#endif
