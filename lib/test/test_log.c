#include <stdio.h>

#include "log.h"

#define TEST_LOG0   0
#define TEST_LOG1   1

int main()
{
	int loop;
	char date[BUFSIZ];
	char t[BUFSIZ];
	if (initlog(TEST_LOG0, "/home/haeyun/_logs/lib/test/logs", "ems", "logtest1", LOG_0, LOG_FILE|LOG_CONSOLE, MODE_PATH)<=0) {
		printf("\nLog Init Fail\n");
		exit(0);
	}
	logset(TEST_LOG0, 5, 4*1024*1024);

	for(loop=0;loop<100000;loop++) {
		//logprn(TEST_LOG0, LOG_0,0,"Log0- Testing (%s:%d)\r\n" ,__FILE__,__LINE__);
		lprintf(TEST_LOG0, LOG_0, "Log1- Testing (%s:%d)\r\n",__FILE__,__LINE__);
    }
}
