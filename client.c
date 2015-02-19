#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
	int ClientSocket = 0;
	struct sockaddr_in serv_addr;
	int rcvdsize;
	int totalsize;
	char buffer[1024];
	char filename[20];
	char returncode[1024];
	char command[1024];
	// Creating a socket

	ClientSocket = socket(AF_INET,SOCK_STREAM,0);
	if(ClientSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
		return 0;
	}
	else
		printf("[CLIENT] Socket created \n");

	int portno = 5005;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//Connection Establishment
// size_t size;
	while(connect(ClientSocket,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0);

	bzero(buffer,1024);
	printf("\nEnter Filename: ");
	fgets(buffer+1,1023,stdin);
 	sscanf(buffer, "%s", filename);

	buffer[0]='d';
	if(send(ClientSocket,buffer,strlen(buffer),0)<0)
		printf("ERROR while writing to the socket\n");
	bzero(buffer,1024);

	FILE *fp = fopen("newfile", "wb");
	if(recv(ClientSocket,buffer,1023,0)<0)
		printf("ERROR while reading from the socket\n");
 	sscanf(buffer, "%s", returncode);
	if(returncode[0]=='x') {
		// recv(ClientSocket, &totalsize, sizeof(int), 0);
		while (2) {
			size_t size = recv(ClientSocket,buffer,1024,0);
			if (size==0) {
			break;
		}
			fwrite(buffer, sizeof(char), size, fp);
	bzero(buffer,1024);

			// rcvdsize += strlen(buffer);
		}
		fclose(fp);
	}
	printf("File download complete.\n");

	printf("Closing Connection\n");
	close(ClientSocket);
	return 0;
}