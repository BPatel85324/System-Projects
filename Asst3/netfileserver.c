#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

struct hostent *server;

typedef struct DATA{
	char cmd;
	int sockfd;
	char buffer[256];
} DATA;


void *operation(void *d)
{
		
	int sockfd, flags, fildes, nbytes;
	char *file_name, *cliBuf, cmd;
	char buffer[256];
	
	cmd = ((DATA*)d)->cmd;
//	printf("cmd = %c\n",cmd);

	sockfd = ((DATA*)d)->sockfd;
//	printf("sockfd = %d\n",sockfd);
	
	strcpy(buffer,((DATA*)d)->buffer);
//	printf("buffer on entry to operation = %s\n",((DATA*)d)->buffer);
	
	char nerrno[20], nfd[8];
        int n, err, fd;
	while(1){
//		printf("Buffer = %s\n",buffer);
		/* if command is open */
		if(cmd == 'o'){
			printf("operation o\n");
			
			flags = buffer[0];
			file_name = buffer + 2;			
//			printf("file name = %s\n", file_name);
			/* calls open on file, sets file descriptor and errno */
			fd = open(file_name,flags); 
			err = errno;
			if(fd == -1){ /* if open failed */
				printf("Error opening file!\n");
			}
			/* writes new errno back to client */
			sprintf(nerrno,"%d",err);
			n = write(sockfd,nerrno,strlen(nerrno));
			if(n == -1){
				printf("Writing new errno to client failed in open!\n");
			}
			/* writes new file descriptor to client */
			sprintf(nfd,"%d",fd);
			n = write(sockfd,nfd,strlen(nfd));
			if(n == -1){
				printf("Writing new file descriptor to client failed!\n");
			}
			bzero(nerrno,20);
			bzero(nfd,8);
		}
		else if(cmd == 'r'){
			printf("operation r\n");
			
			/* extracts fildes */
			int len,i;
			char *buf = malloc(strlen(buffer) * sizeof(char) + 1);
			for(i = 0; buffer[i] != ' '; i++){
                        	buf[i] = buffer[i];
                	}
			len = strlen(buf);
                	fildes = atoi(buf);
  //              	printf("read fildes = %d\n",fildes);
			bzero(buf,strlen(buf));
			
			/* extracts nbytes from d->buffer */
                	int j = 0;
                	for(i = len+1; i < strlen(buffer); i++){
                        	buf[j] = buffer[i];
                        	j++;
                	}
                	nbytes = atoi(buf);
//			printf("nbytes = %d\n", nbytes);
			
			char *resp = malloc(nbytes * sizeof(char));
			/* calls read with params sent from client */	
			n = read(fildes,resp,nbytes);
			err = errno;
			if(n == -1){
				printf("read failed\n");
			}

			/* writes errno back to client */
			sprintf(nerrno,"%d",err);
                	n = write(sockfd,nerrno,strlen(nerrno));
                	if(n == -1){
                	        printf("Writing new errno to client failed!\n");
				printf("Errno: %s\n", strerror(errno));
                	}
			/* writes back to client the buf that was read from file */
			n = write(sockfd,resp,nbytes);
			if(n == -1){
				printf("Writing the buf back to client failed\n");
			}
			bzero(nerrno,20);
			bzero(nfd,8);
			free(buf);
			free(resp);
		}	
		else if(cmd == 'w'){
			printf("operation w\n");
			char resp[20];
			int c;
			
			/* extracts fildes */
			int len,i;
                        char *buf = malloc(strlen(buffer) * sizeof(char) + 1);
                        for(i = 0; buffer[i] != ' '; i++){
                                buf[i] = buffer[i];
                        }
                        len = strlen(buf);
                        fildes = atoi(buf);
//			printf("fildes in write = %d\n",fildes);
                        bzero(buf,strlen(buf));
			
			/* extracts nbytes */
			int j = 0;
                        for(i = len+1; buffer[i] != ' '; i++){
                                buf[j] = buffer[i];
                                j++;
                        }
                        nbytes = atoi(buf);
//			printf("nbytes in write = %d\n",nbytes);

			cliBuf = buffer + len + strlen(buf) + 2;
//			printf("clibuf = %s\n",cliBuf);

			/* writes to file descriptor given by client */
			c = write(fildes,cliBuf,nbytes);
			err = errno;
			if(c < 0){
				printf("Write failed\n");
			}		
			/* writes errno back to client */
			sprintf(nerrno,"%d",err);
			n = write(sockfd, nerrno,strlen(nerrno));
			if(n < 0){
				printf("Writing errno to client failed\n");
			}
			/* writes back the number of bytes written to file to client */
			sprintf(resp,"%d",c);
//			printf("resp in write = %s\n",resp);
			n = write(sockfd, resp, strlen(resp));
			if(n < 0){
				printf("Couldnt write response back to client\n");
			} 
			bzero(resp,20);
		}	
		else if(cmd == 'c'){
			printf("operation c\n");
			int c;
			char resp[8];
			/* buffer only has the fildes in it */
			fildes = atoi(buffer);
//			printf("fildes = %d\n",fildes);	
			c = close(fildes);
			printf("c = %d\n",c);
			err = errno;
			if(c < 0){
				printf("Close failed\n");
				printf("Errno: %s\n",strerror(errno));
				break;
			}else{
				printf("Close File Success!\n");
			}
			/* sends errno back to client */
			sprintf(nerrno,"%d",err);
			n = write(sockfd, nerrno, strlen(nerrno));
			if(n < 0){
				printf("Failed to write back errno to client\n");
			}
			/* send result of close back to client */
			sprintf(resp, "%d",c);
			n = write(sockfd, resp, strlen(resp));
			if(n < 0){
				printf("Couldnt write response back to client\n");
			}
			printf("Wrote back success to client\n");
		/* if no command and nothing written to buffer, break */
		}
		/* listens for further commands made on same socket */
		n = read(sockfd,&cmd,1);
		bzero(buffer,256);
                n = read(sockfd,buffer,256);
		/* breaks out if nothing is read into buffer */
		if(strlen(buffer) == 0){
			break;
		}
	}
	return NULL;
}

