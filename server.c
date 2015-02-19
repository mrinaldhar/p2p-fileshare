#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
	int listenSocket = 0;	// This is my server's socket which is created to 
	struct stat obj;
	char buffer[1024];					
	char filename[1024];	//	listen to incoming connections
	int connectionSocket = 0;

	struct sockaddr_in serv_addr;		// This is for addrport for listening

	// Creating a socket

	listenSocket = socket(AF_INET,SOCK_STREAM,0);
	if(listenSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
		return 0;
	}
	else
		printf("[SERVER] SOCKET ESTABLISHED SUCCESSFULLY\n\n");

	// Its a general practice to make the entries 0 to clear them of malicious entry

	bzero((char *) &serv_addr,sizeof(serv_addr));

	// Binding the socket
	int size;
	int sentsize;
	int portno = 5005;
	serv_addr.sin_family = AF_INET;	//For a remote machine
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portno);
int yes=1;
//char yes='1'; // use this under Solaris

if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    perror("setsockopt");
    return 1;
}
	if(bind(listenSocket,(struct sockaddr * )&serv_addr,sizeof(serv_addr))<0)
		printf("ERROR WHILE BINDING THE SOCKET\n");
	else
		printf("[SERVER] SOCKET BINDED SUCCESSFULLY\n");

	// Listening to connections

	if(listen(listenSocket,10) == -1)	//maximum connections listening to 10
	{
		printf("[SERVER] FAILED TO ESTABLISH LISTENING \n\n");
	}
	printf("[SERVER] Waiting fo client to connect....\n" );

	// Accepting connections
	while(2) {
	while((connectionSocket=accept(listenSocket , (struct sockaddr*)NULL,NULL))<0);

	// NULL will get filled in by the client's sockaddr once a connection is establised

	printf("[CONNECTED]\n");

	bzero(buffer,1024);
	if(recv(connectionSocket,buffer,1023,0)<0)
		printf("ERROR while reading from Client\n");


	printf("Message from Client: %s\n",buffer );
	switch(buffer[0]) {
		case 'd':
			sscanf(buffer+1, "%s", filename);
	FILE *fp = fopen(filename, "rb");

	bzero(buffer,1023);
	printf("Reply to Client: ");
	// fgets(buffer,1023,stdin);
	if (fp>=0) 
	{
		buffer[0]='x';
	send(connectionSocket,buffer,strlen(buffer),0);
}
	  stat(filename, &obj);
	  size = obj.st_size;
	  send(connectionSocket, &size, sizeof(long long int), 0);
	  // printf("Size: %lld\n", *(&size));

	  sentsize = 0;
	while (1)
	{
		size_t newLen = fread(buffer, sizeof(char), 1024, fp);
		if (newLen==0) {
			break;
		}

		send(connectionSocket,buffer,strlen(buffer),0);
	bzero(buffer,1024);

		// printf("%d %d ", sentsize, newLen);
   		// sentsize += static_cast<long long int>(strlen(buffer));
	}	
	break;

	}
	bzero(buffer,1024);
 	
	printf("\nClosing connection\n");
	close(connectionSocket);
}
	close(listenSocket);
	return 0;
}