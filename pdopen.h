/******************************************************************************/
/*                                                                            */
/* Copyright and Disclaimer                                                   */
/*                                                                            */
/* All code listed here is copyright 2005 by David P. Riesz.  You are allowed */
/* to use or redistribute this code under any license you choose.  By using   */
/* this code, you are agreeing to the warranty terms stated below.            */
/*                                                                            */
/* The author makes absolutely no warranty guaranteeing the suitability of    */
/* this code for any purpose whatsoever.  Use it your own risk.               */
/*                                                                            */
/******************************************************************************/

/* pdopen.h */

#ifndef __DPR__PDOPEN_H__
#define __DPR__PDOPEN_H__

#include <stdio.h>

#define EXEC_SHELL "/bin/sh"

FILE *pdopen(const char *cmdstring, FILE *fpin);

#endif /* __DPR__PDOPEN_H__ */
