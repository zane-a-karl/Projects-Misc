/* for sockaddr_in */
#include <netinet/in.h>
/* for socket functions */
#include <sys/socket.h>
/* for fcntl */
#include <fcntl.h>
/* for select */
#include <sys/select.h>

/* for libevent2 */
#include <event2/event.h>

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define MAX_LINE 16384


/* The following program is using the libevent2 API to deal with multiple client requests in order to pass their input through a ROT13 cipher */

/* Other notable parts of the program include:
   - instead of typing the sockets as 'int' we use 'evutil_socket_t'
   - instead of calling fcntl(O_NONBLOCK) to make the sockets non-blocking, we call evutil_make_socket_nonblocking()
   - these two changes make the program compatible with the divergent parts of the Win32 networking API
 */

/* Notice however that as our code has become more efficient and scalable, it has become significantly more complex. 
   -Back when we were forking we didn't have to manage a buffer for each connection, but merely a separate stack allocated buffer for each process.
   -We didn't need to explicitly keep track of whether a socket was reading or writing, that was implicit in our locate in the code( i.e. send vs recv)
   -We also didn't need a structure to keep track of how much of each operation had completed, we just used loops and stack variables. 
 */

/* LOOKING EVEN FARTHER:
   Apparently if one is very experienced with networking on Windows, you'll understand that libevent isn't going to give you the optimal performance in this use you will see below.
   Windows doesn't use select() for fast asynchronos IO, but IOCP (IO completion ports).
   Unlike all the fast networking APIs, IOCP doesn't ALERT your program when it's READY for some operation, instead the Windows networking stack sends the signal to START and opeartion, and then IOCP tells program when operation has finished.
   - Libevent's bufferevents interface solves both of these issues. 
   - it's simpler to write and provides an interface for both Unix and Windows. 
 */

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

struct fd_state {
  char buffer[MAX_LINE];
  size_t buffer_used;

  size_t n_written;
  size_t write_upto;

  struct event* read_event;
  struct event* write_event;
};

struct fd_state* alloc_fd_state (struct event_base* base, evutil_socket_t fd) {
  
  struct fd_state* state = (struct fd_state*)malloc(sizeof(struct fd_state));
  if (!state) {
    return NULL;
  }

  state->read_event = event_new(base, fd, EV_READ | EV_PERSIST, do_read, state);
  if (!state->read_event) {
    free(state);
    return NULL;
  }
  state->write_event = event_new(base, fd, EV_WRITE|EV_PERSIST, do_write,state);
  if (!state->write_event) {
    event_free(state->read_event);
    free(state);
    return NULL;
  }

  state->buffer_used = state->n_written = state->write_upto = 0;
  
  return state;
}

void free_fd_state (struct fd_state* state) {
  event_free(state->read_event);
  event_free(state->write_event);
  free(state);
}

void do_read (evutil_socket_t fd, short events, void* arg) {

  struct fd_state* state = arg;
  char buf[1024];
  int i;
  ssize_t result;
  while (1) {
    assert(state->write_event);
    result = recv(fd, buf, sizeof(buf), 0);
    if (result <= 0) {
      break;
    }
    
    for (i=0; i<result; ++i) {
      if (state->buffer_used < sizeof(state->buffer)) {
	state->buffer[state->buffer_used++] = rot13_char(buf[i]);
      }
      if (buf[i] == '\n') {
	assert(state->write_event);
	event_add(state->write_event, NULL);
	state->write_upto = state->buffer_used;
      }
    }
  }
  
  if (result == 0) {
    free_fd_state(state);
  } else if (result < 0) {
    if (errno == EAGAIN) { // XXXX use evutil macro
      return;
    }
    perror("recv");
    free_fd_state(state);
  }
}

void do_write (evutil_socket_t fd, short events, void* arg) {

  struct fd_state* state = arg;
  
  while (state->n_written < state->write_upto) {
    ssize_t result = send(fd, state->buffer + state->n_written, state->write_upto - state->n_written, 0);
    if (result < 0) {
      if (errno == EAGAIN){ // XXXX use evutil macro
	return;
      }
      free_fd_state(state);
      return;
    }
    assert(result != 0);

    state->n_written += result;
  }

  if (state->n_written == state->buffer_used) {
    state->n_written = state->write_upto = state->buffer_used = 0;
  }
  event_del(state->write_event);
}

void do_accept (evutil_socket_t listener, short event, void* arg) {

  struct event_base* base = arg;
  struct sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int fd = accept(listener, (struct sockaddr*)&ss, &slen);
  if (fd<0) { // XXXX eagain?
    perror("accept");
  } else if (fd > FD_SETSIZE) {
    close(fd); // XXX replace all closes with EVUTIL_CLOSESOCKET
  } else {
    struct fd_state* state;
    evutil_make_socket_nonblocking(fd);
    state = alloc_fd_state(base, fd);
    assert(state); // XXX err
    assert(state->write_event);
    event_add(state->read_event, NULL);
  }
}

void run (void) {
  
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

  listener_event = event_new(base, listener, EV_READ | EV_PERSIST, do_accept, (void*)base);
  /* XXX check it */
  event_add(listener_event, NULL);

  event_base_dispatch(base);
}

int main (int argc, char* argv[]) {

  setvbuf(stdout, NULL, _IONBF, 0);

  run();
  return 0;
}
