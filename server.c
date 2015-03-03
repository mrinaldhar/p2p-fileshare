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
#include "mongoose.h"
#include "tcpHandler.c"

char *WEB_PORT;

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

char * sendMsg(char * msg) {
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

	// if(recv(ClientSocket,buffer,1024,0)<0)
	// 	printf("ERROR while reading from the socket\n");
 // 	sscanf(buffer, "%s", returncode);
	// close(ClientSocket);
	// return returncode;
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

static const char *s_no_cache_header =
  "Cache-Control: max-age=0, post-check=0, "
  "pre-check=0, no-store, no-cache, must-revalidate\r\n";

static void handle_restful_call(struct mg_connection *conn) {
  char n1[100], n2[100];

  // Get form variables
  mg_get_var(conn, "n1", n1, sizeof(n1));
  mg_get_var(conn, "n2", n2, sizeof(n2));

  mg_printf_data(conn, "{ \"result\": %lf }", strtod(n1, NULL) + strtod(n2, NULL));
}

static void show_index(struct mg_connection *conn) {
  char who[100];
  char date[20], times[20], type[5], size[10], name[100];
  mg_get_var(conn, "whose", who, sizeof(who));
  if (!strcmp(who, "me")) {
  FILE *fp = fopen("index-me-gui.txt", "rb"); /* I need a index-me-gui.txt file with the format as shown in the current index-me-gui.txt file 
  										that I created from hand right now. It should be easy, just need to change the indexget file output a bit.	*/
 
 char buffer[1024];
  bzero(buffer,1024);
  while (fread(buffer, sizeof(char), 1024, fp))
  {
  mg_printf_data(conn, "%s", buffer);
  bzero(buffer, 1024);
  } 
}
else if (!strcmp(who, "they")) {
  // FILE *fp = fopen("index-they-gui.txt", "rb"); /* Similar to the above case, just for the remote server. */

 // char buffer[1024];
 //  bzero(buffer,1024);
 //  while (fread(buffer, sizeof(char), 1024, fp))
 //  {
 //  mg_printf_data(conn, "%s", buffer);
 //  bzero(buffer, 1024);
 //  } 
}

}

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      if (!strcmp(conn->uri, "/ind")) {
        // handle_restful_call(conn);
        show_index(conn);
        return MG_TRUE;
      }
      mg_send_file(conn, "index.html", s_no_cache_header);
      return MG_MORE;
    default: return MG_FALSE;
  }
}



int main(int argc, char *argv[])
{
	
	int serv_pid = 0;
	int udpServ_pid = 0;
	int i;
	char cmd[1024];
	char *vals[49];
	WEB_PORT = strdup("8000");

	if(argc == 2)
	{
		bzero(IP,sizeof(IP));
		strcpy(IP,argv[1]);
	}
	else {
		strcpy(IP, "127.0.0.1");
	}


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

        	serv_pid = fork();
	if (serv_pid == 0) {

struct mg_server *server;

  // Create and configure the server
  server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", WEB_PORT);

  // Serve request. 
  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) {
    mg_poll_server(server, 1000);
  }

  // Cleanup, and free server instance
  mg_destroy_server(&server);



	}
	


	char filename[50];
	char chat_msg[50];

	
	char * returncode;
	while (1) {
		printf("$> ");
        scanf("%[^\n]s", cmd);
        printf("THIS: %s\n", cmd);
        getchar();
        if (cmd[0]=='-') {
        	bzero(chat_msg, 50);
			cpy(filename+1, cmd+1);
			filename[0] = 'c';
			printf("THIS=%s", filename);
			sendMsg(filename);
        }
        else {
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
				if (!initUDPClient(filename+2, IP))
					break;
				bzero(filename, 50);

			}
			
        }
        else if (!strcmp(vals[0], "FileUpload")) {
			if (!strcmp(vals[1], "TCP")) {
				printf("DOING!!\n\n");
				bzero(filename, 50);
				cpy(filename+2, vals[2]);
				filename[0] = 'u';
				filename[1] = 't';
				sendMsg(filename);
				bzero(filename, 50);

			}
			else if (!strcmp(vals[1], "UDP")) {
				bzero(filename, 50);
				cpy(filename+2, vals[2]);
				filename[0] = 'u';
				filename[1] = 'u';
				sendMsg(filename);
				initUDPServer(filename+2, IP);
				bzero(filename, 50);

			}
        }
        else if(!strcmp(vals[0], "exit")) {
            close(listenSocket);
            exit(0);
        }
        else if(!strcmp(vals[0], "gui")) {

	// 			THE FOLLOWING CODE TRIES TO AUTOMATICALLY OPEN FIREFOX WHEN GUI IS ACTIVATED. This has some bugs. 
	// char * ffox[1];
	
	// ffox[0] = strdup("firefox ");
	// strcat(ffox[0], IP);
	// strcat(ffox[0], ":");
	// strcat(ffox[0], WEB_PORT);
	// serv_pid = fork();
	// if (serv_pid==0) {
	// if (execvp(ffox[0], ffox) < 0) {      
 //               printf("*** ERROR: exec failed\n");
 //               exit(1);
 //          }
 //      }
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
}
	printf("\nClosing connection2\n");
	
	return 0;
}
