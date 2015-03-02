#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "indexGet.c"
#include "udpFileTransfer.c"

int listenSocket = 0;
int connectionSocket = 0;
char buffer[1024];
struct sockaddr_in s_serv_addr;
int portno = 5005;

void parse(char cmd[], char ** vals) {
	int i, j, k;
	char arg[49], c;
	i = j = k = 0;
	while (2) {
		j=0;
        c= cmd[i++];
        while(c!=' ' && c!='\0') {
            arg[j++] = c;
            c = cmd[i++];
        }
        while(c==' '){
            c = cmd[i++];
        }
        i--;
        arg[j] = '\0';
        vals[k++] = strdup(arg);
        if(c=='\0')
            break;
    }
    vals[k] = '\0';
}
void initServer() {
	char filename[50], filesize[20];
	char fbuffer[1024];
	struct stat obj;
	
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
	printf("[SERVER] Waiting fo client to connect....\n" );


while((connectionSocket=accept(listenSocket , (struct sockaddr*)NULL,NULL))<0);


	printf("[CONNECTED]\n");

	bzero(buffer,1024);
	if(recv(connectionSocket,buffer,1024,0)<0)
		printf("ERROR while reading from Client\n");


	printf("Message from Client: %s\n",buffer );
	
	switch(buffer[0]) {
		case 'd':

	sscanf(buffer+2, "%s", filename);
	if (buffer[1]=='t')
	{
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
else if (buffer[1]=='u') {
	// if (!initUDPServer(filename))
					// break;
	initUDPServer(filename);
}
	break;
	 case 'u':
	 sscanf(buffer+2, "%s", filename);
	if (buffer[1]=='t'){

	}
	else if (buffer[1]=='u') {
		if (!initUDPClient(filename))
					break;
	}


}

	close(connectionSocket);
	initServer();
}

char * sendMsg(char * msg) {
	char returncode[1024];
	int ClientSocket = 0;
	struct sockaddr_in c_serv_addr;

	ClientSocket = socket(AF_INET,SOCK_STREAM,0);
	c_serv_addr.sin_family = AF_INET;
	c_serv_addr.sin_port = htons(portno);
	c_serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
while(connect(ClientSocket,(struct sockaddr *)&c_serv_addr,sizeof(c_serv_addr))<0);
	bzero(buffer,1024);
	sscanf(msg, "%s", buffer);
	if(send(ClientSocket,buffer,strlen(buffer),0)<0)
		printf("ERROR while writing to the socket\n");
	bzero(buffer,1024);

	// if(recv(ClientSocket,buffer,1024,0)<0)
	// 	printf("ERROR while reading from the socket\n");
 // 	sscanf(buffer, "%s", returncode);
	// close(ClientSocket);
	// return returncode;
}
int initCDTCP(char * filename) {
	int client_pid = fork();
	if(client_pid != 0)
        return client_pid;
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
		return 0;
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
	strcat(filename, "_TCP");
	FILE *fp = fopen(filename+2, "wb");
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

void cpy(char * a, char * b) {
	int i, j;
	i=j=0;
	while (b[i]!='\0') {
		a[i] = b[i];
		i++;
	}
	a[i] = '\0';
}
int main(int argc, char *argv[])
{
	
	int serv_pid = 0;
	int udpServ_pid = 0;
	int i;
	char cmd[1024];
	char *vals[49];
	serv_pid = fork();
	if (serv_pid == 0) {
		listenSocket = socket(AF_INET,SOCK_STREAM,0);
	if(listenSocket<0)
	{
		printf("ERROR WHILE CREATING A SOCKET\n");
	}
	else
		printf("[SERVER] SOCKET ESTABLISHED SUCCESSFULLY\n\n");

		initServer();
        close(listenSocket);

	}
	char filename[50];
	char * returncode;
	while (1) {
		printf("$> ");
        scanf("%[^\n]s", cmd);
        getchar();
        parse(cmd, vals);
        if(!strcmp(vals[0], "FileDownload")) {
			if (!strcmp(vals[1], "TCP")) {
			bzero(filename, 50);
			cpy(filename+2, vals[2]);
			filename[0] = 'd';
			filename[1] = 't';
			if(!initCDTCP(filename))
                break;
			
			}
			else if (!strcmp(vals[1], "UDP")) {
				bzero(filename, 50);
				cpy(filename+2, vals[2]);
				filename[0] = 'd';
				filename[1] = 'u';
				sendMsg(filename);
				if (!initUDPClient(filename+2))
					break;
			}
			
        }
        else if (!strcmp(vals[0], "FileUpload")) {
			if (!strcmp(vals[1], "TCP")) {

			}
			else if (!strcmp(vals[1], "UDP")) {
				bzero(filename, 50);
				cpy(filename+2, vals[2]);
				filename[0] = 'u';
				filename[1] = 'u';
				sendMsg(filename);
				initUDPServer(filename+2);
			}
        }
        else if(!strcmp(vals[0], "exit")) {
            close(listenSocket);
            exit(0);
        }
        else if(!strcmp(vals[0], "IndexGet")) {
        	// handleIndex(1,0);
        	if (!strcmp(vals[1], "--shortlist")) {
        		handleIndex(0,0,"\0");
        	}
        	else if (!strcmp(vals[1], "--longlist")) {
        		handleIndex(1,0,"\0");
        	}
        	else if (!strcmp(vals[1], "--regex")) {
        		handleIndex(1,1,vals[2]);
        	}
        }
        continue;
	}
	printf("\nClosing connection2\n");
	
	return 0;
}