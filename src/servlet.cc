#include "servlet.h"

#include <iostream>
#include <memory>

namespace util {

void __defaultServlet(HttpRequest::SharedPtr req, HttpResponse::SharedPtr rsp,
                      HttpSession::SharedPtr session) {
  rsp->setBody("Hello, We come to Cnet!\n");
}

}  // namespace util

ServletManager::ServletManager() : handleGroup(), servletMutex() {}

ServletManager::~ServletManager() {
  for (auto &handle : handleGroup) {
    // std::cout << handle.second.use_count();
    handle.second.reset();
  }
}

FunctionServlet::FunctionServlet(Servlet::HandleType handle)
    : callback(handle) {}

int FunctionServlet::handle(HttpRequest::SharedPtr request,
                            HttpResponse::SharedPtr response,
                            HttpSession::SharedPtr session) {
  callback(request, response, session);
  std::cout << "[count: req-" << request.use_count() << "\trsp-"
            << response.use_count() << "\tsession-" << session.use_count()
            << "]\n";
  return 0;
}
void ServletManager::addServlet(const std::string &uri,
                                Servlet::HandleType handle) {
  std::lock_guard<std::mutex> lock(servletMutex);

  handleGroup[uri] = ServletCreator::create<FunctionServlet>(handle);
}
void ServletManager::addServlet(const std::string &uri,
                                const Servlet::SharedPtr &handle) {
  std::lock_guard<std::mutex> lock(servletMutex);

  handleGroup[uri] = handle;
}
Servlet::SharedPtr ServletManager::getServlet(const std::string &uri) {
  std::lock_guard<std::mutex> lock(servletMutex);

  auto handleIt = handleGroup.find(uri);
  if (handleIt == handleGroup.end()) {
    return nullptr;
  }
  return handleIt->second;
}

int ServletManager::handle(HttpRequest::SharedPtr request,
                           HttpResponse::SharedPtr response,
                           HttpSession::SharedPtr session) {
  std::string uri = request->getPath();
  if (uri.empty()) {
    return 1;
  }

  auto servlet = getServlet(uri);
  if (servlet == nullptr) {
    return -1;
  }
  servlet->handle(request, response, session);

  return 0;
}