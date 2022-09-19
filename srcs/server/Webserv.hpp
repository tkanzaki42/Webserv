#ifndef SRCS_SERVER_WEBSERV_HPP_
#define SRCS_SERVER_WEBSERV_HPP_

#include <iostream>
#include <vector>
#include <string>

#include "srcs/util_network/Socket.hpp"
#include "srcs/server/HttpResponse.hpp"
#include "srcs/server/HttpRequest.hpp"

const int HTTP_PORT = 5000;

class Webserv {
 public:
    Webserv() {}
    ~Webserv() {}

    int init();
    void loop();
    int finalize();

 private:
    Socket                   *sock;
    HttpRequest              request_;
    HttpResponse             response_;

    int recv_until_double_newline_();
    void create_response_();
    void print_debug_();
};

#endif  // SRCS_SERVER_WEBSERV_HPP_
