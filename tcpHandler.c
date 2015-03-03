int listenSocket = 0;
int connectionSocket = 0;
char buffer[1024];
struct sockaddr_in s_serv_addr;
int portno = 5005;
char IP[25];
	char perm;
char* replace(char* string, char replaceThis, char replaceWith);

char * sendMsg(char * msg, int wait) {
	// printf("Send: : :%s\n",msg );
	msg = replace(msg,' ', '&');
	// printf("messssss %s\n",msg );
	char returncode[1024];
	int ClientSocket = 0;
	struct sockaddr_in c_serv_addr;

	ClientSocket = socket(AF_INET,SOCK_STREAM,0);
	c_serv_addr.sin_family = AF_INET;
	c_serv_addr.sin_port = htons(portno);
	c_serv_addr.sin_addr.s_addr = inet_addr(IP);
while(connect(ClientSocket,(struct sockaddr *)&c_serv_addr,sizeof(c_serv_addr))<0);
	bzero(buffer,1024);
	sscanf(msg, "%s", buffer);
	if(send(ClientSocket,buffer,strlen(buffer),0)<0) 
		printf("ERROR while writing to the socket\n");
	bzero(buffer,1024);
if (wait == 1) {
	if(recv(ClientSocket,buffer,1024,0)<0)
		printf("ERROR while reading from the socket\n");
 	sscanf(buffer, "%s", returncode);
	close(ClientSocket);
	// printf("WAITING: %s\n", returncode);
	return returncode;
}
	
}

void initServer() {
	char filename[50], filesize[20];
	char fbuffer[1024];
	struct stat obj;
	perm = 'n';
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

	// Listening to connections

	if(listen(listenSocket,10) == -1)	//maximum connections listening to 10
	{
		printf("[SERVER] FAILED TO ESTABLISH LISTENING \n\n");
	}


while((connectionSocket=accept(listenSocket , (struct sockaddr*)NULL,NULL))<0);


	// printf("[CONNECTED]\n");

	bzero(buffer,1024);
	if(recv(connectionSocket,buffer,1024,0)<0)
		printf("ERROR while reading from Client\n");


	// printf("Message from Client: %s\n",buffer );
	strcpy(buffer, replace(buffer,'&',' '));
	
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
	// printf("The size of file is: %s\n", buffer);

	while (fread(buffer, sizeof(char), 1024, fp))
	{
		send(connectionSocket,buffer,1024,0);
		bzero(buffer,1024);
	}	
	strcpy(buffer+1, getFileInfo(filename));
	buffer[0]='c';
	// printf("DATA: %s", buffer);
	sendMsg(buffer, 0);

}
else if (buffer[1]=='u') {
	// if (!initUDPServer(filename))
					// break;
	initUDPServer(filename, IP);
}
	break;
	 case 'u':
				 sscanf(buffer, "%s", filename);

	 printf("Remote wants to upload a file. Allow? (y/n)\n->");
	 if (perm == 'n') {
	 scanf("%c", &perm);
	}
	// printf("PERM: %c\n", perm);
	// printf("FILE%s\n", filename);
	if (filename[1]=='t'){
	 sscanf(buffer, "%s", filename);
	 filename[0]='d';
	initCDTCP(filename);
}
	else if (filename[1]=='u') {
		if (perm == 'y') {
			// printf("ENTERED!\n\n");
				 sscanf(buffer+2, "%s", filename);
		bzero(buffer,1024);
		strcpy(buffer, "y");
		// printf("buff%s\n", buffer);
	 	send(connectionSocket,buffer,1024,0);
		bzero(buffer,1024);
		if (!initUDPClient(filename, IP))
					break;
			}
	}
break;
case 'c':
putchar('+');

	printf("%s\n", buffer+1);
	break;

	case 'i':
	if (buffer[1]=='l') {
        		handleIndex(1,0,"\0", 0, 0);

	}
	else if (buffer[1]=='s') {
        		handleIndex(0,0,"\0", 0, 0);

	}
	else if (buffer[1]=='r') {
        		handleIndex(1,1,buffer+2, 0, 0);

	}
	break;

	case 'h':
	if (buffer[1]=='v') {
		fileHash(1, buffer+2, ".");		//change if shared directory has been changed. THIS IS HARDCODED
	}
	else if (buffer[1]=='c') {
		fileHash(0, ".", ".");
	}
	break;
}

	close(connectionSocket);
	initServer();
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
	// else
		// printf("[CLIENT] Socket created \n");



	c_serv_addr.sin_family = AF_INET;
	c_serv_addr.sin_port = htons(portno);
	c_serv_addr.sin_addr.s_addr = inet_addr(IP);


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
    	// printf("The filesize is %lld\n", fileSize);
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
	// printf("Closing Connection\n");
	close(ClientSocket);
	exit(0);
	return 1;
}