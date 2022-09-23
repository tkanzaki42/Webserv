#include "srcs/server/FDManager.hpp"
#include "srcs/server/Webserv.hpp"

FDManager::FDManager(Webserv &server):
server_(server){
}

FDManager::~FDManager(){
}

FDManager::FDManager(FDManager &obj):
server_(obj.server_) {
    *this = obj;
}

FDManager &FDManager::operator=(const FDManager &obj) {
    accept_fd_     = obj.accept_fd_;
    return *this;
}

bool FDManager::accept() {
    accept_fd_ = ::accept(server_.get_socket().get_listen_fd(),(struct sockaddr*)NULL, NULL);
    if (accept_fd_ == -1) {
        return false;
    }
    return true;
}

bool FDManager::send(const std::string &str) {
    if (::send(accept_fd_,str.c_str(),str.length(), 0) == -1) {
        return false;
    }
    return true;
}

bool FDManager::recieve(char buf[]) {
    memset(buf, 0, sizeof(char) * BUF_SIZE);
    int read_size = recv(accept_fd_,
        buf, sizeof(char) * BUF_SIZE - 1, 0);
    buf[read_size] = '\0';
    // std::cout << "!this!" << buf << std::endl;
    if (read_size < 0) {
        return false;
    }
    return true;
}

void FDManager::disconnect(){
    close(accept_fd_);
    accept_fd_ = -1;
}

int FDManager::get_accept_fd() const {
    return accept_fd_;
}
