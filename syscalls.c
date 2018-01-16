#include <_ansi.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>

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

/* copy from FreeBSD kernel code */

int
strncmp(const char *s1, const char *s2, size_t n)
{
 
	if (n == 0)
		return (0);
	do {
		if (*s1 != *s2++)
			return (*(const unsigned char *)s1 -
			    *(const unsigned char *)(s2 - 1));
		if (*s1++ == '\0')
			break;
	} while (--n != 0);
	return (0);
}

read_gpio_hw_setting()
{}

dprintf()
{}
