#include "httpServer.h"

#include <memory>

#include "http.h"
#include "httpSession.h"
#include "servlet.h"
#include "socket.h"

namespace util {
int defaultService(HttpRequest::SharedPtr request,
                   HttpResponse::SharedPtr response,
                   HttpSession::SharedPtr session) {
  response->setBody(
      "<html><body><h3>Hello, Wecome to the cnet!</h3></body></html>");
  response->setStatus(HttpStatus::OK);
  return 0;
}
}  // namespace util

HttpServer::HttpServer(bool keepalive, IOManager *acceptorWorker,
                       IOManager *ioWorker)
    : TcpServer(acceptorWorker, ioWorker), isKeepalive(keepalive) {
  serviceManager = std::make_unique<ServletManager>();
  serviceManager->addServlet("/example", util::defaultService);
}
HttpServer::~HttpServer() { std::cout << "[HttpServer::~HttpServer()]\n"; }

void HttpServer::clientHandle(net::Socket::SharedPtr sock) {
  std::cout << "[Client-Handle accept, host: "
            << sock->getAddress()->getStringAddress() << "]\n";

  HttpSession::SharedPtr session(new HttpSession(sock));
  do {
    auto req = session->recvRequest();

    if (req == nullptr) {
      std::cout << "[ERR: HttpSession recv Request is wrong!]\n";
      break;
    }

    HttpResponse::SharedPtr rsp(new HttpResponse);

    serviceManager->handle(req, rsp, session);

    int rt = session->sendResponse(rsp);
    if (rt == -1) {
      std::cout << "[ERR: HttpSession send Resonse is wrong! |  host: "
                << sock->getAddress()->getStringAddress() << "]\n";
      continue;
    }
    if (!isKeepalive || req->isClose()) {
      std::cout << "[CLI CLOSE OR NON-KEEPALIVE | host: "
                << sock->getAddress()->getStringAddress() << "]\n";
      break;
    }

  } while (true);
  session->close();
}

void HttpServer::regist(const std::string &uri, Servlet::HandleType handle) {
  serviceManager->addServlet(uri, handle);
}