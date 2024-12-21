#pragma once
#include "http_parser.h"
#include "http.h"
#include <cstddef>
#include <memory>
#include <string>
#include <sys/types.h>
#include <variant>

class HttpParser
{
public:

    enum class Type{
        REQUEST = HTTP_REQUEST,
        RESPONSE = HTTP_RESPONSE
    };

    HttpParser(HttpParser::Type type);
    ~HttpParser();

    ssize_t parser(char* data, size_t len);
    ssize_t parser(const std::string data);

    Type getType();

    template<class T>
    std::shared_ptr<T> getData();
    
    void __setCurrentFiled(const std::string& filed);
    std::string  __getCurrentFiled();

    void __setFinish(bool finish);
    bool  __getFinish();
    
private:
    struct http_parser Parser;
    std::variant<std::shared_ptr<HttpRequest>, std::shared_ptr<HttpResponse>> Package;
    Type ParserType;

    HttpErrno Error;
    bool Finish;

    std::string CurrentFiled;
};

class HttpResponseParser
{
public:
HttpResponseParser();
bool parser(const char* data, size_t len);
bool parser(const std::string data);
private:
    struct http_parser Parser;
    HttpResponse Response;

    bool finish;
    HttpErrno Error;
};