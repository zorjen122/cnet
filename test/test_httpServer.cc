#include <spdlog/spdlog.h>

#include <iostream>

#include "http.h"
#include "httpServer.h"
#include "iomanager.h"
#include "servlet.h"

IOManager iom(4, true);

int hello_handle(HttpRequest::SharedPtr req, HttpResponse::SharedPtr rsp,
                 HttpSession::SharedPtr session) {
  spdlog::info("[Call-function: hello_handle]\n");
  rsp->setBody("Hello! Wecome to cnet!\n");
  rsp->setStatus(HttpStatus::OK);

  return 0;
}

void run() {
  HttpServer::SharedPtr serv(new HttpServer());

  serv->regist("/home", hello_handle);
  std::vector<Address::SharedPtr> addr = {};
  // Address::lookup(addr, "127.0.0.1:2020", AF_INET);
  addr.push_back(Address::lookupAny("127.0.0.1:3333", AF_INET));

  for (auto &it : addr) {
    spdlog::info("[addr: {}]\n", it->getStringAddress());
  }

  std::vector<Address::SharedPtr> fails = {};
  assert(serv->bind(addr, fails));

  serv->start();
  spdlog::info("[~run()]\n");
}

int main() {
  iom.push(&run);
  iom.start();

  return 0;
}