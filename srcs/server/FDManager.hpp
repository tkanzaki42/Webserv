#ifndef FD_MANAGER_HPP_
#define FD_MANAGER_HPP_

#include <iostream>
#include <string>

class Webserv;

class FDManager {
 private:
    int                 accept_fd_;
    const Webserv       &server_;
 public:
    explicit FDManager(const Webserv &server);
    ~FDManager();
    FDManager(const FDManager &obj);
    FDManager &operator=(const FDManager &obj);

    bool accept();
    void disconnect() const;
    bool send(const std::string &str) const;
    bool recieve(char buf[]) const;
    int get_accept_fd() const;
};

#endif  // PROJ_WEBSERV_SRCS_SERVER_FDMANAGER_HPP_
