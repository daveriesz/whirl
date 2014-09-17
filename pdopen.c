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

/* pdopen.c */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#include "pdopen.h"

FILE *pdopen(const char *cmdstring, FILE *fpin)
{
    int fdout[2];
    pid_t pid;
    FILE *fpout;

/* If we fail either to create a pipe or to fork the process, */
/* an error has occurred and we must return.                  */

/* Create the pipe. */
    if(pipe(fdout))        { fprintf(stderr, "pipe error: %s\n", strerror(errno)); return NULL; }

/* Fork the process. */
    if((pid = fork()) < 0) { fprintf(stderr, "fork error: %s\n", strerror(errno)); return NULL; }

/* The parent process closes the end of the pipe to which the child will     */
/* write.  It then creates a FILE pointer for the pipe's output file         */
/* descriptor.  We must duplicate the file descriptor with dup() because...? */

    if(pid > 0) /* parent */
    {
        close(fdout[1]);                       /* child writes to this end */
        fpout = fdopen((dup(fdout[0])), "r");  /* fpout is a friendly FILE* */
    }

/* The child process fist closes the end of the pipe from which the parent */
/* reads.  Then it uses dup2() to pipe everything from fpin into STDIN and */
/* to pipe everything from STDOUT back into the pipe that the parent is    */
/* reading.  Finally, execl() executes the requested command with the      */
/* STDIN and STDOUT redirects.  execl() is used simply because it is the   */
/* simplest of all the exec*() calls.                                      */

    else /* child */
    {
        close(fdout[0]);                  /* parent reads from this end */
        dup2(fileno(fpin), STDIN_FILENO); /* fpin becomes STDIN */
        dup2(fdout[1], STDOUT_FILENO);    /* fdout receives STDOUT */
        execl(EXEC_SHELL, "sh", "-c", cmdstring, NULL); /* exec the command */

        /* execl() replaces the fork's process image with that created by */
        /* execl().  If the image isn't replaced and the original fork    */
        /* image still exists, an error has occurred.                     */

        fprintf(stderr, "error: execl \"%s\" failed (%s)\n", EXEC_SHELL, strerror(errno));
        exit(1);
    }

/* fpout now points to a stream from the subcommand we just executed. */

    return fpout;
}

