#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define BUFLEN 512
#define NPACK 10
#define PORT 9930
#define SRV_IP "127.0.0.1"

void PrintErrorAndDie(char *s)
{
	printf("%s\n",s );
	perror(s);
	exit(1);
}

int initUDPServer(void)
{
	while(1)
	{
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
		socket_me.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(udp_socket, &socket_me, sizeof(socket_me))==-1)
			PrintErrorAndDie("Error in bind");

		printf("[UDP SERVER]: Listening...Waiting for Client...\n");
	    

		int fileReceived, output;
		fileReceived =  open("SampleFiles/Received_image.jpeg",O_CREAT|O_WRONLY|O_TRUNC, S_IWRITE);
		if(fileReceived < 0)
			PrintErrorAndDie("Error creating new file");

		while(1)
		{
			if (recvfrom(udp_socket, buf, BUFLEN, 0, &socket_other, &slen)==-1)
				PrintErrorAndDie("recvfrom()");
			output =  write(fileReceived,buf + 1,BUFLEN - 1);
			if(output < 0)
				PrintErrorAndDie("Error in writing");
		}
		close(udp_socket);
	}
	return 0;
}

int initUDPClient(void)
{
	struct sockaddr_in socket_other;
	int udp_socket, i, slen=sizeof(socket_other);
	char buf[BUFLEN];
		 
	if ((udp_socket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	 PrintErrorAndDie("Error in creating socket");
 
	memset((char *) &socket_other, 0, sizeof(socket_other));
	socket_other.sin_family = AF_INET;
	socket_other.sin_port = htons(PORT);
	if (inet_aton(SRV_IP, &socket_other.sin_addr)==0)
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}


	int fileToSend,output;
	fileToSend = open("SampleFiles/image.jpeg",O_RDONLY,S_IREAD);
	bzero(buf,BUFLEN);
	while(output = read(fileToSend, buf + 1, BUFLEN - 1))
	{
		buf[0] ='x';
		if(sendto(udp_socket, buf, BUFLEN, 0, &socket_other, slen)==-1)
			PrintErrorAndDie("sendto()");
		bzero(buf,BUFLEN);
	}
 
	close(udp_socket);
	return 0;
 }
 int main(int argc, char const *argv[])
 {
 	int soc;
 	scanf("%d",&soc);
 	if(soc == 1)
 		initUDPClient();
 	else
 		initUDPServer();
 	return 0;
 }