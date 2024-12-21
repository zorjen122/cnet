#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <tuple>
#include <unordered_map>

#include "http.h"
#include "httpSession.h"

class Servlet {
 public:
  using SharedPtr = std::shared_ptr<Servlet>;
  using HandleType = std::function<int(
      HttpRequest::SharedPtr, HttpResponse::SharedPtr, HttpSession::SharedPtr)>;

  virtual int handle(HttpRequest::SharedPtr request,
                     HttpResponse::SharedPtr response,
                     HttpSession::SharedPtr session) = 0;

  Servlet() = default;
  virtual ~Servlet() = default;
};

class FunctionServlet : public Servlet {
 public:
  using SharedPtr = std::shared_ptr<FunctionServlet>;

  FunctionServlet(Servlet::HandleType handle);
  virtual ~FunctionServlet() = default;
  int handle(HttpRequest::SharedPtr request, HttpResponse::SharedPtr response,
             HttpSession::SharedPtr session) override;

 private:
  Servlet::HandleType callback;
};

class IServletCreator {
 public:
  using SharedPtr = std::shared_ptr<IServletCreator>;

  IServletCreator() = default;
  virtual ~IServletCreator() = default;
  virtual Servlet::SharedPtr create() = 0;
};

class ServletCreator : public IServletCreator {
 public:
  using SharedPtr = std::shared_ptr<ServletCreator>;

  ServletCreator() = default;
  virtual ~ServletCreator() = default;

  template <class T>
  static Servlet::SharedPtr create() {
    return Servlet::SharedPtr(new T);
  }

  template <class T, class... Args>
  static Servlet::SharedPtr create(Args &&... args) {
    return Servlet::SharedPtr(new T(std::forward<Args>(args)...));
  }
};

class ServletManager {
 public:
  using SharedPtr = std::shared_ptr<ServletManager>;

  ServletManager();
  ~ServletManager();

  void addServlet(const std::string &uri, Servlet::HandleType handle);
  void addServlet(const std::string &uri, const Servlet::SharedPtr &handle);
  Servlet::SharedPtr getServlet(const std::string &uri);

  int handle(HttpRequest::SharedPtr request, HttpResponse::SharedPtr response,
             HttpSession::SharedPtr session);

 private:
  std::unordered_map<std::string, Servlet::SharedPtr> handleGroup;
  Servlet::SharedPtr defaultHandle;
  std::mutex servletMutex;
};