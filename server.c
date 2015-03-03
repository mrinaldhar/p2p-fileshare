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
#include "fileHash.c"
#include "tcpHandler.c"
#include <signal.h>
#include <dirent.h>


char *WEB_PORT;

char* replace(char* string, char replaceThis, char replaceWith)
{
	int l = strlen(string);
	int i;
	for (i = 0; i < l; ++i)
	{
		if(string[i] == replaceThis)
			string[i] = replaceWith;
	}
	return string;
}

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
		char filename[50];
	bzero(filename, 50);
			filename[0] = 'd';
			filename[1] = 't';
			strcpy(filename+2, "index-me-gui.txt");

			initCDTCP(filename);
               
            sleep(5);
  FILE *fp = fopen("index-me-gui.txt_TCP", "rb"); /* Similar to the above case, just for the remote server. */

 char buffer[1024];
  bzero(buffer,1024);
  while (fread(buffer, sizeof(char), 1024, fp))
  {
  mg_printf_data(conn, "%s", buffer);
  bzero(buffer, 1024);
  } 
}

}
static void fileTransfer(struct mg_connection *conn) {
	char action[10];
	char protocol[10];
	char filename[50];
  mg_get_var(conn, "act", action, sizeof(action));
  mg_get_var(conn, "via", protocol, sizeof(protocol));
  mg_get_var(conn, "file", filename, sizeof(filename));

      if (!strcmp(action, "Download")) {
      	if (!strcmp(protocol, "udp")) {
sendMsg(filename, 0);
		initUDPClient(filename+2, IP);
      	}
      	else {
			initCDTCP(filename);
                
      	}

      }
      else {
      	if (!strcmp(protocol, "udp")) {
sendMsg(filename, 0);
				initUDPServer(filename+2, IP);
      	}
      	else {
      		sendMsg(filename, 0);
      	}

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
      else if (!strcmp(conn->uri, "/transfer")) {
      	fileTransfer(conn);
        return MG_TRUE;

      }
      mg_send_file(conn, "index.html", s_no_cache_header);
      return MG_MORE;
    default: return MG_FALSE;
  }
}



int main(int argc, char *argv[])
{
	printf("====================== mSquared File Transfer Program ======================\n\n");
	printf("Developed by: Mrinal Dhar 201325118 | Mounika Somisetty 201330076\n");
	printf("Enter 'help' to get help about commands and other cool stuff this program can do.\n\n");
	printf("============================================================================\n\n");

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
		// printf("ERROR WHILE CREATING A SOCKET\n");
	}
	else {}
		// printf("[SERVER] SOCKET ESTABLISHED SUCCESSFULLY\n\n");

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
  // printf("Web Server Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) {
    mg_poll_server(server, 1000);
  }

  // Cleanup, and free server instance
  mg_destroy_server(&server);



	}
	

	periodicCheck();


	char filename[50];
	char chat_msg[50];

	
	char * returncode;
	while (1) {
		printf("$> ");
        scanf("%[^\n]s", cmd);
        // printf("THIS: %s\n", cmd);
        getchar();
        if (cmd[0]=='-') {
        	bzero(chat_msg, 50);
			cpy(filename+1, cmd+1);
			filename[0] = 'c';
			// printf("THIS=%s", filename);
			sendMsg(filename, 0);
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
				// sleep(5);
				if (!initUDPClient(filename+2, IP))
					break;

				sendMsg(filename, 0);
				bzero(filename, 50);

			}
			
        }
        else if (!strcmp(vals[0], "FileUpload")) {
			if (!strcmp(vals[1], "TCP")) {
				// printf("DOING!!\n\n");
				bzero(filename, 50);
				cpy(filename+2, vals[2]);
				filename[0] = 'u';
				filename[1] = 't';
				sendMsg(filename, 0);
				bzero(filename, 50);

			}
			else if (!strcmp(vals[1], "UDP")) {
				bzero(filename, 50);
				cpy(filename+2, vals[2]);
				filename[0] = 'u';
				filename[1] = 'u';
				returncode = sendMsg(filename, 1);
				// printf("GOT RCODE %s\n\n", returncode);
				if (returncode[0]=='y')
				{	
					printf("Permission granted!\n");
					sleep(2);
					initUDPServer(filename+2, IP);
				}
				bzero(filename, 50);

			}
        }
        else if(!strcmp(vals[0], "exit")) {
            close(listenSocket);
            system("killall -s 9 server");
            exit(0);
        }
        else if(!strcmp(vals[0], "FileHash")) {
        	if (!strcmp(vals[1], "--verify")) {
        		strcpy(filename, "hv");
        		strcat(filename, vals[2]);
        		sendMsg(filename, 0);
        	}
        	else if (!strcmp(vals[1], "--checkall")) {
        		strcpy(filename, "hc");
        		sendMsg(filename, 0);
        	}
        	bzero(filename, 50);
			filename[0] = 'd';
			filename[1] = 't';
			strcpy(filename+2, "fileHashOutput.txt");
			initCDTCP(filename);
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
        else if (!strcmp(vals[0], "help")) {
			FILE *fp = fopen("readme.txt", "rb");
			while (fread(buffer, sizeof(char), 1024, fp))
			{
				printf("%s", buffer);
				bzero(buffer,1024);
			}	
        }
        else if(!strcmp(vals[0], "IndexGet")) {  		/* I need indexget to work on both the remote machine and the local machine
        													and give output in four files: index-they.txt index-me.txt index-they-gui.txt index-me-gui.txt */
        	// handleIndex(1,0);
        	if (!strcmp(vals[1], "--shortlist")) {
        		strcpy(filename, "is");
        		sendMsg(filename, 0);
        		handleIndex(0,0,"\0", 0, 0);
        	}
        	else if (!strcmp(vals[1], "--longlist")) {
        		strcpy(filename, "il");
        		sendMsg(filename, 0);
        		handleIndex(1,0,"\0", 0, 0);
        	}
        	else if (!strcmp(vals[1], "--regex")) {
        		strcpy(filename, "ir");
        		strcat(filename, vals[2]);
        		sendMsg(filename, 0);
        		// handleIndex(1,1,vals[2], 0, 0);
        	}
        	bzero(filename, 50);
			filename[0] = 'd';
			filename[1] = 't';
			strcpy(filename+2, "index.txt");
			initCDTCP(filename);


        }
        continue;
	}
}
	printf("\nClosing connection2\n");
	
	return 0;
}
