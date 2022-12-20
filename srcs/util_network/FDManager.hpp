// Copyright 2022 tkanzaki
#ifndef SRCS_UTIL_NETWORK_FDMANAGER_HPP_
#define SRCS_UTIL_NETWORK_FDMANAGER_HPP_

#include <iostream>
#include <string>
#include <map>

#include "includes/webserv.hpp"
#include "srcs/util_network/Socket.hpp"
#include "srcs/config/Config.hpp"
#include <vector>

# define TIMEOUT_CONNECTION 60

class FDManager {
 private:
    typedef struct S_Connection
    {
      int    accepted_fd;
      time_t last_time;
    } T_Connection;

    // ソケット
    std::vector<Socket>           sockets_;
    std::vector<Socket>::iterator sockets_it_;

    // 接続確立済みのファイルディスクリプタ
    std::vector<T_Connection>              connections_;
    std::vector<T_Connection>::iterator    connections_it_;

    // ディスクリプタ(ソケット+処理用)の最大値
    int             max_fd_;

    // 読み書き用ディスクリプタの集合(select用)
    fd_set          received_fd_collection_;
    fd_set          sendable_fd_collection_;

    // タイムアウト時間(select用)
    struct timeval  select_time_;
    
    void            select_prepare_();
    bool            select_fd_();
    void            search_connected_fds_it_();

 public:    
    std::map<int, string_vector_map> config;

    std::map<std::string, std::string> host0;
    std::map<std::string, std::string> host1;

    // selectのタイムアウト時間
    static const time_t SELECT_TIME_SECOND = 5;
    static const time_t SELECT_TIME_U_SECOND = 0;

    FDManager();
    ~FDManager();
    FDManager(const FDManager &obj);
    FDManager &operator=(const FDManager &obj);

    // 接続可能なソケットを認識する
    bool select_active_socket();

    // 
    bool check_established();

    // クライアントからの接続を承認する
    void accept();

    // クライアントとの接続を切断する
    void disconnect();

    void release();

    // クライアントに文字列を送る
    bool send(const std::string &str);

    // クライアントからの文字列をバッファに読み込ませる(戻り値は読み込んだ文字列の長さ)
    int receive(char *buf);

    // ソケットを作成
    void create_socket();

    // ソケットを削除
    void destory_socket();

    // getter
    struct sockaddr_in  get_client_addr();
};

#endif  // SRCS_UTIL_NETWORK_FDMANAGER_HPP_
