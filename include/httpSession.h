#pragma once
#include <sys/types.h>

#include <cstddef>
#include <memory>

#include "http.h"
#include "socket.h"
#include "stream.h"

class HttpSession : public SocketStream,
                    public std::enable_shared_from_this<HttpSession> {
 public:
  using SharedPtr = std::shared_ptr<HttpSession>;

  HttpSession(net::Socket::SharedPtr sock);
  HttpRequest::SharedPtr recvRequest();
  ssize_t sendResponse(std::shared_ptr<HttpResponse> response);
};