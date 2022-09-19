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
    Socket *sock;

    int recv_until_double_newline_(std::string &recv_str, int accept_fd);
    void read_contents_from_file_(int &is_file_exist,
            int &body_length, std::vector<std::string> &message_body);
    void create_response_(std::string &server_response,
            int body_length, std::vector<std::string> &message_body,
            int is_file_exist, HttpRequest &req);
    void print_debug_(std::string &recv_str, HttpRequest &req);
};

#endif  // SRCS_SERVER_WEBSERV_HPP_
