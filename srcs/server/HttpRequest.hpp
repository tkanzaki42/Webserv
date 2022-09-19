#ifndef SRCS_SERVER_HTTPREQUEST_HPP_
#define SRCS_SERVER_HTTPREQUEST_HPP_

#include <string.h>
#include <iostream>
#include <string>

#include "includes/webserv.hpp"

const std::string kBaseHtmlPath = "./public_html";
const std::string kIndexHtmlFileName = "index.html";

enum HttpMethod {
    METHOD_POST = 1,
    METHOD_GET = 2,
    METHOD_DELETE = 3
};

class HttpRequest {
 public:
    HttpMethod  method;
    std::string request_path;
    std::string path_to_file;
    std::string recieved_line_;
    int         accept_fd_;

    HttpRequest() {}
    ~HttpRequest() {}

    const std::string &get_received_line();
    void set_accept_fd(int accept_fd);
    void analyze_request();
    int  recv_until_double_newline_();

 private:
    std::size_t skip_space_(std::size_t read_idx);
    std::size_t parse_method_(std::size_t read_idx);
    std::size_t parse_request_path_(std::size_t read_idx);
    void generate_path_to_file_();
};

#endif  // SRCS_SERVER_HTTPREQUEST_HPP_
