
#ifndef SRCS_SERVER_FDMANAGER_HPP_
#define SRCS_SERVER_FDMANAGER_HPP_

#include <iostream>
#include <string>

#include "includes/webserv.hpp"
#include "srcs/util_network/Socket.hpp"

class FDManager {
 private:
    int             accept_fd_;
    Socket          socket_;

    // 受信用バッファ
    //  char            acception_buffer_[2048];

    // 通信用ファイルディスクリプタの配列
    int             packet_fd_[10];

    // ディスクリプタの最大値
    int             max_fd_;

    // 接続待ち、受信待ちをするディスクリプタの集合
    fd_set          received_fd_collection_;

    // タイムアウト時間
    struct timeval  timeout_;

    // クライアントからの接続を確立するときに使う
    struct sockaddr_in from_addr_;
    socklen_t          from_addr_len_;

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
    int receive(char *buf) const;

    // ソケットを作成(ソケット)
    void create_socket();

    // ソケットを削除(ソケット)
    void destory_socket();
};

#endif  // SRCS_SERVER_FDMANAGER_HPP_
