
#ifdef WHIRL_DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <strings.h>

#include "whdebug.h"

#define DBGHDR "##DEBUG## "

static char local_format[1024];
static int nodebug = 0;

static void query_debug_environment()
{
	char *ep;
	static int queried = 0;

	if(queried) return;
	if((ep=getenv("WHIRL_DEBUG_PRINT")) && strcasecmp(ep, "true")) nodebug = 1;
	queried = 1;
}

int gprintf(const char *format, ...)
{
	va_list ap;
	int rv;

	query_debug_environment();

	if(nodebug) return 0;

	sprintf(local_format, "%s%s", DBGHDR, format);

	va_start(ap, format);
	rv = vfprintf(stderr, local_format, ap);
	va_end(ap);

	return rv;
}

void suppress_debug()
{
//	gprintf("Suppressing debug output\n");

	nodebug = 1;
}

#else /* WHIRL_DEBUG */

static int whdebug = 0;

#endif /* WHIRL_DEBUG */
