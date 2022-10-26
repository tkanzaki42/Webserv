#ifndef SRCS_SERVER_HTTPBODY_HPP_
#define SRCS_SERVER_HTTPBODY_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/server/HttpHeader.hpp"

class HttpBody {
 private :
    static const int         BUFFER_SIZE = 1024;

    const HttpRequest&       request_;
    std::vector<std::string> content_;
    std::ifstream            output_file_;
    // int                      body_content_length_;

    int                      read_contents_from_file_();
    void                     make_status_response_(int status_code);

 public:
    explicit HttpBody(const HttpRequest& request);
    ~HttpBody();
    HttpBody(const HttpBody &obj);
    HttpBody &operator=(const HttpBody &obj);

    int                            make_response(int status_code);
    const std::vector<std::string> &get_content();
    std::size_t                    get_content_length();
    void                           clear_contents();
};

#endif  // SRCS_SERVER_HTTPBODY_HPP_
