/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/


#include <stdlib.h>
#include <malloc.h>
#include <memory.h>


#include "net_memory.h"


void*	netmm_new(int32_t size)
{
	void* p = malloc(size);
	memset(p, 0, size);
	return p;
}

void	netmm_delete(void* ptr)
{
	free(ptr);
}