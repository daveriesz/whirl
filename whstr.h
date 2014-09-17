
#ifndef __WHIRL__WHSTR_H__
#define __WHIRL__WHSTR_H__

typedef struct wh_str_elem {
	char *base;
	struct wh_str_elem *next;
} wh_str_elem;

typedef struct wh_str {
	wh_str_elem *root;
	struct wh_str *next;
} wh_str;

wh_str *wh_str_new   (                     );
wh_str *wh_str_delete(wh_str *ws           );
int     wh_str_add   (wh_str *ws, char *str);
int     wh_str_print (wh_str *ws           );

#endif /* __WHIRL__WHSTR_H__ */
