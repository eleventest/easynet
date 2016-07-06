/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/




#include "net_thread.h"
#include "net_memory.h"

#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

struct ut_thread
{
	ut_thread_func_t	thread_func;
	void*				arg;
	pthread_t			handle;
};

static void* inner_thread_func_t(void* arg)
{
	ut_thread_t*	a_thread = (ut_thread_t*)arg;
	
	return (*a_thread->thread_func)(a_thread->arg);
}

ut_thread_t*	ut_thread_new(ut_thread_func_t thread_func, void* arg)
{
	ut_thread_t*	a_thread = NET_NEW(ut_thread_t);
	if (0 == a_thread)
	{
		return 0;
	}	

	a_thread->thread_func = thread_func;
	a_thread->arg = arg;

	if(pthread_create(&a_thread->handle, NULL, inner_thread_func_t, a_thread) != 0)
	{
		ut_thread_delete(a_thread);
		a_thread = 0;
	}

	return a_thread;
}

void	ut_thread_delete(ut_thread_t* a_thread)
{
	pthread_cancel(a_thread->handle);
	NET_DELETE(a_thread);
}

void	ut_thread_join(ut_thread_t* a_thread)
{
	pthread_join(a_thread->handle, 0);
}

unsigned long ut_thread_self(void) 
{
	return (unsigned long) pthread_self();
}

void	ut_sleep(int32_t milliseconds)
{
	usleep(milliseconds*1000);
}
