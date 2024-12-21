#include <iostream>

#include "http.h"
#include "httpParser.h"
// HTTP 请求消息示例
char* httpRequest =
    "GET /search?q=chatgpt&lang=en#section2 HTTP/1.1\r\n"
    "Host: www.example.com\r\n"
    "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
    "(KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36\r\n"
    "Accept: "
    "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/"
    "apng,*/*;q=0.8\r\n"
    "Accept-Language: en-US,en;q=0.9\r\n"
    "Accept-Encoding: gzip, deflate, br\r\n"
    "Connection: keep-alive\r\n"
    "Cookie: sessionid=abc123; user=example_user; preferences=dark_mode\r\n"
    "\r\n";  // 空行表示头部结束

// HTTP 响应消息示例
char* httpResponse =
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

int main() {
  HttpParser parser_req(HttpParser::Type::REQUEST);
  HttpParser parser_rsp(HttpParser::Type::RESPONSE);

  std::cout << "[PARSER-REQUEST]\n";
  parser_req.parser(httpRequest, strlen(httpRequest));
  std::cout << "[PARSER-REPONSE]\n";
  parser_rsp.parser(httpResponse, strlen(httpResponse));

  return 0;
}