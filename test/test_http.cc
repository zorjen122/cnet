#include <iostream>

#include "http.h"
#include "http_parser.h"
int begin_callback(http_parser* parser) {
  std::cout << "begin!\n";
  return 0;
}
int url_callback(http_parser* parser, const char* buf, size_t size) {
  std::string s(buf, size);
  std::cout << "url: " << s << "\n";
  return 0;
}

int status_callback(http_parser* parser, const char* buf, size_t size) {
  std::string s(buf, size);
  std::cout << "status:" << s << "\n";
  return 0;
}
int headerfiled_callback(http_parser* parser, const char* buf, size_t size) {
  std::string s(buf, size);
  std::cout << "header-filed:" << s << "\n";
  return 0;
}
int headervalue_callback(http_parser* parser, const char* buf, size_t size) {
  std::string s(buf, size);
  std::cout << "header-value:" << s << "\n";
  return 0;
}
int headercomplete_callback(http_parser* parser) { return 0; }
int body_callback(http_parser* parser, const char* buf, size_t size) {
  std::string s(buf, size);
  std::cout << "body:" << s << "\n";
  return 0;
}
int messagecomplete_callback(http_parser* parser) { return 0; }

void test(const char* request) {
  http_parser_settings setting;
  http_parser_settings_init(&setting);

  setting.on_message_begin = begin_callback;
  setting.on_url = url_callback;
  setting.on_status = status_callback;
  setting.on_header_field = headerfiled_callback;
  setting.on_header_value = headervalue_callback;
  setting.on_headers_complete = headercomplete_callback;
  setting.on_body = body_callback;
  setting.on_message_complete = messagecomplete_callback;

  http_parser parser;

  http_parser_init(&parser, HTTP_REQUEST);

  size_t rt = http_parser_execute(&parser, &setting, request, strlen(request));
  if (rt != strlen(request)) {
    std::cout << "err!\n";
    throw;
  }
}

void test_request() {
  std::cout << "[TEST-HTTP-REQUEST]\n";
  HttpRequest req;
  req.setMethod(HttpMethod::GET)
      .setPath("/home")
      .setQuery("user=zorjen&pos=local")
      .setHeader("Content-Type", "Application/json")
      .setBody("{\"message\": \"Hello, HTTP/1.1\"}");

  auto val = req.format();
  std::cout << "[RETURN]\n" << val << "\n";

  std::cout << "[PARSER]\n";
  test(val.c_str());
}

int on_message_begin(http_parser* parser) { return 0; }

int on_status(http_parser* parser, const char* at, size_t length) {
  std::cout << "HTTP Status: " << std::string(at, length) << "\n";
  return 0;
}

int on_header_field(http_parser* parser, const char* at, size_t length) {
  std::cout << "Header-Field: " << std::string(at, length) << "\n";
  return 0;
}

int on_header_value(http_parser* parser, const char* at, size_t length) {
  std::cout << "Header-Value: " << std::string(at, length) << "\n";
  return 0;
}

int on_headers_complete(http_parser* parser) { return 0; }

int on_body(http_parser* parser, const char* at, size_t length) {
  std::cout << "Body: " << std::string(at, length) << "\n";
  return 0;
}

int on_message_complete(http_parser* parser) {
  std::cout << "HTTP message parsing completed."
            << "\n";
  return 0;
}

void test2(const std::string& http_response) {
  // 创建http_parser对象并初始化
  http_parser_settings settings;
  std::memset(&settings, 0, sizeof(settings));

  // 设置回调函数
  settings.on_message_begin = on_message_begin;
  settings.on_status = on_status;
  settings.on_header_field = on_header_field;
  settings.on_header_value = on_header_value;
  settings.on_headers_complete = on_headers_complete;
  settings.on_body = on_body;
  settings.on_message_complete = on_message_complete;

  http_parser parser;
  http_parser_init(&parser, HTTP_RESPONSE);

  // 解析响应
  size_t nparsed = http_parser_execute(
      &parser, &settings, http_response.c_str(), http_response.size());

  if (nparsed != http_response.size()) {
    std::cerr << "Error parsing HTTP response"
              << "\n";
  }
}

void test_response() {
  std::cout << "[TEST-HTTP-RESPONSE]\n";

  // 示例 HTTP 响应
  const std::string http_response =
      "HTTP/1.1 200 OK\r\n"
      "Date: Tue, 18 Dec 2024 12:00:00 GMT\r\n"
      "Server: Apache/2.4.46 (Unix)\r\n"
      "Content-Type: text/html; charset=UTF-8\r\n"
      "Content-Length: 160\r\n"
      "Set-Cookie: sessionid=xyz789; Path=/; HttpOnly\r\n"
      "Set-Cookie: preferences=light_mode; Path=/; Expires=Thu, 31 Dec 2025 "
      "23:59:59 GMT\r\n"
      "Connection: keep-alive\r\n"
      "\r\n"  // 空行表示头部结束
      "<html>\r\n"
      "<head><title>Example</title></head>\r\n"
      "<body>\r\n"
      "<h1>Welcome to Example.com</h1>\r\n"
      "<p>This is a sample HTML page.</p>\r\n"
      "</body>\r\n"
      "</html>\r\n";  // HTML 内容作为响应体;

  HttpResponse rsp;

  rsp.setStatus(HttpStatus::OK)
      .setHeader("Content-Type", "text/html")
      .setHeader("Connection", "keep-alive");
  std::cout << "[RETURN]\n" << rsp.format() << "\n";
  std::cout << "[PARSER]\n";
  //   test2(rsp.format());
  test2(http_response);
}

int main() {
  test_request();
  test_response();

  return 0;
}