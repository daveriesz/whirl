
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>

#include "whirl.h"
#include "whstr.h"
#include "whdebug.h"
#include "wherror.h"

static int wh_terminal_width()
{
	static int width = 0;
	static char *wenv;
	struct winsize wsz;

	if(width > 0) return width;
	
	if(wenv=getenv("WHIRL_SCREEN_WIDTH"))
		if((width=atoi(wenv))>0)
			return width;

	if(ioctl(STDIN_FILENO, TIOCGWINSZ, (char *)&wsz) < 0)
	{
		gprintf("wh_terminal_width: ioctl() failed: %s\n", strerror(errno));
		width = 80;
	}
	else
	{
		width = wsz.ws_col;
	}
	
	gprintf("terminal width: %d\n", width);

	return width;
}

static void wh_str_format_print(wh_str *ws)
{
	wh_str_elem *wp;
	static char *cpa[1024], *cp1, *cp2;
	static int   len[  32], i, ip=0, l, n, lp, lq, lr;
	static char  fmt[32][32], fmtx[128];

	wh_terminal_width();

	bzero(cpa, 1024 * sizeof(char *));
	bzero(len,   32 * sizeof(int   ));

	for(wp=ws->root, n=0 ; wp ; wp=wp->next)
	{
		for(cp1=strchr(wp->base, '{'), i=0 ; cp1 ; cp1=strchr(cp1, '{'), i++, n++)
		{
			*(cp1++) = '\0';
			cpa[n] = cp1;
		}
		if(wp!=ws->root && i!=ip)
			wh_error(10, "unequal number of string breaks\n");
		ip = i;
		for(i=0 ; i<ip ; i++) len[i] = ((l=strlen(cpa[n-ip+i]))>len[i]) ? l : len[i];
	}

	if(ip == 0) for(wp=ws->root ; wp ; wp=wp->next) printf("%s\n", wp->base);
	else
	{
		for(i=0, l=0 ; i<ip-1 ; i++)
		{
			sprintf(fmt[i], "%%-%ds ", len[i]);
			if(i < ip-1) l += (len[i]+1);
		}
		sprintf(fmt[i], "%%-%ds", l);
		for(wp=ws->root, n=0 ; wp ; wp=wp->next)
		{
			for(i=0 ; i<ip-1 ; i++, n++)
			{
				printf(fmt[i], cpa[n]);
			}

			lp = strlen(cpa[n]);
			lq = ((wh_terminal_width() - l) - 1);

			for(cp1=cpa[n] ; (lr=strlen(cp1)) > lq ; cp1=cp2+1)
			{
				if(cp1>cpa[n]) printf(fmt[i], " ");
				for(cp2=cp1+lq-1 ; *cp2 != ' ' ; cp2--);
				*cp2 = '\0';
				printf("%s", cp1);
				printf("\n");
			}
			if(cp1>cpa[n]) printf(fmt[i], " ");
			printf("%s", cp1);

			n++;
			printf("\n");
		}
	}

}



wh_str *wh_str_new()
{
	wh_str *ws = (wh_str *)malloc(sizeof(wh_str));
	ws->root = NULL;
	ws->next = NULL;
	return ws;
}

int wh_str_add(wh_str *ws, char *str)
{
	wh_str_elem **wpp;

	for(wpp=&(ws->root) ; *wpp ; wpp=&((*wpp)->next));
	*wpp = (wh_str_elem *)malloc(sizeof(wh_str_elem));

	(*wpp)->base = (char *)malloc(strlen(str) + 1); strcpy((*wpp)->base, str);
	(*wpp)->next = NULL;

	return 0;
}

int wh_str_print(wh_str *ws)
{

	wh_str_elem *wp;

	if(raw_output())
	{
		for(wp=ws->root ; wp ; wp=wp->next)
		{
			printf("%s\n", wp->base);
		}
	}
	else
	{
		wh_str_format_print(ws);
	}

	return 0;
}

wh_str *wh_str_delete(wh_str *ws)
{
	wh_str_elem *wp1, *wp2;

	for(wp1=ws->root ; wp1 ; wp1=wp2)
	{
		wp2=wp1->next;
		free(wp1->base);
		free(wp1);
	}

	return NULL;
}

