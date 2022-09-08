#ifndef WEBSERV_HPP
#define WEBSERV_HPP

#include "../util_network/Socket.hpp"
#include "HttpResponse.hpp"

#include <iostream>
#include <vector>

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

    int read_until_double_newline_(std::string &recv_str, char buf[BUF_SIZE], int accept_fd);
    void get_request_path_(std::string &path, std::string &path_string);
    void read_contents_from_file_(int &is_file_exist,
            int &body_length, std::vector<std::string> &message_body);
    void create_response_(std::string &server_response,
            int body_length, std::vector<std::string> &message_body,
            int is_file_exist, std::string &path);
};

#endif
