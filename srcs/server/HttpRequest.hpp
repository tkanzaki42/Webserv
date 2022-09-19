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
    HttpMethod  method;
    std::string request_path;
    std::string path_to_file;
    std::string http_ver;
    std::map<std::string, std::string> header_field;

    HttpRequest() : method(NOT_DEFINED), request_path(""), path_to_file("") {}
    ~HttpRequest() {}

    const std::string &get_received_line();
    void set_accept_fd(int accept_fd);
    void analyze_request();
    void print_debug_();

 private:
    std::size_t skip_space_(
            std::string &recv_str, std::size_t read_idx);
    std::size_t skip_crlf_(
            std::string &recv_str, std::size_t read_idx);
    std::size_t parse_method_(
            std::string &recv_str, std::size_t read_idx);
    std::size_t parse_request_path_(
            std::string &recv_str, std::size_t read_idx);
    std::size_t parse_http_ver_(
            std::string &recv_str, std::size_t read_idx);
    std::size_t parse_header_field_(
            std::string &recv_str, std::size_t read_idx);
    std::size_t parse_one_header_field_(
            std::string &recv_str, std::size_t read_idx);
    void generate_path_to_file_();
    void rtrim_(std::string &str);
};

#endif  // SRCS_SERVER_HTTPREQUEST_HPP_
