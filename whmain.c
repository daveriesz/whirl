
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <libgen.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "whirl.h"
#include "whdebug.h"
#include "wherror.h"
#include "whcurl.h"
#include "whparse.h"

void wh_initialize_user_options(int argc, char **argv);
wh_option *wh_add_user_option(char *s_opt, char *s_def, char *s_prs, char *s_ttl, char *s_url, char *s_dsc);
void wh_read_builtin_options(int argc, char **argv);
void wh_read_user_options(int argc, char **argv);
void usage(char *pname, int code);
void wh_fill_parse_buffer(const char *filename, char **buffer);
void wh_read_location(int argc, char **argv);
void wh_load_pages();
void wh_parse_pages();
void wh_cleanup();

void set_raw_output();

static wh_option options_builtin[] = {
#ifdef WHIRL_DEBUG
	{ "-nd"  , 0, NULL, NULL, NULL, suppress_debug, "Suppress debug messages", 0 },
#endif /* WHIRL_DEBUG */
	{ "-raw" , 0, NULL, NULL, NULL, set_raw_output, "Unformatted output"     , 0 },
};
static int options_builtin_count = sizeof(options_builtin)/sizeof(wh_option);

static wh_option *options_user[1024];
static int options_user_count = 0;

static char whirl_rc_directory[1024];

char wh_location_string[2048];

int main(int argc, char **argv)
{
	wh_read_builtin_options(argc, argv);
	wh_initialize_user_options(argc, argv);
	wh_read_user_options(argc, argv);
	wh_read_location(argc, argv);

	if(strlen(wh_location_string) == 0) usage(argv[0], 6);

	wh_load_pages();
	wh_parse_pages();
	wh_cleanup();

	return 0;
}

void wh_initialize_user_options(int argc, char **argv)
{
	char *home, options_file[1024];
	FILE *fp_options;
	char option[1024];
	char *cp;
	char *s_opt, *s_def, *s_prs, *s_ttl, *s_url, *s_dsc;

	sprintf(whirl_rc_directory, WHIRL_RCPATTERN, basename(argv[0]));

	if(home=getenv("HOME"))
		sprintf(options_file, "%s/%s/%s", home, whirl_rc_directory, WHIRL_OPTIONS_FILE);
	else
	{
		fprintf(stderr, "Could not determine HOME directory.\n");
		exit(1);
	}

	gprintf("options file: %s\n", options_file);

	if(!(fp_options = fopen(options_file, "r")))
	{
		fprintf(stderr, "Could not open options file: %s\n", options_file);
		exit(2);
	}

	gprintf("opened options file\n");

	while(dnl(fgets(option, 1024, fp_options)))
	{
		if(*option == '#') continue;
		gprintf("option string: %s\n", option);

		s_opt = option;
		s_def = strchr(s_opt, '|'); *(s_def++)='\0';
		s_prs = strchr(s_def, '|'); *(s_prs++)='\0';
		s_ttl = strchr(s_prs, '|'); *(s_ttl++)='\0';
		s_url = strchr(s_ttl, '|'); *(s_url++)='\0';
		s_dsc = strchr(s_url, '|'); *(s_dsc++)='\0';

		for(cp=strchr(s_opt, '\0')-1 ; cp>s_opt && ((*cp)==' '||(*cp)=='\0') ; (*(cp--))='\0');
		for(cp=strchr(s_def, '\0')-1 ; cp>s_def && ((*cp)==' '||(*cp)=='\0') ; (*(cp--))='\0');
		for(cp=strchr(s_prs, '\0')-1 ; cp>s_prs && ((*cp)==' '||(*cp)=='\0') ; (*(cp--))='\0');
		for(cp=strchr(s_ttl, '\0')-1 ; cp>s_ttl && ((*cp)==' '||(*cp)=='\0') ; (*(cp--))='\0');
		for(cp=strchr(s_url, '\0')-1 ; cp>s_url && ((*cp)==' '||(*cp)=='\0') ; (*(cp--))='\0');
		for(cp=strchr(s_dsc, '\0')-1 ; cp>s_dsc && ((*cp)==' '||(*cp)=='\0') ; (*(cp--))='\0');

		wh_add_user_option(s_opt, s_def, s_prs, s_ttl, s_url, s_dsc);
	}

#ifdef WHIRL_DEBUG
	{
		int i;
		for(i=0 ; i<options_user_count ; i++)
			gprintf("uopt %i: %s, %d, %s, %s, %s, %s\n", i,
				options_user[i]->option,
				options_user[i]->def,
				options_user[i]->parse_file,
				options_user[i]->title_parse_file,
				options_user[i]->urlfmt,
				options_user[i]->descr);
	}
#endif /* WHIRL_DEBUG */		

}

