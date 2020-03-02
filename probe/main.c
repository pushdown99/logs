#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <netdb.h> 
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/inotify.h>
#include <time.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <pthread.h>

#include "conf.h"
#include "sock.h"

#define EVENT_SIZE    	(sizeof (struct inotify_event))
#define EVENT_BUF_LEN 	(1024 * (EVENT_SIZE + 16))
#define LOOP			1

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void*
probe_task (void* args)
{
    fd_set fds, rfds;

	int length;
	int maxfd = 0;
    FILE* fp;
	int fd, wd, id;
	char buffer[EVENT_BUF_LEN], line[BUFSIZ];
	char f[BUFSIZ], s[BUFSIZ], h[BUFSIZ], v[BUFSIZ];
    char *host;
	uint16_t port;
    char command[BUFSIZ];
    struct sockaddr_in sin;
    struct timeval tm;
    struct hostent *hent; 

    strcpy(f, (char*)args);

    getconf(f, "source", s);

    getconfdelim(f, "sink", ':', 0, h);
    getconfdelim(f, "sink", ':', 1, v);
	port = atoi(v);

	hent = gethostbyname(h); 
	host = inet_ntoa(*((struct in_addr*)hent->h_addr_list[0])); 

	printf("%s: %s %s(%s):%d\n", f, s, h, host, port);

    fd = udpsock(0);
    bzero((char*)&sin,sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = inet_addr(host);
    sin.sin_port        = htons(port);

    sprintf(command, "/usr/bin/tail -n 1 -f %s", s);
    printf("%s\n", command);
	if((fp = popen(command, "r")) == NULL) {
		return NULL;
	}

	if((id = inotify_init1(IN_CLOEXEC)) < 0) {
		perror("inotify_init error");
	}
	//wd = inotify_add_watch(id, s, IN_MODIFY);  // IN_CREATE | IN_DELETE | IN_ACCESS | IN_MODIFY | IN_OPEN
	wd = inotify_add_watch(id, s, IN_CREATE | IN_DELETE | IN_ACCESS | IN_MODIFY | IN_OPEN);

    FD_ZERO(&rfds);
    FD_SET (id, &rfds);
	maxfd = MAX(maxfd, id);

    while(LOOP) {
    	tm.tv_sec  = 1; 
		tm.tv_usec = 0;
		fds = rfds;

		if(select(maxfd + 1, &fds, NULL, NULL, &tm) <= 0) {
			printf("timeout %s\n", f);
			continue;
		}

		if(FD_ISSET(id, &fds)) {
			if((length = read(id, buffer, EVENT_BUF_LEN)) < 0) {
				perror( "read" );
				continue;
			}
			while(fgets(line, BUFSIZ, fp) != NULL)  {
				sendto(fd, line, strlen(line), 0, (struct sockaddr*)&sin, sizeof(sin));
				printf("%s", line);
			}
		}
    }
}

#define MAX_THREAD		16

pthread_t t_id[MAX_THREAD];
int num_of_pthread = 0;

int main(int argc, char *argv[])
{
    DIR *dp = NULL;
	struct dirent *file = NULL;
	void* resp;
	char dir[] = "../conf/";
	char f[BUFSIZ];

    if((dp=opendir(dir)) == NULL) {
		fprintf(stderr, "cannot read %s directory", dir);
	}
	while((file = readdir(dp)) != NULL) { 
        if(file->d_type != DT_REG)  continue;

        sprintf(f, "%s%s", dir, file->d_name);
	    pthread_create(&t_id[num_of_pthread], NULL, probe_task, f);
		num_of_pthread += 1;
		sleep(1);
	}

    while(LOOP) {
		sleep(1);
	}
}

