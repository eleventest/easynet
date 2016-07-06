/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/



#ifndef _NET_MEMORY_H_
#define _NET_MEMORY_H_

#include <inttypes.h>

/*…Í«Îƒ⁄¥Ê*/
void*	netmm_new(int32_t size);
/* Õ∑≈ƒ⁄¥Ê*/
void	netmm_delete(void* ptr);

#define NET_NEW(T)					(T*)netmm_new(sizeof(T))
#define NET_NEW_ARRAY(T, num)		(T*)netmm_new(sizeof(T)*num)
#define NET_DELETE(PTR)				{netmm_delete((void*)PTR);PTR=0;}


#endif