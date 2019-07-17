#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

#define DATA "If you only do what you can do, you'll never be better than what you are\n"

int main (int argc, char* argv[]) {
  int sock;
  struct sockaddr_in server;
  char buff[1024];
  struct hostent* hp;

  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    close(sock);
  }

  server.sin_family = AF_INET;
  
  hp = gethostbyname (argv[1]);
  if ( hp == 0) {
    perror("Could not find that hostname, gethostbyname failed");
    close(sock);
    exit(1);
  }

  memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
  server.sin_port = htons(5000);

  if ((connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0)) {
    perror("connection failed");
    close(sock);
    exit(1);
  }

  if ((send(sock, DATA, sizeof(DATA), 0)) < 0) {
    perror("send failed");
    close(sock);
  }
  printf("Sent %s \n", DATA);
  close(sock);
  return 0;
}
