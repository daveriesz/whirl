
#ifndef __WHRIL_DYNAMIC_H__
#define __WHRIL_DYNAMIC_H__

#include <dlfcn.h>

#define WhirlUserDynFcn(xtype, xname, xargs1, xargs2, library) \
  extern "C" xtype xname##__WHIRL_USER_WRAPPER xargs1 \
  { \
    static void *libhandle = NULL; \
    static xtype (*fpFunc_##xname) xargs1 = NULL; \
    if(!libhandle && !(libhandle = dlopen(#library "", RTLD_NOW))) \
      fprintf(stderr, "Could not open library " #library ": %s\n", dlerror()); \
    else if(!fpFunc_##xname && !(fpFunc_##xname = (xtype(*)xargs1)dlsym(libhandle, #xname ""))) \
      fprintf(stderr, "Could not resolve symbol " #xname ": %s\n", dlerror()); \
    else return fpFunc_##xname xargs2; \
  }

#endif /* __WHRIL_DYNAMIC_H__ */

