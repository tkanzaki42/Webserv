#ifndef SRCS_SERVER_HTTPRESPONSE_HPP_
#define SRCS_SERVER_HTTPRESPONSE_HPP_

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>

#include "includes/webserv.hpp"
#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/server/HttpHeader.hpp"
#include "srcs/server/HttpBody.hpp"
#include "srcs/server/CGI.hpp"
#include "srcs/util/PathUtil.hpp"

class HttpResponse {
 private:
    HttpRequest         *request_;
    HttpHeader          header_;
    HttpBody            message_body_;
    CGI                 *cgi_;
    FileType            file_type_;
    std::string         response_;
    int                 status_code_;

    void                make_message_body_();
    void                make_header_();
    void                merge_header_and_body_();

 public:
    explicit HttpResponse(HttpRequest *request);
    ~HttpResponse();
    HttpResponse(const HttpResponse &obj);
    HttpResponse &operator=(const HttpResponse &obj);

    void                make_response();
    const std::string&  get_response();
};

#endif  // SRCS_SERVER_HTTPRESPONSE_HPP_
