/*
** simple net lib for linux
**
** Copyright (C) 2016 Eleven. See Copyright Notice in net.h
*/

#include <time.h>
#include <sys/time.h>

#include "net_time.h"

int64_t net_gettickcount()
{
	static struct timeval tv_start;
	static struct timeval* tvs = 0;
	if (tvs == 0)
	{
		tvs = &tv_start;
		gettimeofday(tvs, 0);
	}

	struct timeval tv;
	gettimeofday(&tv, 0);
	return ((tv.tv_sec-tv_start.tv_sec+1) * 1000 + tv.tv_usec / 1000);
}