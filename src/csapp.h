#ifndef CSAPP
#define CSAPP

#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <errno.h>
#include <memory.h>

#define RIO_BUFSIZE 8192
typedef struct{
	int rio_fd;
	int rio_cnt;
	char *rio_bufptr;
	char rio_buf[RIO_BUFSIZE];
} rio_t;

typedef struct sockaddr SA;

void rio_readinitb(rio_t *rp, int fd);
ssize_t rio_readn(int fd, void *usrbuf, size_t n);
ssize_t rio_writen(int fd, void *usrbuf, size_t n);

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen);
ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n);

#define LISTENQ 1024
#define MAXLINE 128
#define MAXBUF 1024
typedef struct sockaddr SA;
int open_listenfd(int port);
int open_clientfd(char* hostname, int port);

#endif
