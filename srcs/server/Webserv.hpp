#ifndef SRCS_SERVER_WEBSERV_HPP_
#define SRCS_SERVER_WEBSERV_HPP_

#include <iostream>
#include <vector>
#include <string>

#include "srcs/util_network/Socket.hpp"
#include "srcs/server/HttpResponse.hpp"
#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/server/FDManager.hpp"

const int HTTP_PORT = 5000;

class Webserv {
 public:
    Webserv();
    ~Webserv();

    void  loop();
    const FDManager &get_fd_manager() const;
    const Socket    &get_socket() const;
 private:
    HttpRequest  request_;
    HttpResponse response_;
    Socket       socket_;
    FDManager    fd_manager_;
};

#endif  // SRCS_SERVER_WEBSERV_HPP_
