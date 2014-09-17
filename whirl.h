
#ifndef __WHIRL__WHMAIN_H__
#define __WHIRL__WHMAIN_H__

typedef struct wh_option
{
	char *option;
	int def;
	char *parse_file;
	char *title_parse_file;
	char *urlfmt;
	void (*func)();
	char *descr;
	int selected;
	char *parse_string;
	char *title_parse_string;
	FILE *urlfp;
} wh_option;

char *dnl(char  *s);

int raw_output();

#endif /* __WHIRL__WHMAIN_H__ */
