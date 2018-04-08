#include <am.h>
#include <amdev.h>
#include <ylib.h>

#ifdef DEBUG
void *memset(void *arr, int val, size_t n)
{
	for (int = 0; i < n; i++)
		arr[i] = 0;
	return arr;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	for (int i = 0; i < n; i++)
		dest[i] = src[i];
	return dest;
}

size_t strlen(const char *s)
{
	size_t i;
	for (i = 0; ;i++)
		if (s[i] == 0)
			break;
	return i;
}

char *strcpy(char *dest, const char *src)
{
	return (char *)memcpy(dest, src, strlen(src));
}

int strcmp(const char str_1, const char str_2)
{
	int i;
	for (i = 0; ; i++)
	{
		if (i == strlen(str_1))
			break;
		if (i == strlen(str_2))
			break;

		if (!( str_2[i] - str_1[i]))
			return str_2[i] - str_1[i];
	}
	return 0;
}

int printf(const char *fmt, ...)
{
	char *ap;
	ap = ((char *)&fmt) + sizeof(fmt);
	char *ic;
	for (ic = fmt;;)
	{
		if (!*ic)
			break;
		if (*ic == '%')
		{
			char *icc = ic;
			while (1)
			{
				icc++;
				if ()
			}
		}
			
	}
}

#endif


