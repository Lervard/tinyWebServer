#include "csapp.h"

ssize_t rio_readn(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	size_t nread;
	char *bufp = (char*)usrbuf;

	while (nleft > 0)
	{
		if ((nread = read(fd, bufp, nleft)) < 0)
		{
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		}else if (nread == 0)
		{
			break;
		}
		nleft -= nread;
		bufp += nread;
	}
	return (n - nleft);
}

ssize_t rio_written(int fd, void *usrbuf, size_t n)
{
	size_t nleft = n;
	size_t nwritten;
	char *bufp = (char*)usrbuf;

	while (nleft > 0)
	{
		if ((nwritten = write(fd, bufp, nleft)) <= 0)
		{
			if (errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		nleft -= nwritten;
		bufp += nwritten;
	}
	return n;
}

void rio_readinitb(rio_t* rp, int fd)
{
	rp->rio_fd = fd;
	rp->rio_cnt = 0;
	rp->rio_bufptr = rp->rio_buf;
}

ssize_t rio_read(rio_t *rp, char* usrbuf, size_t n)
{
	int cnt; 

	/*Refill if buf is empty*/
	while (rp->rio_cnt <= 0)
	{
		rp->rio_cnt = read(rp->rio_fd, rp->rio_buf,
							sizeof(rp->rio_buf));
		if (rp->rio_cnt < 0)
		{
			if (errno != EINTR)
				return -1;
		}
		else if (rp->rio_cnt == 0)
			return 0;
		else 
			rp->rio_bufptr = rp->rio_buf;
	}

	/*Copy min(n, rp->rio_cnt) bytes from internal buf to usr buf*/
	cnt = n;
	if (cnt > rp->rio_cnt)
		cnt = rp->rio_cnt;
	memcpy(usrbuf, rp->rio_bufptr, cnt);
	rp->rio_bufptr += cnt;
	rp->rio_cnt -= cnt;
	return cnt;
}

ssize_t rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen)
{
	int n, rc;
	char c, *bufp = (char*)usrbuf;

	for (n = 1; n < maxlen; n++)
	{
		if ((rc = rio_read(rp, &c, 1)) == 1)
		{
			*bufp++ = c;
			if (c == '\n')
			{
				break;
			}
		}else if (rc == 0) {
			if (n == 1)
				return 0; /*EOF, no data read*/
			else
				break; /*EOF, some data was read*/
		}else
			return -1; /*Error*/
	}
	*bufp = 0;
	return n;
}

ssize_t rio_readnb(rio_t *rp, void *usrbuf, size_t n)
{
	size_t nleft = n;
	size_t nread;
	char *bufp = (char*)usrbuf;

	while (nleft > 0)
	{
		if ((nread = rio_read(rp, bufp, nleft)) < 0)
			if (errno == EINTR)
				nread = 0;
			else
				return -1;
		else if (nread == 0)
			break;
		nleft -= nread;
		bufp += nread;
	}
	return (n - nleft);
}

/*create by cj on 2014.3.1*/
int open_listenfd(int port)
{
	int listenfd, optval = 1;
	struct sockaddr_in serveraddr;

	/*Create a socket descriptor*/
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	
	/*Eliminates* Address aleary in use error from bind*/
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
			      (const void*)&optval, sizeof(int)) < 0)
		return -1;

	printf("listen fd -----------1\n");
	/*Listenfd will be an end point for all requests to port on any
	 * IP address for this host*/
	bzero((char*)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
	if (bind(listenfd, (SA*)&serveraddr, sizeof(serveraddr)) < 0)
		return -1;

	printf("listen fd ---------------2\n");
	/*Make it a listening socket ready to accept connection requests*/
	if (listen(listenfd, LISTENQ) < 0)
		return -1;
	return listenfd;
}

int open_clientfd(char* hostname, int port)
{
	int clientfd;
	struct hostent *hp;
	struct sockaddr_in serveraddr;

	if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1; /*Check errno for cause of error*/
	
	/*Fill in the server's IP address and port*/
	bzero((char*)&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);
	struct in_addr addr;
	if (inet_aton(hostname, &addr) != 0)
	{
		hp = gethostbyaddr((const char*)&addr, sizeof(addr), AF_INET);
	}else{
		if ((hp = gethostbyname(hostname)) == NULL)
			return -2;
	}
	bcopy((char*)hp->h_addr_list[0], (char*)&serveraddr.sin_addr.s_addr, hp->h_length);

	/*Establish a connection with the server*/
	if (connect(clientfd, (SA*) &serveraddr, sizeof(serveraddr)) < 0)
		return -1;
	return clientfd;
}





