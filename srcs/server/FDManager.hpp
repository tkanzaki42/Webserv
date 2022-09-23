#ifndef FD_MANAGER_HPP_
#define FD_MANAGER_HPP_

#include <iostream>
#include <string>

class Webserv;

class FDManager {
 private:
    int           accept_fd_;
    Webserv       &server_;
 public:
    FDManager(Webserv &server);
    ~FDManager();
    FDManager(FDManager &obj);
    FDManager &operator=(const FDManager &obj);

    bool accept();
    void disconnect();
    bool send(const std::string &str);
    bool recieve(char buf[]);
    int get_accept_fd() const;
};

#endif
