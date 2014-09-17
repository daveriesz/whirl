
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "wherror.h"

int wh_error(int code, const char *format, ...)
{
	va_list ap;
	int rv;

	va_start(ap, format);
	rv = vfprintf(stderr, format, ap);
	va_end(ap);

	exit(code);
}

