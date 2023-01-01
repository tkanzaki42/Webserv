#ifndef SRCS_UTIL_NETWORK_CONNECTION_HPP_
#define SRCS_UTIL_NETWORK_CONNECTION_HPP_

#include <iostream>
#include <string>
#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/server/HttpResponse.hpp"

class Connection {
 private:
    int    accepted_fd_;
    int    port_;
    time_t last_time_;
    int    pp_recv_[2];
    HttpRequest  request_;
    HttpResponse response_;

 public:
    Connection();
    Connection(const Connection &obj);
    Connection &operator=(const Connection &obj);
    ~Connection();

    int    get_response_status_code();
    void   set_port(int port);
    int    get_accepted_fd() const;
    void   set_accepted_fd(int accepted_fd);
    time_t get_last_time() const;
    void   set_client_addr(struct sockaddr_in client_addr);
    void   set_last_time(time_t last_time);
    int    get_write_pipe();
    int    get_status_code();
    bool   receive_from_pipe();
    const std::string&
           get_response();
    void   set_response_status_code_(int status_code);
};

#endif  // SRCS_UTIL_NETWORK_CONNECTION_HPP_
