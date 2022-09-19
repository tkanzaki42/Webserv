#ifndef SRCS_SERVER_HTTPREQUEST_HPP_
#define SRCS_SERVER_HTTPREQUEST_HPP_

#include <string.h>
#include <iostream>
#include <string>

#include "includes/webserv.hpp"

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
    HttpMethod method;
    std::string request_path;
    std::string path_to_file;

    HttpRequest() : method(NOT_DEFINED), request_path(""), path_to_file("") {}
    ~HttpRequest() {}

    void analyze_request(std::string &recv_str);

 private:
    std::size_t skip_space_(
            std::string &recv_str, std::size_t read_idx);
    std::size_t parse_method_(
            std::string &recv_str, std::size_t read_idx);
    std::size_t parse_request_path_(
            std::string &recv_str, std::size_t read_idx);
    void generate_path_to_file_();
};

#endif  // SRCS_SERVER_HTTPREQUEST_HPP_
