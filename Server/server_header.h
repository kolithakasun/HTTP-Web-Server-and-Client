#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

#define	MAXLINE	100000
#define	LISTENQ	1024
#define	SERVER_PORT	8080  // replace this with your assigned port number
#define	SERV_TCP_PORT	8080
#define	SA	struct sockaddr