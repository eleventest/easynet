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

/* 线程执行函数体.*/
typedef void*(*ut_thread_func_t)(void*);

/*创建一个线程执行体*/
ut_thread_t*	ut_thread_new(ut_thread_func_t thread_func, void* arg);

thread_handle	ut_thread_handle(ut_thread_t* thread);

/*删除一个线程*/
void	ut_thread_delete(ut_thread_t* a_thread);

/*等待一个线程的结束*/
void	ut_thread_join(ut_thread_t* a_thread);

/*获取当前线程id*/
thread_id_t ut_thread_self(void);

/*当前线程进入休眠状态*/
void	ut_sleep(int32_t milliseconds);


#endif
