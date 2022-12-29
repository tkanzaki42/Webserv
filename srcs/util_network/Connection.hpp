#ifndef SRCS_UTIL_NETWORK_CONNECTION_HPP_
#define SRCS_UTIL_NETWORK_CONNECTION_HPP_

#include <iostream>
#include <string>
#include "srcs/server_request/HttpRequest.hpp"
#include "srcs/server/HttpResponse.hpp"

class Connection {
 private:
    int    accepted_fd_;
    time_t last_time_;
    int    pp_recv_[2];
    int    pp_send_[2];
    HttpRequest  request_;
    HttpResponse *response_;

 public:
    Connection();
    Connection(const Connection &obj);
    Connection &operator=(const Connection &obj);
    ~Connection();

    int    get_accepted_fd() const;
    void   set_accepted_fd(int accepted_fd);
    time_t get_last_time() const;
    void   set_last_time(time_t last_time);
    int    get_write_pipe();
    int    get_read_pipe();
    int    get_status_code();

    void   receive_from_pipe();
    void   send_to_pipe();
};

#endif  // SRCS_UTIL_NETWORK_CONNECTION_HPP_
