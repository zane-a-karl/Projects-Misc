#include <stdexcept>
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <cstdint>
#include <vector>
#include <evhttp.h>

/* The following is an outline of the succeeding code:

    - Create several threads. Their number should be equal to the doubled quantity of CPU cores. C++11 supports operation with threads so you don’t need to write your wrappers.

    - Create an object operating with events using event_base_new function for each thread. event_base_free function should delete this object in the end. std::unique_ptr and RAII allow executing it more compactly.

    - Create a separate HTTP server object for every thread using evhttp_new considering the created above object. You should delete this object as well using evhttp_free.

    - Setup request handler with the help of evhttp_set_gencb.

    - This step may seem the strangest one. You should create a socket and attach it to the network interface for several handlers. Each of the handlers is located in its thread. In order to operate with sockets (create a socket, adjust it, attach to the certain interface) you can utilize API. Then you pass the socket for the server operation using evhttp_accept_socket function. It’s too long. Libevent provides several functions to help to solve it. As I’ve mentioned before, libevent allows going to lower levels allowing users more finer grained control in cases of necessity. In this case evhttp_bind_socket_with_handle executes all of the first thread work on creating the socket, adjusting and attaching it. evhttp_bound_socket_get_fd extracts a socket for other threads from the adjusted object. All other threads are already using the socket having set it up to be processed by evhttp_accept_socket function. It’s a bit strange, but far easier than using sockets API. It’s even easier taking the cross-platforms into account. The API for Berkeley sockets may seem the same. But if you have written cross-platform software using it for Windows and Linux, you should know that the code written for one system differs from the code written for the other one.

    - Start the event service loop. You should do it differently, in contrast to a single-threaded server as there are different objects. There’s event_base_dispatch function in libevent for that purpose. To my mind, it has one drawback: it’s difficult to correct it properly (for example, there should be a situation when you can call event_base_loopexit). In or case you can use event_base_loop function. It’s non-blocking even if there are no events to be processed. It returns control that allows you to terminate the event service loop and do something between the calls. There’s also a drawback as you should place a bit delay not to load the idling processor (in C++11 you can do it using std::this_thread::sleep_for(std::chrono::milliseconds(10)) ).

    - Requests processing is similar to the first example.

    - During another thread creation and adjustment there can be something wrong with its function. For example, some libevent function has thrown an error. You can throw an exception and catch it. Then you can send it out of the thread limits using C++11 facilities (std::exception_ptr, std::current_exception and std::rethrow_exception)

 */

int main()
{
  char const SrvAddress[] = "127.0.0.1";
  std::uint16_t const SrvPort = 5555;
  int const SrvThreadCount = 4;
  try
  {
    void (*OnRequest)(evhttp_request *, void *) = [] (evhttp_request *req, void *)
    {
      auto *OutBuf = evhttp_request_get_output_buffer(req);
      if (!OutBuf)
        return;
      evbuffer_add_printf(OutBuf, "<html><body><center><h1>Back to the Past, Samurai Jack!</h1></center></body></html>");
      evhttp_send_reply(req, HTTP_OK, "", OutBuf);
    };
    std::exception_ptr InitExcept;
    bool volatile IsRun = true;
    evutil_socket_t Socket = -1;
    auto ThreadFunc = [&] ()
    {
      try
      {
        std::unique_ptr<event_base, decltype(&event_base_free)> EventBase(event_base_new(), &event_base_free);
        if (!EventBase)
          throw std::runtime_error("Failed to create new base_event.");
        std::unique_ptr<evhttp, decltype(&evhttp_free)> EvHttp(evhttp_new(EventBase.get()), &evhttp_free);
        if (!EvHttp)
          throw std::runtime_error("Failed to create new evhttp.");
          evhttp_set_gencb(EvHttp.get(), OnRequest, nullptr);
        if (Socket == -1)
        {
          auto *BoundSock = evhttp_bind_socket_with_handle(EvHttp.get(), SrvAddress, SrvPort);
          if (!BoundSock)
            throw std::runtime_error("Failed to bind server socket.");
          if ((Socket = evhttp_bound_socket_get_fd(BoundSock)) == -1)
            throw std::runtime_error("Failed to get server socket for next instance.");
        }
        else
        {
          if (evhttp_accept_socket(EvHttp.get(), Socket) == -1)
            throw std::runtime_error("Failed to bind server socket for new instance.");
        }
        for ( ; IsRun ; )
        {
          event_base_loop(EventBase.get(), EVLOOP_NONBLOCK);
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
      }
      catch (...)
      {
        InitExcept = std::current_exception();
      }
    };
    auto ThreadDeleter = [&] (std::thread *t) { IsRun = false; t->join(); delete t; };
    typedef std::unique_ptr<std::thread, decltype(ThreadDeleter)> ThreadPtr;
    typedef std::vector<ThreadPtr> ThreadPool;
    ThreadPool Threads;
    for (int i = 0 ; i < SrvThreadCount ; ++i)
    {
      ThreadPtr Thread(new std::thread(ThreadFunc), ThreadDeleter);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      if (InitExcept != std::exception_ptr())
      {
        IsRun = false;
        std::rethrow_exception(InitExcept);
      }
      Threads.push_back(std::move(Thread));
    }
    std::cout << "Press Enter fot quit." << std::endl;
    std::cin.get();
    IsRun = false;
  }
  catch (std::exception const &e)
  {
    std::cerr << "Error: " << e.what() << std::endl;
  }
  return 0;
}
