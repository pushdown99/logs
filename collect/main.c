#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <strings.h>
#include <errno.h>
#include <netdb.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>

#include "conf.h"
#include "sock.h"
#include "log.h"

#define LOOP			1

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define	LOG_COLLECT	0
#define LOG1(f, a...) lprintf(LOG_COLLECT,LOG_0, f, ## a)


int main(int argc, char *argv[])
{
    fd_set fds, rfds;
	int maxfd = 0, fd, lfd;
	int nbyte, slen;
    struct timeval tm;
    DIR *dp = NULL;
	struct dirent *file = NULL;
	char dir[] = "../conf/";
	char buffer[BUFSIZ];
	char f[BUFSIZ], v[BUFSIZ];
	uint16_t port;
    struct sockaddr_in sin;

    FD_ZERO(&rfds);

    if((dp=opendir(dir)) == NULL) {
		fprintf(stderr, "cannot read %s directory", dir);
	}
	while((file = readdir(dp)) != NULL) { 
        if(file->d_type != DT_REG)  continue;

        sprintf(f, "%s%s", dir, file->d_name);
        getconfdelim(f, "sink", ':', 1, v);
	    port = atoi(v);

		if((lfd = udpsock(port)) > 0) {
			FD_SET(lfd, &rfds);
			maxfd = MAX(maxfd, lfd);
		}
	}

	if(mkdir("logs", 0776) == -1 && errno != EEXIST) { 
		fprintf(stderr, "directory create error: %s\n", strerror(errno)); return -1; 
	}
    if (initlog(LOG_COLLECT, "logs", "kaa", "log-collect", LOG_0, LOG_FILE | LOG_CONSOLE, MODE_PATH)<=0) {
        printf("\nLog Init Fail\n");
        exit(0);
    }
	logset(LOG_COLLECT, 5, 4*1024*1024);

    while(LOOP) {
    	tm.tv_sec  = 1; 
		tm.tv_usec = 0;
		fds = rfds;

		if(select(maxfd + 1, &fds, NULL, NULL, &tm) <= 0) {
			continue;
		}

		for(int fd = 0; fd <= maxfd; fd++) {
			if(FD_ISSET(fd,&fds)) {
				bzero(buffer, BUFSIZ);
				if((nbyte = recvfrom(fd, buffer, BUFSIZ, 0, (struct sockaddr*)&sin, &slen)) > 0) {
					//printf("[C] %s", buffer);
					lprintf(LOG_COLLECT, LOG_0, "%s" , buffer);
				}	
			}
		}
	}
}

