/*
 * Copyright (c) 2018 Hiroki Mori. All rights reserved.
 */

/* time() use in BearSSL. Reasone of separate this function is conflict by system header. BearSSL use newlib header then undefined error at link. */

unsigned long starttime;

unsigned long
time(unsigned long *t)
{
	return sys_now()/1000 + starttime;
}
