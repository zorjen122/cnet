#pragma once
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/* Request Methods */
#define HTTP_METHOD_MAP(FUNC)        \
  FUNC(0, DELETE, DELETE)            \
  FUNC(1, GET, GET)                  \
  FUNC(2, HEAD, HEAD)                \
  FUNC(3, POST, POST)                \
  FUNC(4, PUT, PUT)                  \
  /* pathological */                 \
  FUNC(5, CONNECT, CONNECT)          \
  FUNC(6, OPTIONS, OPTIONS)          \
  FUNC(7, TRACE, TRACE)              \
  /* WebDAV */                       \
  FUNC(8, COPY, COPY)                \
  FUNC(9, LOCK, LOCK)                \
  FUNC(10, MKCOL, MKCOL)             \
  FUNC(11, MOVE, MOVE)               \
  FUNC(12, PROPFIND, PROPFIND)       \
  FUNC(13, PROPPATCH, PROPPATCH)     \
  FUNC(14, SEARCH, SEARCH)           \
  FUNC(15, UNLOCK, UNLOCK)           \
  FUNC(16, BIND, BIND)               \
  FUNC(17, REBIND, REBIND)           \
  FUNC(18, UNBIND, UNBIND)           \
  FUNC(19, ACL, ACL)                 \
  /* subversion */                   \
  FUNC(20, REPORT, REPORT)           \
  FUNC(21, MKACTIVITY, MKACTIVITY)   \
  FUNC(22, CHECKOUT, CHECKOUT)       \
  FUNC(23, MERGE, MERGE)             \
  /* upnp */                         \
  FUNC(24, MSEARCH, M - SEARCH)      \
  FUNC(25, NOTIFY, NOTIFY)           \
  FUNC(26, SUBSCRIBE, SUBSCRIBE)     \
  FUNC(27, UNSUBSCRIBE, UNSUBSCRIBE) \
  /* RFC-5789 */                     \
  FUNC(28, PATCH, PATCH)             \
  FUNC(29, PURGE, PURGE)             \
  /* CalDAV */                       \
  FUNC(30, MKCALENDAR, MKCALENDAR)   \
  /* RFC-2068, section 19.6.1.2 */   \
  FUNC(31, LINK, LINK)               \
  FUNC(32, UNLINK, UNLINK)           \
  /* icecast */                      \
  FUNC(33, SOURCE, SOURCE)

