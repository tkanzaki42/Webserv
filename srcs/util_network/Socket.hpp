#ifndef SRCS_UTIL_NETWORK_SOCKET_HPP_
#define SRCS_UTIL_NETWORK_SOCKET_HPP_

// c includes
#include <unistd.h>
#include <string.h>
#include <errno.h>

// c network includes
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

// cpp includes
#include <iostream>

class Socket {
 public:
    explicit Socket(int port) : port_(port) {}
    ~Socket() {}

    int prepare();
    int cleanup();
    int get_listen_fd() const { return listen_fd_; }

    int accept();

 private:
    int                 listen_fd_;
    int                 port_;
    struct sockaddr_in  serv_addr_;
    socklen_t           serv_addr_len_;
    struct sockaddr_in  from_addr_;
    socklen_t           from_addr_len_;

    int open_socket_();
    int bind_address_();
    int listen_();
};

#endif  // SRCS_UTIL_NETWORK_SOCKET_HPP_
