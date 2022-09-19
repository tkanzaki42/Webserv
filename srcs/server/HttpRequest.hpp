#ifndef SRCS_SERVER_HTTPREQUEST_HPP_
#define SRCS_SERVER_HTTPREQUEST_HPP_

#include <string.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>

#include "includes/webserv.hpp"

// TODO(tkanzaki) 定数、列挙型はバッティングの可能性があるので
// クラス内にstatic constで定義の方がいいかも(?)
const std::string kBaseHtmlPath = "./public_html";
const std::string kIndexHtmlFileName = "index.html";

enum HttpMethod {
    NOT_DEFINED = 0,
    METHOD_POST = 1,
    METHOD_GET = 2,
    METHOD_DELETE = 3
};

class HttpRequest {
 public:
    HttpMethod  http_method;
    std::string request_path;
    std::string path_to_file;
    std::string http_ver;
    std::map<std::string, std::string> header_field;

    HttpRequest();
    ~HttpRequest();
    HttpRequest(const HttpRequest &obj);
    HttpRequest& operator=(const HttpRequest &obj);

    const std::string& get_received_line();
    void set_accept_fd(int accept_fd);
    int  recv_until_double_newline_();
    void analyze_request();
    void print_debug();

 private:
    std::string received_line_;
    int         accept_fd_;

    std::size_t skip_space_(std::size_t read_idx);
    std::size_t skip_crlf_(std::size_t read_idx);
    std::size_t parse_method_(std::size_t read_idx);
    std::size_t parse_request_path_(std::size_t read_idx);
    std::size_t parse_http_ver_(std::size_t read_idx);
    std::size_t parse_header_field_(std::size_t read_idx);
    std::size_t parse_one_header_field_(std::size_t read_idx);
    void rtrim_(std::string &str);
    void generate_path_to_file_();
};

#endif  // SRCS_SERVER_HTTPREQUEST_HPP_
