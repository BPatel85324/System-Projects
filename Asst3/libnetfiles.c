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

int netserverinit(char *hostname)
{
	serverIP = gethostbyname(hostname);
  	if(serverIP == NULL){
    		printf("Error with host\n");
		return -1;
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Error connecting to socket\n");
		return -1;
	}

	bzero((char*) &servaddr,sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(portNo);
	//servaddr.sin_addr.s_addr = INADDR_ANY;	
	
	bcopy((char *)serverIP->h_addr,(char *) &servaddr.sin_addr.s_addr,serverIP->h_length);

	if( connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0 ){
		printf("Error with connect(), %s \n",strerror(errno));
		return -1;
	}
	

//	printf("Connection successful!!\n");
  	return 0;
}

int  netopen(const char *pathname, int flags)
{
	//checks for the correct flags
	if(flags != O_RDONLY && flags != O_WRONLY && flags != O_RDWR){
		printf("Error: flags\n");
		return 0;
	}
 	char buf[256], err[8], fd[8];
	bzero(buf,256);
	sprintf(buf,"%d %s",flags, pathname);	
	
	/* sends the command first */
	checker = write(sockfd,"o",1);
        if(checker < 0){
                printf("write() failed for the command\n");
                return 0;
        }
	/* sends the buf containing flags and pathname second */
	checker = write(sockfd,buf,strlen(buf));
//	printf("buf = %s\n",buf);
        if(checker < 0){
                printf("write() failed for the flags\n");
                return 0;
        }

	checker = read(sockfd,err,8);
	if(checker == -1){
		printf("read() failed on the  new errno\n");
		return 0;
	}
	checker = read(sockfd,fd,8);
	if(checker == -1){
		printf("read() failed on the file descriptor\n");
		return 0;
	}
	if(atoi(fd) == -1){
		printf("Server couldn't open file.\n");
		printf("Errno: %s\n",strerror(atoi(err)));
		return 0;
	}
	/* returns a negative version of the file descriptor that open gives us
 	 * in order to differentiate from actual fd */
	return -(atoi(fd));
}

ssize_t netread(int fildes, void *buf, size_t nbyte)
{
//	printf("in netread");
	char buffer[256];
	char err[20];	
	/* sends command first */
	checker = write(sockfd,"r", 1);
	if(checker < 0){
		printf("write failed on command in netread\n");
		return 0;
	}	
	fildes = -1 * fildes;
//	printf("fildes = %d\n",fildes);

	/* sends buffer containing args second */
	sprintf(buffer,"%d %d",fildes, (int)nbyte);
	checker = write(sockfd,buffer,strlen(buffer));
	if(checker < 0){
		printf("write failed for fildes\n");
		return 0;
	}
//	printf("wrote buffer to server\n");
	/* reads in new errno */
	checker = read(sockfd,err,20);
        if(checker == -1){
                printf("read() failed on the  new errno\n");
                return 0;
        }
//	printf("received new errno\n");
	bzero(buffer,256);
//	printf("nbyte = %d\n",(int)nbyte);

	/* reads in response from server */
	int n = read(sockfd,buffer,(int)nbyte);
	if(n < 0){
		printf("read failed on buffer\n");
		printf("Errno: %s\n",strerror(atoi(err)));
		return 0;
	}
//	printf("Resp from server in netread = %s\n",buffer);

	strncpy((char*)buf, buffer, nbyte);
	return n;
}

int netwrite(int fildes, const void *buf, size_t nbyte)
{
	char buffer[256];
	char err[8];
	char resp[8];
	/* sends command first */
        int checker = write(sockfd,"w", 1);
        if(checker < 0){
                printf("write failed on command in netwrite\n");
                return 0;
        }
	fildes = -1 * fildes;

	/* sends fildes, nbytes, and buf to server */
	sprintf(buffer, "%d %d %s", fildes, (int)nbyte,(char*) buf);
	int n = write(sockfd, buffer, strlen(buffer));
	if(n < 0){
		printf("error writing to server in netopen\n");
	}
	/* reads in new errno */
	n = read(sockfd,err,8);
	if(n < 0){
		printf("read failed on new errno\n");
		return 0;
	}

	/* reads in response from server */
	n = read(sockfd,resp,8);
	if(n < 0){
		printf("reading response from client failed\n");
	}
	n = atoi(resp);
	if(n < 0){
		printf("Write failed\n");
		printf("Errno: %s\n",strerror(atoi(err)));
	}
//	printf("write was success\n");
//	printf("wrote %d bytes\n",n);
	return n;
}

/* possibly done but needs to be tested */
int netclose(int fd)
{
	char buf[256];
	char err[8];
	char resp[8];
	int r;

	/* sends command first */
        int checker = write(sockfd,"c", 1);
        if(checker < 0){
                printf("write failed on command netclose\n");
                return 0;
        }
	
	fd = -1 * fd;
	sprintf(buf,"%d",fd);
	//printf("fd = %s\n",buf);
	/* write the file descripter to close */
	int n = write(sockfd,buf,strlen(buf));
	if(n < 0){
		printf("Write failed in netclose\n");
		return -1;
	}
	/* receives new errno */
	n = read(sockfd,err,8);
	if(n < 0){
		printf("Couldnt read errno from server\n");
		return -1;
	}
	/* reads back whether close was a success or not */
	n = read(sockfd, resp,8);
	if(n < 0){
		printf("Couldnt read response from server\n");
		return -1;
	}
	r = atoi(resp);
	if(r < 0){
		printf("Errno: %s\n",strerror(atoi(err)));
		return -1;
	}
	//printf("File Closed Successfully!\n");
	return 0;
}
/*
int main(int argc, char **argv){
	char fname[100];
	
	if(argc < 3){
                printf("Wrong input format ./client hostaddr(ilab machine that servers on) mode\n");
                exit(0);
        }
	if(netserverinit(argv[1]) == -1){
                printf("Host does not exist!\n");
                return 0;
        }
	printf("O_rdonly = %d\n", O_RDONLY);
	printf("O_wronly = %d\n", O_WRONLY);
	printf("O_rdwr = %d\n", O_RDWR);	

	strcpy(fname, argv[2]);
	
	char buf[20];
	char *buh = "Hello write this plz";
	int nbytes = 20;
	// calls netopen()
        nfd = netopen(fname, O_RDWR);
        if(nfd == 0){
		printf("netopen() returned 0\n");
                return 0;
        }
        printf("Netopen Worked!!\n");
		
	netread(nfd,buf,nbytes);
	printf("Read: %s\n",buf);
	
	netwrite(nfd,buh,20);
	
	netclose(nfd);
	

	return 0;
}
*/