/* Status Codes */
#define HTTP_STATUS_MAP(FUNC)                                                 \
  FUNC(100, CONTINUE, Continue)                                               \
  FUNC(101, SWITCHING_PROTOCOLS, Switching Protocols)                         \
  FUNC(102, PROCESSING, Processing)                                           \
  FUNC(200, OK, OK)                                                           \
  FUNC(201, CREATED, Created)                                                 \
  FUNC(202, ACCEPTED, Accepted)                                               \
  FUNC(203, NON_AUTHORITATIVE_INFORMATION, Non - Authoritative Information)   \
  FUNC(204, NO_CONTENT, No Content)                                           \
  FUNC(205, RESET_CONTENT, Reset Content)                                     \
  FUNC(206, PARTIAL_CONTENT, Partial Content)                                 \
  FUNC(207, MULTI_STATUS, Multi - Status)                                     \
  FUNC(208, ALREADY_REPORTED, Already Reported)                               \
  FUNC(226, IM_USED, IM Used)                                                 \
  FUNC(300, MULTIPLE_CHOICES, Multiple Choices)                               \
  FUNC(301, MOVED_PERMANENTLY, Moved Permanently)                             \
  FUNC(302, FOUND, Found)                                                     \
  FUNC(303, SEE_OTHER, See Other)                                             \
  FUNC(304, NOT_MODIFIED, Not Modified)                                       \
  FUNC(305, USE_PROXY, Use Proxy)                                             \
  FUNC(307, TEMPORARY_REDIRECT, Temporary Redirect)                           \
  FUNC(308, PERMANENT_REDIRECT, Permanent Redirect)                           \
  FUNC(400, BAD_REQUEST, Bad Request)                                         \
  FUNC(401, UNAUTHORIZED, Unauthorized)                                       \
  FUNC(402, PAYMENT_REQUIRED, Payment Required)                               \
  FUNC(403, FORBIDDEN, Forbidden)                                             \
  FUNC(404, NOT_FOUND, Not Found)                                             \
  FUNC(405, METHOD_NOT_ALLOWED, Method Not Allowed)                           \
  FUNC(406, NOT_ACCEPTABLE, Not Acceptable)                                   \
  FUNC(407, PROXY_AUTHENTICATION_REQUIRED, Proxy Authentication Required)     \
  FUNC(408, REQUEST_TIMEOUT, Request Timeout)                                 \
  FUNC(409, CONFLICT, Conflict)                                               \
  FUNC(410, GONE, Gone)                                                       \
  FUNC(411, LENGTH_REQUIRED, Length Required)                                 \
  FUNC(412, PRECONDITION_FAILED, Precondition Failed)                         \
  FUNC(413, PAYLOAD_TOO_LARGE, Payload Too Large)                             \
  FUNC(414, URI_TOO_LONG, URI Too Long)                                       \
  FUNC(415, UNSUPPORTED_MEDIA_TYPE, Unsupported Media Type)                   \
  FUNC(416, RANGE_NOT_SATISFIABLE, Range Not Satisfiable)                     \
  FUNC(417, EXPECTATION_FAILED, Expectation Failed)                           \
  FUNC(421, MISDIRECTED_REQUEST, Misdirected Request)                         \
  FUNC(422, UNPROCESSABLE_ENTITY, Unprocessable Entity)                       \
  FUNC(423, LOCKED, Locked)                                                   \
  FUNC(424, FAILED_DEPENDENCY, Failed Dependency)                             \
  FUNC(426, UPGRADE_REQUIRED, Upgrade Required)                               \
  FUNC(428, PRECONDITION_REQUIRED, Precondition Required)                     \
  FUNC(429, TOO_MANY_REQUESTS, Too Many Requests)                             \
  FUNC(431, REQUEST_HEADER_FIELDS_TOO_LARGE, Request Header Fields Too Large) \
  FUNC(451, UNAVAILABLE_FOR_LEGAL_REASONS, Unavailable For Legal Reasons)     \
  FUNC(500, INTERNAL_SERVER_ERROR, Internal Server Error)                     \
  FUNC(501, NOT_IMPLEMENTED, Not Implemented)                                 \
  FUNC(502, BAD_GATEWAY, Bad Gateway)                                         \
  FUNC(503, SERVICE_UNAVAILABLE, Service Unavailable)                         \
  FUNC(504, GATEWAY_TIMEOUT, Gateway Timeout)                                 \
  FUNC(505, HTTP_VERSION_NOT_SUPPORTED, HTTP Version Not Supported)           \
  FUNC(506, VARIANT_ALSO_NEGOTIATES, Variant Also Negotiates)                 \
  FUNC(507, INSUFFICIENT_STORAGE, Insufficient Storage)                       \
  FUNC(508, LOOP_DETECTED, Loop Detected)                                     \
  FUNC(510, NOT_EXTENDED, Not Extended)                                       \
  FUNC(511, NETWORK_AUTHENTICATION_REQUIRED, Network Authentication Required)

/* Map for errno-related constants
 *
 * The provided argument should be a macro that takes 2 arguments.
 */
#define HTTP_ERRNO_MAP(XX)                                                 \
  /* No error */                                                           \
  XX(OK, "success")                                                        \
                                                                           \
  /* Callback-related errors */                                            \
  XX(CB_message_begin, "the on_message_begin callback failed")             \
  XX(CB_url, "the on_url callback failed")                                 \
  XX(CB_header_field, "the on_header_field callback failed")               \
  XX(CB_header_value, "the on_header_value callback failed")               \
  XX(CB_headers_complete, "the on_headers_complete callback failed")       \
  XX(CB_body, "the on_body callback failed")                               \
  XX(CB_message_complete, "the on_message_complete callback failed")       \
  XX(CB_status, "the on_status callback failed")                           \
  XX(CB_chunk_header, "the on_chunk_header callback failed")               \
  XX(CB_chunk_complete, "the on_chunk_complete callback failed")           \
                                                                           \
  /* Parsing-related errors */                                             \
  XX(INVALID_EOF_STATE, "stream ended at an unexpected time")              \
  XX(HEADER_OVERFLOW, "too many header bytes seen; overflow detected")     \
  XX(CLOSED_CONNECTION,                                                    \
     "data received after completed connection: close message")            \
  XX(INVALID_VERSION, "invalid HTTP version")                              \
  XX(INVALID_STATUS, "invalid HTTP status code")                           \
  XX(INVALID_METHOD, "invalid HTTP method")                                \
  XX(INVALID_URL, "invalid URL")                                           \
  XX(INVALID_HOST, "invalid host")                                         \
  XX(INVALID_PORT, "invalid port")                                         \
  XX(INVALID_PATH, "invalid path")                                         \
  XX(INVALID_QUERY_STRING, "invalid query string")                         \
  XX(INVALID_FRAGMENT, "invalid fragment")                                 \
  XX(LF_EXPECTED, "LF character expected")                                 \
  XX(INVALID_HEADER_TOKEN, "invalid character in header")                  \
  XX(INVALID_CONTENT_LENGTH, "invalid character in content-length header") \
  XX(UNEXPECTED_CONTENT_LENGTH, "unexpected content-length header")        \
  XX(INVALID_CHUNK_SIZE, "invalid character in chunk size header")         \
  XX(INVALID_CONSTANT, "invalid constant string")                          \
  XX(INVALID_INTERNAL_STATE, "encountered unexpected internal state")      \
  XX(STRICT, "strict mode assertion failed")                               \
  XX(PAUSED, "parser is paused")                                           \
  XX(UNKNOWN, "an unknown error occurred")

