#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mongoose.h"

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

static void show_my_index(struct mg_connection *conn) {
  char who[100];
  char date[20], times[20], type[5], size[10], name[100];
  // mg_get_var(conn, "whose", who, sizeof(who));
  // if (!strcmp(who, "me")) {
  FILE *fp = fopen("index-gui.txt", "rb");
  char buffer[1024];
  bzero(buffer,1024);
  // mg_printf_data(conn, "{");
  while (fread(buffer, sizeof(char), 1024, fp))
  {
  //   sscanf(buffer, "%s\t%s\t%s\t%s\t%s", date, times, type, size, name);
  mg_printf_data(conn, "%s", buffer);
  // mg_printf_data(conn, "{ \"date\": %s, \"times\": %s, \"type\": %s, \"size\": %s, \"name\": %s  },", date, times, type, size, name);

  bzero(buffer, 1024);
    
  } 
  // mg_printf_data(conn, "}");

}

static int ev_handler(struct mg_connection *conn, enum mg_event ev) {
  switch (ev) {
    case MG_AUTH: return MG_TRUE;
    case MG_REQUEST:
      if (!strcmp(conn->uri, "/ind")) {
        // handle_restful_call(conn);
        show_my_index(conn);
        return MG_TRUE;
      }
      mg_send_file(conn, "index.html", s_no_cache_header);
      return MG_MORE;
    default: return MG_FALSE;
  }
}

int main(void) {
  struct mg_server *server;

  // Create and configure the server
  server = mg_create_server(NULL, ev_handler);
  mg_set_option(server, "listening_port", "8000");

  // Serve request. Hit Ctrl-C to terminate the program
  printf("Starting on port %s\n", mg_get_option(server, "listening_port"));
  for (;;) {
    mg_poll_server(server, 1000);
  }

  // Cleanup, and free server instance
  mg_destroy_server(&server);

  return 0;
}