int main(int argc, char **argv)
{
	int option = 1, portNo = 9543, sockfd;
	socklen_t len;

	struct sockaddr_in servaddr, client_addr;
	
	if(argc > 2){
		printf("Syntax : ./server.out\n");
		return 0;
	}
	
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		printf("Error connecting to socket\n");
		return 0;
	}

	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int));

	bzero((char*)&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(portNo);

	if(bind(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) < 0){
		printf("Cannot bind\n");
		return 0;	
	}
	listen(sockfd,5);
	printf("Listening...\n");
		
	len = sizeof(client_addr);

	char cmd, buf[256];
	bzero(buf,256);
	DATA **arr = malloc(sizeof(DATA*));
	int n, connfd, i = 0;
	while(1){

		arr = realloc(arr, (i+1)*sizeof(DATA*));
		DATA *d = malloc(sizeof(DATA));
		/* accepts the new socket from client */
		connfd = accept(sockfd,(struct sockaddr*)&client_addr,&len);
		if(connfd < 0){
			printf("Cannot accept connection.\n");
			return 0;
		}
		d->sockfd = connfd;
		
		printf("Accepted Connection!\n");
		
		/* reads in command from client */
		n = read(connfd,&cmd,1);
                if(n < 0){
                        printf("Reading command failed\n");
                        return 0;
                }
		d->cmd = cmd;

		/* reads in buf from client */
		n = read(connfd,buf,256);
                if(n < 0){
                       	printf("Reading buf failed\n");
                       	return 0;
               	}
		strcpy(d->buffer,buf);
		//printf("buffer on entry = %s\n",d->buffer);	
		
		arr[i] = d;
		
		pthread_t id;	
		pthread_create(&id, NULL, operation, (void*)arr[i]);
		
		i++;
		
		bzero(buf,256);
	}
	return 0;
}
