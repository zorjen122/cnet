#include "httpSession.h"

#include <sys/types.h>

#include <iostream>
#include <memory>

#include "http.h"
#include "httpParser.h"
#include "socket.h"
#include "stream.h"

HttpSession::HttpSession(net::Socket::SharedPtr sock) : SocketStream(sock) {}

HttpRequest::SharedPtr HttpSession::recvRequest() {
  std::cout << "[recvRequest...]\n";
  char buf[4 * 1024] = {};
  ssize_t nbyte = SocketStream::read(buf, 4 * 1024);

  if (nbyte <= 0) {
    return nullptr;
  }

  HttpParser parser(HttpParser::Type::REQUEST);

  parser.parser(buf, nbyte);

  return parser.getData<HttpRequest>();
}
ssize_t HttpSession::sendResponse(std::shared_ptr<HttpResponse> response) {
  std::string buf = response->format();
  ssize_t nbyte = SocketStream::writeFixed(buf.c_str(), buf.size());

  if (nbyte <= 0) {
    std::cout << "[ERR: Send-Response is wrong!\n]";
    return -1;
  }
  std::cout << "[Send-Response: " << buf << "\n";

  return nbyte;
}