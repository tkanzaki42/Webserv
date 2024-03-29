// Copyright 2022 tkanzaki
#include "srcs/util_network/Socket.hpp"
#include "srcs/util/StringConverter.hpp"

Socket::Socket():
port_(0) {
}

Socket::Socket(int port):
port_(port) {
}

Socket::Socket(const Socket &obj) {
    listen_fd_     = obj.listen_fd_;
    port_          = obj.port_;
    serv_addr_     = obj.serv_addr_;
    serv_addr_len_ = obj.serv_addr_len_;
    from_addr_     = obj.from_addr_;
    from_addr_len_ = obj.from_addr_len_;
}

Socket::~Socket() {
}

void Socket::set_port(int port) {
    port_ = port;
}

int Socket::get_port() const {
    return (this->port_);
}

bool Socket::prepare() {
    serv_addr_len_ = sizeof(serv_addr_);
    from_addr_len_ = sizeof(from_addr_);
    if (open_socket_() == -1)
        return false;
    if (bind_address_() == -1)
        return false;
    if (listen_() == -1)
        return false;
    return true;
}

int Socket::cleanup() {
    close(listen_fd_);
    return 0;
}

int Socket::accept() {
    int fd = ::accept(listen_fd_,
        (struct sockaddr *)&from_addr_,
        &from_addr_len_);
    if (fd < 0) {
        _exit(1);
    }
    return fd;
}

struct sockaddr_in Socket::get_client_addr() {
    return from_addr_;
}

int Socket::open_socket_() {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ == -1) {
        std::cerr << "socket() failed." << std::endl;
        return -1;
    }

    int optval = 1;
    if (setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR,
            &optval, sizeof(optval)) == -1) {
        std::cerr << "setsockopt() failed." << std::endl;
        close(listen_fd_);
        return -1;
    }
    return 0;
}

int Socket::bind_address_() {
    StringConverter::ft_memset(&serv_addr_, 0, sizeof(serv_addr_));
    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons(port_);
    serv_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(listen_fd_, (struct sockaddr*)&serv_addr_,
            serv_addr_len_) == -1) {
        std::cerr << "bind() failed.(" << errno << ")" << std::endl;
        close(listen_fd_);
        return -1;
    }
    return 0;
}

int Socket::listen_() {
    if (listen(listen_fd_, SOMAXCONN) == -1) {
        std::cerr << "listen() failed." << std::endl;
        close(listen_fd_);
        return -1;
    }
    return 0;
}
