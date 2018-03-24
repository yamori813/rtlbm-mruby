/* time() use in BearSSL. Reasone of separate this function is conflict by system header. BearSSL use newlib header then undefined error at link. */

unsigned long
time(unsigned long *t)
{
	return sys_now()/1000 + 1521528757UL;
}
