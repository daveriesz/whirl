
#ifndef __WHIRL__WHDEBUG_H__
#define __WHIRL__WHDEBUG_H__

#ifdef WH_DEBUG

int gprintf(const char *format, ...);

void suppress_debug();

#else /* WH_DEBUG */

#define gprintf

#endif /* WH_DEBUG */

#endif /* __WHIRL__WHDEBUG_H__ */
