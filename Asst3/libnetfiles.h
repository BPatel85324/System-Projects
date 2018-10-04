#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

char host[100];
struct hostent *serverIP;
struct sockaddr_in servaddr;
int portNo = 9543, nfd,sockfd,checker = -1;

int netserverinit(char *hostname);
int  netopen(const char *pathname, int flags);
ssize_t netread(int fildes, void *buf, size_t nbyte);
int netwrite(int fildes, const void *buf, size_t nbyte);
int netclose(int fd);