/* Define HPE_* values for each errno value above */
enum class HttpErrno {
#define HTTP_ERRNO_GEN(n, s) HPE_##n,
  HTTP_ERRNO_MAP(HTTP_ERRNO_GEN)
#undef HTTP_ERRNO_GEN
};

/* Get an http_errno value from an http_parser */
#define HTTP_PARSER_ERRNO(p) ((enum http_errno)(p)->http_errno)

/**
 * @brief HTTP方法枚举
 */
enum class HttpMethod {
#define FUNC(num, name, string) name = num,
  HTTP_METHOD_MAP(FUNC)
#undef FUNC
      INVALID_METHOD
};

extern const char* __HttpMethodFiledGroup[];

extern const char* __HttpStatusReasonGroup[];
extern const char* __HttpParserErrnoGroup[];

/**
 * @brief HTTP状态枚举
 */
enum class HttpStatus {
#define FUNC(code, name, desc) name = code,
  HTTP_STATUS_MAP(FUNC)
#undef FUNC
};

const char* MethodToString(HttpMethod method);
const char* StatusToString(HttpStatus status);
const char* ParserErrorToString(HttpErrno err);

class FieldComparator {
 public:
  inline bool operator()(const std::string& a, const std::string& b) const {
    return strcasecmp(a.c_str(), b.c_str()) < 0;
  }
};
class HttpRequest {
 public:
  using SharedPtr = std::shared_ptr<HttpRequest>;
  using MapType = std::map<std::string, std::string, FieldComparator>;

 public:
  HttpRequest();

  HttpRequest& setPath(const std::string& path);
  HttpRequest& setQuery(const std::string& query);
  HttpRequest& setFragment(const std::string& fragment);
  HttpRequest& setBody(const std::string& body);
  HttpRequest& setMethod(HttpMethod method);
  HttpRequest& setHeader(const std::string& key, const std::string& value);
  HttpRequest& setVersion(int version);

  std::string getPath();
  std::string getQuery();
  std::string getFragment();
  std::string getBody();
  HttpMethod getMethod();
  std::string getHandle(const std::string& filed);

  int getVersion();

  std::string format();

  bool isClose();

 private:
  uint8_t Version : 1;
  bool OnWebSocket;

  bool IsClose;
  uint8_t ParserParamFlag;

  HttpMethod Method;

  std::string Path;
  std::string Query;
  std::string Fragment;
  std::string Body;

  MapType Header;
  MapType Param;
  MapType Cookie;
};
class HttpResponse {
 public:
  using SharedPtr = std::shared_ptr<HttpResponse>;
  using MapType = std::map<std::string, std::string, FieldComparator>;

 public:
  HttpResponse();

  HttpResponse& setReason(const std::string& reason);
  HttpResponse& setBody(const std::string& body);
  HttpResponse& setHeader(const std::string& key, const std::string& value);
  HttpResponse& setStatus(HttpStatus status);
  HttpResponse& setCookie(const std::string& cookie);
  HttpResponse& setVersion(int version);

  std::string getReason();
  std::string getBody();
  std::string getHeader(const std::string& filed);
  HttpStatus getStatus();
  int getVersion();

  std::string format();

 private:
  // 版本
  uint8_t Version : 1;
  bool OnWebSocket;

  // 响应状态
  HttpStatus Status;
  // 是否自动关闭
  bool IsClose;
  // 响应消息体
  std::string Body;
  // 响应原因
  std::string Reason;
  // 响应头部MAP
  MapType Header;

  std::vector<std::string> Cookies;
};