wh_option *wh_add_user_option(
	char *s_opt, char *s_def, char *s_prs,
	char *s_ttl, char *s_url, char *s_dsc)
{
	gprintf("s_opt = >%s<\n", s_opt);
	gprintf("s_def = >%s<\n", s_def);
	gprintf("s_prs = >%s<\n", s_prs);
	gprintf("s_ttl = >%s<\n", s_ttl);
	gprintf("s_url = >%s<\n", s_url);
	gprintf("s_dsc = >%s<\n", s_dsc);

	gprintf("adding option %d\n", options_user_count);

	options_user[options_user_count] = (wh_option *)malloc(sizeof(wh_option));

	options_user[options_user_count]->option             = (char *)malloc(strlen(s_opt)+1); strcpy(options_user[options_user_count]->option          , s_opt);
	options_user[options_user_count]->def                = strcasecmp(s_def, "TRUE")?0:1;
	options_user[options_user_count]->parse_file         = (char *)malloc(strlen(s_prs)+1); strcpy(options_user[options_user_count]->parse_file      , s_prs);
	options_user[options_user_count]->title_parse_file   = (char *)malloc(strlen(s_ttl)+1); strcpy(options_user[options_user_count]->title_parse_file, s_ttl);
	options_user[options_user_count]->urlfmt             = (char *)malloc(strlen(s_url)+1); strcpy(options_user[options_user_count]->urlfmt          , s_url);
	options_user[options_user_count]->func               = NULL;
	options_user[options_user_count]->descr              = (char *)malloc(strlen(s_dsc)+1); strcpy(options_user[options_user_count]->descr           , s_dsc);
	options_user[options_user_count]->selected           = 0;
	options_user[options_user_count]->parse_string       = NULL;
	options_user[options_user_count]->title_parse_string = NULL;
	options_user[options_user_count]->urlfp              = NULL;

	options_user_count++;
}

char *dnl(char *s)
{
	char *dnl_cp;
	if(s && (dnl_cp = strchr(s, '\n'))) *dnl_cp = '\0';
	if(s && (dnl_cp = strchr(s, 0x0d))) *dnl_cp = '\0';
	return s;
}

static int doraw = 0;
void set_raw_output() { gprintf("Setting output to raw\n"); doraw = 1; }
int  raw_output    () { return doraw; }

void wh_read_builtin_options(int argc, char **argv)
{
	int i, j;

	for(j=0 ; j<options_builtin_count ; j++)
	{
		for(i=0 ; i<argc ; i++)
		{
			if(!strcmp(options_builtin[j].option, argv[i]))
			{
				if(options_builtin[j].func) options_builtin[j].func();
				*(argv[i]) = '\0';
			}
		}
	}
}

void usage(char *pname, int code)
{
	int i;

	fprintf(stderr, "usage: %s [options] location\n", basename(pname));
	fprintf(stderr, "    options:\n");

	for(i=0 ; i<options_user_count ; i++)
		fprintf(stderr, "        %-10s %s\n", options_user[i]->option, options_user[i]->descr);
	for(i=0 ; i<options_builtin_count ; i++)
		fprintf(stderr, "        %-10s %s\n", options_builtin[i].option, options_builtin[i].descr);

	if(code != 0) exit(code);
}

