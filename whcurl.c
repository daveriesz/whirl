
#include <curl/curl.h>

#include "whdebug.h"

char location_url[16384];

FILE *wh_fetch_url(const char *url)
{
	CURL *crl;
	FILE *fp = tmpfile();

	gprintf("wh_fetch_url: URL: %s\n", url);

	crl = curl_easy_init();

	curl_easy_setopt(crl, CURLOPT_URL, url);
	curl_easy_setopt(crl, CURLOPT_FILE, fp);

	curl_easy_perform(crl);

	curl_easy_cleanup(crl);

	return fp;
}

