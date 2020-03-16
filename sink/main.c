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


int main(int argc, char *argv[])
{
	char f[BUFSIZ] = "/opt/radix/sink/log-sink";
    char s[BUFSIZ], h[BUFSIZ], v[BUFSIZ];
    struct sockaddr_in sin;
    char *host;
    uint16_t port;
    struct hostent *hent;

    if(argc < 2) {
		printf("Usage: %s message\n\n", argv[0]);
		exit(0);
	}
    getconfdelim(f, "source", ':', 0, h);
    getconfdelim(f, "source", ':', 1, v);

	printf("%s:%s\n", h, v);

    hent = gethostbyname(h);
    host = inet_ntoa(*((struct in_addr*)hent->h_addr_list[0]));
	port = atoi(v);

    int fd = udpsock(0);
    bzero((char*)&sin,sizeof(sin));
    sin.sin_family      = AF_INET;
    sin.sin_addr.s_addr = inet_addr(host);
    sin.sin_port        = htons(port);
    dump(&sin, sizeof(sin));
	int nbyte = sendto(fd, argv[1], strlen(argv[1]), 0, (struct sockaddr*)&sin, sizeof(sin));
	printf("%d bytes sent (%s)\n\n", nbyte, argv[1]);
	close(fd);
}

