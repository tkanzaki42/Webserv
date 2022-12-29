// Copyright 2022 tkanzaki
#ifndef SRCS_UTIL_NETWORK_FDMANAGER_HPP_
#define SRCS_UTIL_NETWORK_FDMANAGER_HPP_

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "includes/webserv.hpp"
#include "srcs/util_network/Socket.hpp"
#include "srcs/util_network/Connection.hpp"
#include "srcs/config/Config.hpp"
#include "srcs/server/Event.hpp"

# define TIMEOUT_CONNECTION 60

class FDManager {
 private:
    // ソケット
    std::vector<Socket>           sockets_;
    std::vector<Socket>::iterator sockets_it_;

    // 接続確立済みのファイルディスクリプタ
    std::vector<Connection>              connections_;
    std::vector<Connection>::iterator    connections_it_;

    // ディスクリプタ(ソケット+処理用)の最大値
    int             max_fd_;

    // 読み書き用ディスクリプタの集合(select用)
    fd_set          received_fd_collection_;
    fd_set          sendable_fd_collection_;

    // タイムアウト時間(select用)
    struct timeval  select_time_;

    void            select_prepare_();
    bool            select_fd_();

 public:
    std::map<int, string_vector_map> config;

    std::map<std::string, std::string> host0;
    std::map<std::string, std::string> host1;

    // selectのタイムアウト時間
    static const time_t SELECT_TIME_SECOND = 1;
    static const time_t SELECT_TIME_U_SECOND = 0;

    FDManager();
    ~FDManager();
    FDManager(const FDManager &obj);
    FDManager &operator=(const FDManager &obj);

    // 接続可能なソケットを認識する
    bool select_fd_collection();

    // 受信したコネクションが接続済みかどうか
    enum E_Event check_event();

    // クライアントから切断すべきかを判定する
    bool is_disconnect();

    // クライアントからの接続を承認する
    void accept();

    // クライアントとの接続を切断する
    void disconnect();

    // 使われてないFDを解放
    void release();

    // コネクションの時間を更新
    void update_time();

    // クライアントに文字列を送る
    bool send();

    // クライアントからの文字列をバッファに読み込ませる(戻り値は読み込んだ文字列の長さ)
    int receive();

    // ソケットを作成
    void create_socket();

    // ソケットを削除
    void destory_socket();

    // getter
    struct sockaddr_in  get_client_addr();
};

#endif  // SRCS_UTIL_NETWORK_FDMANAGER_HPP_
