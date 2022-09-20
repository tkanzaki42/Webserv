#ifndef SRCS_SERVER_HTTPRESPONSE_HPP_
#define SRCS_SERVER_HTTPRESPONSE_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include "includes/webserv.hpp"
#include "srcs/server/HttpHeader.hpp"
#include "srcs/server/HttpBody.hpp"
#include "srcs/server/HttpRequest.hpp"
#include "srcs/server/Debug.hpp"

class HttpResponse {
 private:
    const HttpRequest&  request_;
    HttpHeader          header_;
    HttpBody            message_body_;
    std::string         response_;

    void        make_header_();
    void        make_message_body_();
 public:
    explicit HttpResponse(const HttpRequest& request);
    ~HttpResponse();
    HttpResponse(const HttpResponse &obj);
    HttpResponse &operator=(const HttpResponse &obj);

    void              make_response();
    const std::string &get_response();
};

#endif  // SRCS_SERVER_HTTPRESPONSE_HPP_
