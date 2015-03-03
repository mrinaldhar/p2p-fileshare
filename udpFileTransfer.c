#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define BUFLEN 512
#define NPACK 10
#define PORT 9930

void PrintErrorAndDie(char *s)
{
	printf("%s\n",s );
	perror(s);
	exit(1);
}

int initUDPClient(char * filename, char * IP)
{
		int client = fork();
	if(client != 0)
        return client;
		int listenSocket, connect;
		listenSocket = socket(AF_INET,SOCK_STREAM,0);
		struct sockaddr_in socket_me, socket_other;
		int udp_socket, i, slen=sizeof(socket_other);
		char buf[BUFLEN];
		if ((udp_socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
			PrintErrorAndDie("Error in creating socket");
		memset((char *) &socket_me, 0, sizeof(socket_me));
		socket_me.sin_family = AF_INET;
		socket_me.sin_port = htons(PORT);
		if (inet_aton(IP, &socket_me.sin_addr)==0)
		{
			fprintf(stderr, "inet_aton() failed\n");
			exit(1);
		}
		if (bind(udp_socket, &socket_me, sizeof(socket_me))==-1)
			PrintErrorAndDie("Error in bind");

		printf("[UDP SERVER]: Listening...Waiting for Client...\n");
	    

		// int output;
		strcat(filename, "_UDP");

		int fileReceived, output;
		remove(filename);
		fileReceived =  open(filename,O_CREAT|O_WRONLY|O_TRUNC, S_IWRITE | S_IREAD);
		if(fileReceived < 0)
			PrintErrorAndDie("Error creating new file");

		while(1)
		{
			if (recvfrom(udp_socket, buf, BUFLEN, 0, &socket_other, &slen)==-1)
				continue;
				// PrintErrorAndDie("recvfrom()");
			printf("RECEIVED: %s", buf);

			output =  write(fileReceived,buf,strlen(buf));
			if(output < 0)
				PrintErrorAndDie("Error in writing");
		}
		close(fileReceived);

		bzero(buf,1024);
		close(udp_socket);
	return 0;
}

int initUDPServer(char * filename, char * IP)
{
	struct sockaddr_in socket_other;
	int udp_socket, i, slen=sizeof(socket_other);
	char buf[BUFLEN];
		 
	if ((udp_socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	 PrintErrorAndDie("Error in creating socket");
 
	memset((char *) &socket_other, 0, sizeof(socket_other));
	socket_other.sin_family = AF_INET;
	socket_other.sin_port = htons(PORT);
	socket_other.sin_addr.s_addr = htonl(INADDR_ANY);
	int fileToSend,output;
	fileToSend = open(filename,O_RDONLY,S_IREAD);
	printf("%s\n", filename);

	bzero(buf,BUFLEN);
	while(output = read(fileToSend, buf, BUFLEN))
	{

		printf("SENT: %s\n", buf);

		if(sendto(udp_socket, buf, BUFLEN, 0, &socket_other, slen)==-1)
			PrintErrorAndDie("sendto()");
		bzero(buf,BUFLEN);
	}
	close(fileToSend);
	close(udp_socket);
	return 0;
 }
 handleUDP(int mode, char * filename)
 {
 }
