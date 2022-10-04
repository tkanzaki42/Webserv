#ifndef SRCS_SERVER_WEBSERV_HPP_
#define SRCS_SERVER_WEBSERV_HPP_

#include <iostream>
#include <vector>
#include <string>

#include "srcs/server/HttpResponse.hpp"
#include "srcs/util_network/FDManager.hpp"
#include "srcs/server_request/HttpRequest.hpp"

class Webserv {
 public:
    Webserv();
    ~Webserv();

    void init();
    void loop();
    void finalize();

 private:
    FDManager fd_manager_;
};

#endif  // SRCS_SERVER_WEBSERV_HPP_
