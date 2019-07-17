/* for sockaddr_in */
#include <netinet/in.h>
/* for socket functions */
#include <sys/socket.h>
/* for fcntl */
#include <fcntl.h>

/* for libevent2 */
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_LINE 16384


/* The following program is using the libevent2 API to deal with multiple client requests in order to pass their input through a ROT13 cipher */

/* Remember that when compiling a libevent program include -levent */

void do_read (evutil_socket_t fd, short events, void* arg);
void do_write (evutil_socket_t fd, short events, void* arg);

char rot13_char (char c) {
  /* We don't want to use isalpha here; setting the locale would change which characters are considered alphbetical */
  if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M')) {
    return c + 13;
  }
  else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z')) {
    return c - 13;
  }
  else {
    return c;
  }
}

void readcb (struct bufferevent* bev, void* ctx) {

  printf("I am reading input right now");
  struct evbuffer *input, *output;
  char *line;
  size_t n;
  int i;

  input = bufferevent_get_input(bev);
  output = bufferevent_get_output(bev);

  while ((line = evbuffer_readln(input, &n, EVBUFFER_EOL_LF))) {
    for (i=0; i<n; ++i) {
      line[i] = rot13_char(line[i]);
    }
    evbuffer_add(output, line, n);
    evbuffer_add(output, "\n", 1);
    free(line);
  }

  if (evbuffer_get_length(input) >= MAX_LINE) {
    /* Too long; just process what there is and go on so that the buffer, doesn't grow indefinitely long.*/
    char buf[1024];
    while (evbuffer_get_length(input)) {
      int n = evbuffer_remove(input, buf, sizeof(buf));
      for(i=0; i<n; ++i) {
	buf[i] = rot13_char(buf[i]);
      }
      evbuffer_add(output, buf, n);
    }
    evbuffer_add(output, "\n", 1);
  }
}

void errorcb (struct bufferevent *bev, short error, void *ctx) {
  printf("I am handling errors right now");
  if ( error & BEV_EVENT_EOF) {
    /* connection has been closed ... do clean up here */
    /* ... */
    perror("eof error");
  } else if (error & BEV_EVENT_ERROR) {
    /* check errno to see what error occurred */
    /* ... */
    perror("error error");
  } else if (error & BEV_EVENT_TIMEOUT) {
    /* must be a timeout event handle... handle it! */
    /* ... */
    perror("timeout error");
  }
  bufferevent_free(bev);
}

void do_accept (evutil_socket_t listener, short event, void *arg) {
  printf("I am accepting right now\n");
  struct event_base* base = arg;
  struct sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int fd = accept(listener, (struct sockaddr*)&ss, &slen);
  if (fd<0) {
    perror("accept");
  } else if (fd > FD_SETSIZE) {
    printf("I am less than FD_SETSIZE\n");
    close(fd);
  } else {
    printf("I am about to create an accept bufferevent\n");
    struct bufferevent *bev;
    evutil_make_socket_nonblocking(fd);
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(bev, readcb, NULL, errorcb, NULL);
    bufferevent_setwatermark(bev, EV_READ, 0, MAX_LINE);
    bufferevent_enable(bev, EV_READ|EV_WRITE);
  }
}

void run (void) {
  printf("I am at the beginning of run()\n");
  evutil_socket_t listener;
  struct sockaddr_in sin;
  struct event_base* base;
  struct event* listener_event;

  base = event_base_new();
  if (!base) {
    return; // XXX err
  }

  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(40713);

  listener = socket(AF_INET, SOCK_STREAM, 0);
  evutil_make_socket_nonblocking(listener);

  #ifndef WIN32
  {
    int one = 1;
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
  }
  #endif

  if ( bind(listener, (struct sockaddr*)&sin, sizeof(sin)) < 0) {
    perror("bind");
    return;
  }

  if ( listen(listener, 16) < 0) {
    perror("listen");
    return;
  }
  printf("I am just about to listen\n");
  listener_event = event_new(base,listener,EV_READ|EV_PERSIST,do_accept,(void*)base);
  /* XXX check it */
  event_add(listener_event, NULL);
  printf("I have added a listener event\n");
  event_base_dispatch(base); printf("I have dispatched an event_base\n");
}

int main (int argc, char* argv[]) {

  setvbuf(stdout, NULL, _IONBF, 0);

  run();
  return 0;
}

