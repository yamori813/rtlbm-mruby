#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>

extern char _end[];

int kill()
{
	return 0;
}

int getpid()
{
	return 0;
}

int _exit()
{
	return 0;
}

void *
_sbrk (incr)
     int incr;
{
   static char * heap_end = _end;
   char *        prev_heap_end;

   prev_heap_end = heap_end;
   heap_end += incr;

   return (void *) prev_heap_end;
}
char * sbrk (int) __attribute__((weak, alias ("_sbrk")));

int sys_now()
{
	return 0;
}

int strncmp()
{
	return 0;
}
