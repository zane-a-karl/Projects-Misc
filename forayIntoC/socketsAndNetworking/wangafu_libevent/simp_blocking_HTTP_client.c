/* for sockaddr_in */
#include <netinet/in.h>
/* for socket functions */
#include <sys/socket.h>
/* for gethostbyname */
#include <netdb.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>

/* All of the network calls in the code are BLOCKING: the gethostbyname does not return until it has succeeded or failed in resolving www.google.com; the connect does not return until ithas connected; the recv calls do not return until they have received data or a close; and the send call does not return until it has at least flushed its output to the kernel's write buffers.
 */

int main (int argc, char* argv[]) {
  const char query[] = "GET / HTTP/1.0\r\n"
    "Host: www.google.com\r\n"
    "\r\n";
  const char hostname[] = "www.google.com";
  struct sockaddr_in sin;
  struct hostent* h;
  const char* cp;
  int fd;
  ssize_t n_written, remaining;
  char buf[1024];

  /* Look up the ip address for the hostname. Watch out! This isn't threadsafe on most platforms */
  if (!(h = gethostbyname(hostname))) {
    fprintf(stderr, "Couldn't lookup %s: %s", hostname, hstrerror(h_errno));
    return 1;
  }
  if (h->h_addrtype != AF_INET) {
    fprintf(stderr, "Sorry, no ipv6 support");
    return 1;
  }
  
  /* Allocate new socket */
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    return 1;
  }

  /* Connect to remote host */
  sin.sin_family = AF_INET;
  sin.sin_port = htons(80);
  sin.sin_addr = *(struct in_addr*)h->h_addr;
  if (connect(fd, (struct sockaddr*)&sin, sizeof(sin))) {
    perror("connect");
    return 1;
  }

  /* Write the query */
  /* xxx can send succeed partially? */
  cp = query;
  remaining = strlen(query);
  while (remaining) {
    n_written = send(fd, cp, remaining, 0);
    if (n_written <= 0) {
      perror("send");
      return 1;
    }
    remaining -= n_written;
    cp +=n_written;
  }

  /* Get and answer back */
  while (1) {
    ssize_t result = recv(fd, buf, sizeof(buf), 0);
    if ( result == 0) {
      break;
    } else if (result < 0) {
      perror("recv");
      close(fd);
      return 1;
    }
    fwrite(buf, 1, result, stdout);
  }

  close(fd);
  return 0;
}
