
#include <stdio.h>

#include "whirl.h"
#include "whparse.h"
#include "whdebug.h"
#include "wherror.h"
#include "whstr.h"
#include "pdopen.h"

void wh_parse(FILE *fpin, const char *pstr)
{
	FILE *pfp;
	char s[16384];
	wh_str *ws = NULL;

	rewind(fpin);

	pfp = pdopen(pstr, fpin);

	ws = wh_str_new();
	while(pfp && dnl(fgets(s, 16384, pfp)))
	{
		wh_str_add(ws, s);
	}
	wh_str_print(ws);

	ws = wh_str_delete(ws);

	fclose(pfp);
}
