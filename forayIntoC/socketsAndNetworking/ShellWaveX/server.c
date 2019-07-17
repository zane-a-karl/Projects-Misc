#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char* argv[]) {
  
  // variables
  int sock;
  struct sockaddr_in server;
  int mysock;

  char buff[1024];
  int rval;

  // Create socket
  sock = socket(AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(5000);

  // Call bind
  if (bind(sock, (struct sockaddr*)&server, sizeof(server))) {
    perror("bind failed");
    exit(1);
  }

  // Listen
  listen(sock, 5);

  // Accept
  do {
    mysock = accept(sock, (struct sockaddr*)0, 0);
    if (mysock == 1) {
      perror("accept failed");
    } else {
      memset(buff,0,sizeof(buff));
      if ((rval = recv(mysock, buff, sizeof(buff), 0)) < 0) {
	perror("reading stream msg error");
      }
      else if (rval == 0) {
	printf("Ending connection");
      } else {
	printf("message: %s", buff);
      }

      printf("got message (rval = %d)\n", rval);
      close(mysock);
    }
  } while(1);

    return 0;
}
  
