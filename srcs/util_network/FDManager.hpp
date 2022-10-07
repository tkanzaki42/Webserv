// Copyright 2022 tkanzaki
#ifndef SRCS_UTIL_NETWORK_FDMANAGER_HPP_
#define SRCS_UTIL_NETWORK_FDMANAGER_HPP_

#include <iostream>
#include <string>
#include <map>

#include "includes/webserv.hpp"
#include "srcs/util_network/Socket.hpp"

class FDManager {
 private:
    Socket          socket_[2];

    // 処理用のファイルディスクリプタ
    int             accept_fd_;

    // ディスクリプタの最大値
    int             max_fd_;

    // 接続待ち、受信待ちをするディスクリプタの集合(select用)
    fd_set          received_fd_collection_;

    // タイムアウト時間
    struct timeval  select_time_;

    void            prepare_select_();
    bool            select_();

 public:
    std::map<std::string, std::map<std::string, std::string> > config;

    std::map<std::string, std::string> host0;
    std::map<std::string, std::string> host1;

    // selectのタイムアウト時間
    static const time_t SELECT_TIME_SECOND = 5;
    static const time_t SELECT_TIME_U_SECOND = 0;

    FDManager();
    ~FDManager();
    FDManager(const FDManager &obj);
    FDManager &operator=(const FDManager &obj);

    // ファイルディスクリプタをオープン(クライアント)
    bool accept();

    // ファイルディスクリプタをクローズ(クライアント)
    void disconnect();

    // ファイルディスクリプタに書き込む(クライアント)
    bool send(const std::string &str);

    // ファイルディスクリプタを読み込む(クライアント)
    int receive(char *buf);

    // ソケットを作成(ソケット)
    void create_socket();

    // ソケットを削除(ソケット)
    void destory_socket();
};

#endif  // SRCS_UTIL_NETWORK_FDMANAGER_HPP_
