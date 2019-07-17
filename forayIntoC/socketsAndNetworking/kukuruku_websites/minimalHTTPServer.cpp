#include "http_server.h"
#include "http_headers.h"
#include "http_content_type.h"
#include <iostream>
int main()
{
  try
  {
    using namespace Network;
    HttpServer Srv("127.0.0.1", 5555, 4,
      [&] (IHttpRequestPtr req)
      {
        req->SetResponseAttr(Http::Response::Header::Server::Value, "MyTestServer");
        req->SetResponseAttr(Http::Response::Header::ContentType::Value,
                             Http::Content::Type::html::Value);
        req->SetResponseString("<html><body><center><h1>Hello Wotld!</h1></center></body></html>");
      });
    std::cout << "Press Enter for quit." << std::endl;
    std::cin.get();
  }
  catch (std::exception const &e)
  {
    std::cout << e.what() << std::endl;
  }
  return 0;
}
