#include "http.h"

#include <strings.h>

#include <iostream>
#include <sstream>
#include <string>
const char *__HttpMethodFiledGroup[] = {
#define XX(a, aa, string) #string,
    HTTP_METHOD_MAP(XX)
#undef XX
};
const char *__HttpStatusReasonGroup[] = {
#define XX(a, aa, string) #string,
    HTTP_STATUS_MAP(XX)
#undef XX
};

const char *__HttpParserErrnoGroup[] = {
#define XX(a, string) #string,
    HTTP_ERRNO_MAP(XX)
#undef XX
};

const char *MethodToString(HttpMethod method) {
  if (int(method) >=
      (sizeof(__HttpMethodFiledGroup) / sizeof(__HttpMethodFiledGroup[0]))) {
    return "<unknow>";
  }

  return __HttpMethodFiledGroup[int(method)];
}
const char *StatusToString(HttpStatus status) {
  switch (status) {
#define XX(a, name, string) \
  case HttpStatus::name:    \
    return #string;
    HTTP_STATUS_MAP(XX);
#undef XX
    default:
      break;
  }
  return "<unknown>";
}

const char *ParserErrorToString(HttpErrno err) {
  if (int(err) >=
      (sizeof(__HttpParserErrnoGroup) / sizeof(__HttpParserErrnoGroup[0]))) {
    return "<unknow>";
  }
  return __HttpParserErrnoGroup[int(err)];
}

HttpRequest::HttpRequest()
    : Header(),
      Param(),
      Cookie(),
      Query(),
      Fragment(),
      Path(),
      Body(),
      Method(),
      IsClose(),
      ParserParamFlag(),
      OnWebSocket() {}

HttpRequest &HttpRequest::setPath(const std::string &path) {
  Path = path;
  return *this;
}
HttpRequest &HttpRequest::setQuery(const std::string &query) {
  Query = query;
  return *this;
}
HttpRequest &HttpRequest::setFragment(const std::string &fragment) {
  Fragment = fragment;
  return *this;
}
HttpRequest &HttpRequest::setBody(const std::string &body) {
  Body.append(body);
  return *this;
}
HttpRequest &HttpRequest::setMethod(HttpMethod method) {
  Method = method;
  return *this;
}
HttpRequest &HttpRequest::setHeader(const std::string &key,
                                    const std::string &value) {
  Header[key] = value;
  return *this;
}

HttpRequest &HttpRequest::setVersion(int version) {
  Version = version;

  return *this;
}
std::string HttpRequest::getPath() { return Path; }
std::string HttpRequest::getQuery() { return Query; }
std::string HttpRequest::getFragment() { return Fragment; }
std::string HttpRequest::getBody() { return Body; }
HttpMethod HttpRequest::getMethod() { return Method; }

std::string HttpRequest::getHandle(const std::string &filed) {
  return Header[filed];
}

std::string HttpRequest::format() {
  std::ostringstream os{};

  os << ::MethodToString(Method) << " " << Path << (Query.empty() ? "" : "?")
     << Query << (Fragment.empty() ? " " : "#") << Fragment
     << (Version == 1 ? "HTTP/1.1" : "HTTP/2.0") << "\r\n";

  if (!OnWebSocket) {
    os << "Connection:" << (IsClose ? "close" : "keep-alive") << "\r\n";
  }
  for (auto &[k, v] : Header) {
    os << k << ":" << v << "\r\n";
  }

  if (!Body.empty()) {
    os << "Content-Length:" << Body.size() << "\r\n\r\n" << Body;
  } else {
    os << "\r\n";
  }
  return os.str();
}

bool HttpRequest::isClose() { return IsClose; }

HttpResponse::HttpResponse() : Cookies(), Header(), Body() {}

HttpResponse &HttpResponse::setReason(const std::string &reason) {
  Reason = reason;
  return *this;
}

HttpResponse &HttpResponse::setStatus(HttpStatus status) {
  Status = status;
  return *this;
}

HttpResponse &HttpResponse::setBody(const std::string &body) {
  Body.append(body);
  return *this;
}
HttpResponse &HttpResponse::setHeader(const std::string &key,
                                      const std::string &value) {
  Header[key] = value;
  return *this;
}

HttpResponse &HttpResponse::setCookie(const std::string &cookie) {
  Cookies.push_back(cookie);
  return *this;
}
HttpResponse &HttpResponse::setVersion(int version) {
  Version = version;
  return *this;
}

std::string HttpResponse::getReason() { return Reason; }
std::string HttpResponse::getBody() { return Body; }
std::string HttpResponse::getHeader(const std::string &filed) {
  return Header[filed];
}

HttpStatus HttpResponse::getStatus() { return Status; }
int HttpResponse::getVersion() { return Version; }

std::string HttpResponse::format() {
  std::ostringstream os{};
  os << (Version == 1 ? "HTTP/1.1" : "HTTP/2.0") << " " << int(Status) << " "
     << (Reason.empty() ? ::StatusToString(Status) : Reason) << "\r\n";

  if (!OnWebSocket) {
    os << "Connection:" << (IsClose ? "close" : "keep-alive") << "\r\n";
  }
  for (auto &[k, v] : Header) {
    os << k << ":" << v << "\r\n";
  }

  for (auto &cookie : Cookies) {
    os << "Set-Cookie:" << cookie << "\r\n";
  }

  if (!Body.empty()) {
    os << "Content-Length:" << Body.size() << "\r\n\r\n" << Body;
  } else {
    os << "\r\n";
  }
  return os.str();
}