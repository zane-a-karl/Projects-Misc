#include <memory>
#include <cstdint>
#include <iostream>
#include <evhttp.h>

/* The following is an outline for the following libevet http server
   
   - Initialize a global object of the library using event_init function. You can use this function for single-threaded processing only. In order to execute a multi-threaded operation you should create a separate object for every thread.

   - Create http server using evhttp_start in case of a single-threaded server with a global object of event processing. You should delete the object created with the help of evhttp_start by using evhttp_free.

    - In order to respond to the incoming requests you should setup a callback function with the help of evhttp_set_gencb.

    - Then you can start the loop of event processing using event_dispatch function. This function is meant for operating with the global object in one thread.

    - When processing the request you can get a response buffer utilizing evhttp_request_get_output_buffer function. You can add some content to the buffer. For example, in order to send the line you can use evbuffer_add_printf function. If you want to send a file use evbuffer_add_file function. Then a response to the request should be sent. You can do this with the help of evhttp_send_reply.

 */


int main () {

  if (!event_init()) {
    std::cerr << "Failed to init libevent" << std::endl;
    return -1;
  }

  const char SrvAddress[] = "127.0.0.1";
  std::uint16_t SrvPort = 5555;
  std::unique_ptr<evhttp, decltype(&evhttp_free)> Server(evhttp_start(SrvAddress, SrvPort), &evhttp_free);

  if (!Server) {
    std::cerr << "Failed to init http server" << std::endl;
    return -1;
  }

  void (*OnReq)(evhttp_request *req, void *) = [] (evhttp_request *req, void *) {
    auto *OutBuf = evhttp_request_get_output_buffer(req);
    if (!OutBuf) {
      return;
    }
    evbuffer_add_printf(OutBuf, "<html><body><center><h1>Hello World!</h1></center></body></html>");
    evhttp_send_reply(req, HTTP_OK, "", OutBuf);
  };
  evhttp_set_gencb(Server.get(), OnReq, nullptr);
  if (event_dispatch() == -1) {
    std::cerr << "Failed to run messahe loop" << std::endl;
    return -1;
  }

  return 0;
}
