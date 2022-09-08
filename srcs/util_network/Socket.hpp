#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <iostream>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

class Socket {
public:
    Socket(int port): port_(port) {}
    ~Socket() {}

    int prepare();
    int cleanup();
    int get_listen_fd() const { return listen_fd_; }

private:
    int                 listen_fd_;
    int                 port_;
    struct sockaddr_in  serv_addr_;

    int open_socket_();
    int bind_address_();
    int listen_();
};

#endif
