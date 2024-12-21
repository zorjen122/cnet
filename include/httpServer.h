#pragma once
#include <memory>
#include <unordered_map>

#include "httpSession.h"
#include "servlet.h"
#include "tcpServer.h"

class HttpServer : public TcpServer {
 public:
  using SharedPtr = std::shared_ptr<HttpServer>;

  HttpServer(bool keepalive = false,
             IOManager *acceptorWorker = IOManager::getThis(),
             IOManager *ioWorker = IOManager::getThis());
  ~HttpServer();

  void regist(const std::string &uri, Servlet::HandleType handle);

 private:
  void clientHandle(net::Socket::SharedPtr sock) override;

 private:
  bool isKeepalive;
  std::unique_ptr<ServletManager> serviceManager;
};
