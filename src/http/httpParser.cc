#include "httpParser.h"

#include <string.h>
#include <sys/types.h>

#include <cstddef>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <type_traits>

#include "http.h"
#include "http_parser.h"

template <class T>
std::shared_ptr<T> HttpParser::getData() {
  return nullptr;
}
template <>
std::shared_ptr<HttpRequest> HttpParser::getData<HttpRequest>() {
  if (ParserType == Type::REQUEST) {
    return std::get<std::shared_ptr<HttpRequest>>(Package);
  }

  return nullptr;
}
template <>
std::shared_ptr<HttpResponse> HttpParser::getData<HttpResponse>() {
  if (ParserType == Type::RESPONSE) {
    return std::get<std::shared_ptr<HttpResponse>>(Package);
  }

  return nullptr;
}

HttpParser::Type HttpParser::getType() { return ParserType; }

void HttpParser::__setFinish(bool finish) { Finish = finish; }

bool HttpParser::__getFinish() { return Finish; }

namespace util {

int on_begin_callback(http_parser *parser) {
  std::cout << "begin!\n";
  return 0;
}
int on_url_callback(http_parser *parser, const char *buf, size_t size) {
  HttpParser *obj = static_cast<HttpParser *>(parser->data);

  switch (obj->getType()) {
    case HttpParser::Type::REQUEST: {
      auto request = obj->getData<HttpRequest>();

      std::string s(buf, size);
      std::cout << "[url: " << s << "]\n";
      http_parser_url url_parser;
      http_parser_url_init(&url_parser);
      int rt = http_parser_parse_url(buf, size, 0, &url_parser);
      if (rt != 0) {
        std::cout << "[ERROR: URL PARSER IS WRONG!\n]";
        return -1;
      }

      if (url_parser.field_set & (1 << UF_PATH)) {
        request->setPath(std::string(buf + url_parser.field_data[UF_PATH].off,
                                     url_parser.field_data[UF_PATH].len));
        std::cout << "[PARSER: URL-PATH IS OK!]\n";
      } else if (url_parser.field_set & (1 << UF_QUERY)) {
        request->setQuery(std::string(buf + url_parser.field_data[UF_PATH].off,
                                      url_parser.field_data[UF_PATH].len));
        std::cout << "[PARSER: URL-QUERY IS OK!]\n";
      } else if (url_parser.field_set & (1 << UF_FRAGMENT)) {
        request->setFragment(
            std::string(buf + url_parser.field_data[UF_PATH].off,
                        url_parser.field_data[UF_PATH].len));
        std::cout << "[PARSER: URL-FRAGMENT IS OK!]\n";
      }
      break;
    }
    case HttpParser::Type::RESPONSE:
      std::cout << "[Parser-request: url parser over!]\n";
      break;
  }

  return 0;
}

int on_status_callback(http_parser *parser, const char *buf, size_t size) {
  HttpParser *obj = static_cast<HttpParser *>(parser->data);

  switch (obj->getType()) {
    case HttpParser::Type::RESPONSE:
      std::string status(buf, size);
      std::cout << "[Parser-status: " << status << "]\n";
      auto response = obj->getData<HttpResponse>();
      response->setStatus((HttpStatus)parser->status_code);
      break;
  }

  return 0;
}
int on_header_filed_callback(http_parser *parser, const char *buf,
                             size_t size) {
  std::string filed(buf, size);
  std::cout << "[header-filed:" << filed << "]\n";

  HttpParser *obj = static_cast<HttpParser *>(parser->data);

  obj->__setCurrentFiled(filed);

  return 0;
}
int on_header_value_callback(http_parser *parser, const char *buf,
                             size_t size) {
  std::string value(buf, size);
  std::cout << "[header-value:" << value << "]\n";

  HttpParser *obj = static_cast<HttpParser *>(parser->data);
  switch (obj->getType()) {
    case HttpParser::Type::REQUEST: {
      auto request = obj->getData<HttpRequest>();
      request->setHeader(obj->__getCurrentFiled(), value);
      break;
    }
    case HttpParser::Type::RESPONSE: {
      auto response = obj->getData<HttpResponse>();
      response->setHeader(obj->__getCurrentFiled(), value);
      break;
    }
  }

  return 0;
}
int on_header_complete_callback(http_parser *parser) {
  HttpParser *obj = static_cast<HttpParser *>(parser->data);
  switch (obj->getType()) {
    case HttpParser::Type::REQUEST: {
      auto request = obj->getData<HttpRequest>();
      request->setVersion(parser->http_major);
      break;
    }
    case HttpParser::Type::RESPONSE: {
      auto response = obj->getData<HttpResponse>();
      response->setVersion(parser->http_major);
      break;
    }
  }
  return 0;
}
int on_body_callback(http_parser *parser, const char *buf, size_t size) {
  std::string body(buf, size);
  std::cout << "\n[body]:\n" << body << "\n";

  HttpParser *obj = static_cast<HttpParser *>(parser->data);
  switch (obj->getType()) {
    case HttpParser::Type::REQUEST: {
      auto request = obj->getData<HttpRequest>();
      request->setBody(body);
      break;
    }
    case HttpParser::Type::RESPONSE: {
      auto response = obj->getData<HttpResponse>();
      response->setBody(body);
      break;
    }
  }
  return 0;
}
int on_message_complete_callback(http_parser *parser) {
  HttpParser *obj = static_cast<HttpParser *>(parser->data);
  obj->__setFinish(true);

  return 0;
}

int on_chunk_header_callback(http_parser *p) { return 0; }

int on_chunk_complete_callback(http_parser *p) { return 0; }

};  // namespace util

void HttpParser::__setCurrentFiled(const std::string &filed) {
  CurrentFiled = filed;
}

std::string HttpParser::__getCurrentFiled() { return CurrentFiled; }

HttpParser::HttpParser(HttpParser::Type type) : Parser(), Error(), Finish() {
  http_parser_init(&Parser, (http_parser_type)type);
  Parser.data = this;

  ParserType = type;
  if (type == Type::REQUEST) {
    Package = std::make_shared<HttpRequest>();
  } else {
    Package = std::make_shared<HttpResponse>();
  }
}
HttpParser::~HttpParser() {}

static struct http_parser_settings __http_setting {
  .on_message_begin = util::on_begin_callback, .on_url = util::on_url_callback,
  .on_status = util::on_status_callback,
  .on_header_field = util::on_header_filed_callback,
  .on_header_value = util::on_header_value_callback,
  .on_headers_complete = util::on_header_complete_callback,
  .on_body = util::on_body_callback,
  .on_message_complete = util::on_message_complete_callback,
  .on_chunk_header = util::on_chunk_header_callback,
  .on_chunk_complete = util::on_chunk_complete_callback
};

ssize_t HttpParser::parser(char *data, size_t len) {
  size_t nparsed = http_parser_execute(&Parser, &__http_setting, data, len);

  if (Parser.http_errno != 0) {
    Error = (HttpErrno)Parser.http_errno;
    std::cout << "[ERROR: PARSER IS WRONG! | error: "
              << http_errno_name(http_errno(Parser.http_errno)) << "]\n";
    return -1;
  } else if (nparsed < len) {
    memmove(data, data + nparsed, len - nparsed);
  }

  return nparsed;
}

ssize_t HttpParser::parser(std::string data) {
  return parser(const_cast<char *>(data.c_str()), data.size());
}