void wh_read_user_options(int argc, char **argv)
{
	int i, j;
	int selected = 0;

	for(j=0 ; j<options_user_count ; j++)
	{
		for(i=0 ; i<argc ; i++)
		{
			if(!strcmp(options_user[j]->option, argv[i]))
			{
				if(options_user[j]->selected)
				{
					gprintf("OPTION RE-SELECTED: %s\n", options_user[j]->option);
					*(argv[i]) = '\0';
					continue;
				}

				gprintf("PROCESSING SELECTED OPTION: %s\n", options_user[j]->option);

				*(argv[i]) = '\0';
				wh_fill_parse_buffer(options_user[j]->      parse_file, &(options_user[j]->      parse_string));
				wh_fill_parse_buffer(options_user[j]->title_parse_file, &(options_user[j]->title_parse_string));


				options_user[j]->selected = 1;
				selected++;
			}
		}
	}

	if(!selected)
	{
		for(i=0 ; i<options_user_count ; i++)
		{
			if(options_user[i]->selected = options_user[i]->def)
			{
				gprintf("PROCESSING DEFAULT OPTION: %s\n", options_user[i]->option);
				wh_fill_parse_buffer(options_user[i]->      parse_file, &(options_user[i]->      parse_string));
				wh_fill_parse_buffer(options_user[i]->title_parse_file, &(options_user[i]->title_parse_string));

				selected++;
			}
		}
	}
}

void wh_fill_parse_buffer(const char *filename, char **buffer)
{
	char fn[1024];
	struct stat stbuf;
	int fd;

	gprintf("wh_fill_parse_buffer: %s\n", filename);

	sprintf(fn, "%s/%s/%s", getenv("HOME"), whirl_rc_directory, filename);
	gprintf("option stat(%s)\n",  filename);
	if(stat(fn, &stbuf))     wh_error(3, "Could not stat %s: %s\n", fn, strerror(errno));

	gprintf("option creating buffer of %d bytes\n", stbuf.st_size + 1);
	(*buffer) = (char *)malloc(stbuf.st_size + 1);

	gprintf("option open(%s)\n", filename);
	if((fd=open(fn, O_RDONLY))<0) wh_error(4, "Could not open %s: %s\n", fn, strerror(errno));

	gprintf("option reading %d bytes into buffer\n", stbuf.st_size);
	if(read(fd, (*buffer), stbuf.st_size) != stbuf.st_size)
		wh_error(5, "Error while reading %s: %s\n", fn, strerror(errno));
	(*buffer)[stbuf.st_size] = '\0';

	close(fd);
}

void wh_read_location(int argc, char **argv)
{
	int i;

	bzero(wh_location_string, 2048);

	for(i=1 ; i<argc ; i++)
	{
		if(*(argv[i]) == '\0') continue;
		else if(*(wh_location_string) == '\0')
		{
			strcpy(wh_location_string, argv[i]);
		}
		else
		{
			strcat(wh_location_string, "+");
			strcat(wh_location_string, argv[i]);
		}
	}

	gprintf("location: %s\n", wh_location_string);
}

void wh_load_pages()
{
	int i, j;
	char url[1024];

	for(i=0 ; i<options_user_count ; i++)
	{
		if(options_user[i]->selected)
		{
			for(j=0 ; j<options_user_count ; j++)
			{
				if((j != i) && !(strcmp(options_user[j]->urlfmt, options_user[i]->urlfmt)) && options_user[j]->urlfp)
				{
					gprintf("url for option %s already loaded by option %s\n",
						options_user[i]->option,
						options_user[j]->option);
					options_user[i]->urlfp = options_user[j]->urlfp;
					j = options_user_count;
				}
			}
			if(!(options_user[i]->urlfp))
			{
				sprintf(url, options_user[i]->urlfmt, wh_location_string);
				options_user[i]->urlfp = wh_fetch_url(url);
			}
		}
	}
}

void wh_parse_pages()
{
	int i;

	for(i=0 ; i<options_user_count ; i++)
	{
		if(options_user[i]->selected)
		{
			wh_parse(options_user[i]->urlfp, options_user[i]->title_parse_string);
			wh_parse(options_user[i]->urlfp, options_user[i]->      parse_string);
		}
	}
}


void wh_cleanup()
{
	int i;

	gprintf("CLEANUP\n");

	for(i=0 ; i<options_user_count ; i++)
	{
		if(options_user[i]->urlfp) fclose(options_user[i]->urlfp);
	}
}
