#ifndef SRCS_SERVER_HTTPRESPONSE_HPP_
#define SRCS_SERVER_HTTPRESPONSE_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "includes/webserv.hpp"
#include "srcs/server/HttpHeader.hpp"

class HttpResponse {
 public:
    static std::string make_response(
            std::vector<std::string> &header,
            std::vector<std::string> &message_body);
    static std::vector<std::string>& make_header(
            int version, int body_length, int is_file_exist, std::string path);
    static void make_body(
            std::vector<std::string> &body_content,
            int &body_content_length,
            std::ifstream &output_file);
};

#endif  // SRCS_SERVER_HTTPRESPONSE_HPP_
