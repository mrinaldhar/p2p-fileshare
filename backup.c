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
#include <pthread.h>

int listenSocket = 0;
int connectionSocket = 0;
char buffer[1024];
struct sockaddr_in s_serv_addr;
int portno = 5005;

void initServer() {
	char filename[50], filesize[20];
	char fbuffer[1024];
	struct stat obj;
	listenSocket = socket(AF_INET,SOCK_STREAM,0);
	if(listenSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
	}
	else
		printf("[SERVER] SOCKET ESTABLISHED SUCCESSFULLY\n\n");

	// Its a general practice to make the entries 0 to clear them of malicious entry

	bzero((char *) &s_serv_addr,sizeof(s_serv_addr));
	

	s_serv_addr.sin_family = AF_INET;	//For a remote machine
	s_serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_serv_addr.sin_port = htons(portno);

	int yes=1;
//char yes='1'; // use this under Solaris

if (setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
    perror("setsockopt");
}


if(bind(listenSocket,(struct sockaddr * )&s_serv_addr,sizeof(s_serv_addr))<0)
		printf("ERROR WHILE BINDING THE SOCKET\n");
	else
		printf("[SERVER] SOCKET BINDED SUCCESSFULLY\n");

	// Listening to connections

	if(listen(listenSocket,10) == -1)	//maximum connections listening to 10
	{
		printf("[SERVER] FAILED TO ESTABLISH LISTENING \n\n");
	}
	printf("[SERVER] Waiting for client to connect....\n" );


while((connectionSocket=accept(listenSocket , (struct sockaddr*)NULL,NULL))<0);


	printf("[CONNECTED]\n");

	bzero(buffer,1024);
	if(recv(connectionSocket,buffer,1024,0)<0)
		printf("ERROR while reading from Client\n");


	printf("Message from Client: %s\n",buffer );
	
	switch(buffer[0]) {
		case 'd':
	
	sscanf(buffer+1, "%s", filename);
	FILE *fp = fopen(filename, "rb");

	bzero(buffer,1024);


	if (fp>=0) 
	{
		buffer[0]='x';
		stat(filename, &obj);
		long long int size = (long long) obj.st_size;
		sprintf(buffer+1, "%lld", size);
		send(connectionSocket,buffer,1024,0);
	}
	printf("The size of file is: %s\n", buffer);

	while (fread(buffer, sizeof(char), 1024, fp))
	{
		send(connectionSocket,buffer,1024,0);
		bzero(buffer,1024);
	}	


}

	close(connectionSocket);
	initServer();
}

int initClient(char * filename) {
	// char * filename = (char *) fileName;
	int client_pid = fork();
	if(client_pid != 0)
        return client_pid;
	printf("Downloading %s\n", filename);
char returncode[1024];
	int ClientSocket = 0;
	int ret;
	int i;
	long long int fileSize;
	struct sockaddr_in c_serv_addr;

	ClientSocket = socket(AF_INET,SOCK_STREAM,0);
	if(ClientSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
		// return 0;
	}
	else
		printf("[CLIENT] Socket created \n");



	c_serv_addr.sin_family = AF_INET;
	c_serv_addr.sin_port = htons(portno);
	c_serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");


while(connect(ClientSocket,(struct sockaddr *)&c_serv_addr,sizeof(c_serv_addr))<0);

	bzero(buffer,1024);
	sscanf(filename, "%s", buffer);

	if(send(ClientSocket,buffer,strlen(buffer),0)<0)
		printf("ERROR while writing to the socket\n");
	bzero(buffer,1024);

	FILE *fp = fopen("newfile", "wb");
	if(recv(ClientSocket,buffer,1024,0)<0)
		printf("ERROR while reading from the socket\n");
 	sscanf(buffer, "%s", returncode);
	if(returncode[0]=='x') {
		i=1;
		fileSize = 0;
		while(returncode[i]!='\0') {
        fileSize *= 10;
        fileSize += returncode[i++]-'0';
    	}
    	printf("The filesize is %lld\n", fileSize);
		bzero(buffer,1024);
		ret = 1;
		while (2) {
			if(!ret)
            break;
			bzero(buffer,1024);
			ret=recv(ClientSocket,buffer,1024,0);
			fwrite(buffer, sizeof(char),strlen(buffer), fp);
			}
		bzero(buffer,1024);
		fclose(fp);
	}
	printf("File download complete.\n");
	printf("Closing Connection\n");
	close(ClientSocket);
	return 1;

}
int main(int argc, char *argv[])
{
	pthread_t tid, tid2;
	int serv_pid = 0;
	serv_pid = fork();
	if (serv_pid != 0) {
		initServer();
		// pthread_create(&tid, NULL, initServer, NULL);
    	// pthread_join(tid, NULL);
        close(listenSocket);

	}
	char filename[50];
	while (1) {
		printf("Enter filename: ");
		bzero(filename, 50);
		fgets(filename+1,49,stdin);
		filename[0] = 'd';
		// pthread_create(&tid2, NULL, initClient, (void *)filename);
    	// pthread_join(tid2, NULL);
		if(!initClient(filename))
                break;
	}
	printf("\nClosing connection2\n");
	
	return 0;
}