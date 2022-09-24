
#ifndef FD_MANAGER_HPP_
#define FD_MANAGER_HPP_

#include <iostream>
#include <string>
#include "srcs/util_network/Socket.hpp"

class FDManager {
 private:
    int                 accept_fd_;
    Socket              socket_;

 public:
    FDManager();
    ~FDManager();
    FDManager(const FDManager &obj);
    FDManager &operator=(const FDManager &obj);

    int get_accept_fd() const;

    // ファイルディスクリプタをオープン(クライアント)
    bool accept();

    // ファイルディスクリプタをクローズ(クライアント)
    void disconnect() const;

    // ファイルディスクリプタに書き込む(クライアント)
    bool send(const std::string &str) const;

    // ファイルディスクリプタを読み込む(クライアント)
    int receive(char buf[]) const;

    // ソケットを作成(ソケット)
    void create_socket();

    // ソケットを削除(ソケット)
    void destory_socket();
};

#endif  // PROJ_WEBSERV_SRCS_SERVER_FDMANAGER_HPP_
