#ifndef SRCS_SERVER_HTTPBODY_HPP_
#define SRCS_SERVER_HTTPBODY_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "srcs/server/HttpHeader.hpp"

class HttpBody {
 private :
    static const int         BUFFER_SIZE = 1024;

    std::vector<std::string> content_;
    char                     read_file_buf_[HttpBody::BUFFER_SIZE];
    std::ifstream            output_file_;
    // int                      body_content_length_;

    std::string              get_error_description_(int status_code);
    void                     make_error_response_(int status_code);

 public:
    HttpBody();
    ~HttpBody();
    HttpBody(const HttpBody &obj);
    HttpBody &operator=(const HttpBody &obj);

    void                           make_response(int status_code);
    const std::vector<std::string> &get_content();
    void                           read_contents_from_file();
    std::size_t                    get_content_length();
    void                           clear_contents();
    // std::vector<std::string>& make_response404();
    // std::vector<std::string>& make_response302(std::string path);
    // std::vector<std::string>& make_responseUpgrade();
};

#endif  // SRCS_SERVER_HTTPBODY_HPP_
