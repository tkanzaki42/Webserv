
#ifndef SRCS_SERVER_FDMANAGER_HPP_
#define SRCS_SERVER_FDMANAGER_HPP_

#include <iostream>
#include <string>

#include "includes/webserv.hpp"
#include "srcs/util_network/Socket.hpp"

class FDManager {
 private:
    Socket          socket_[2];

    // 通信用ファイルディスクリプタの配列
    int             packet_fd_[10];
    // 処理用のファイルディスクリプタのコピー
    int             accept_fd_index_;

    // ディスクリプタの最大値
    int             max_fd_;

    // 接続待ち、受信待ちをするディスクリプタの集合(select用)
    fd_set          received_fd_collection_;

    // タイムアウト時間
    struct timeval  timeout_;

    void            prepare_select_();
    bool            select_();

 public:
    std::map<std::string, std::map<std::string, int> > config;

    std::map<std::string, int> host0;
    std::map<std::string, int> host1;

    // selectのタイムアウト時間
    static const time_t TIMEOUT_SECOND = 10;
    static const time_t TIMEOUT_U_SECOND = 500000;

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

#endif  // SRCS_SERVER_FDMANAGER_HPP_
