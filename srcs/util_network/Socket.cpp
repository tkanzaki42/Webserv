#include "Socket.hpp"

int Socket::prepare() {
    if (open_socket_() == -1)
        return -1;
    if (bind_address_() == -1)
        return -1;
    if (listen_() == -1)
        return -1;
    return 0;
}

int Socket::cleanup() {
    close(listen_fd_);
    return 0;
}

int Socket::open_socket_() {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ == -1) {
        std::cerr << "socket() failed." << std::endl;
        return -1;
    }

    int optval = 1;
    if (setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        std::cerr << "setsockopt() failed." << std::endl;
        close(listen_fd_);
        return -1;
    }
    return 0;
}

int Socket::bind_address_() {
    memset(&serv_addr_, 0, sizeof(serv_addr_));
    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons(port_);
    serv_addr_.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_fd_, (struct sockaddr*)&serv_addr_, sizeof(serv_addr_)) == -1) {
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
