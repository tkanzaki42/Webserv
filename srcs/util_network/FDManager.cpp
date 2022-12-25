// Copyright 2022 tkanzaki
#include "srcs/util_network/FDManager.hpp"
#include "srcs/server/Webserv.hpp"
#include "srcs/util/StringConverter.hpp"
FDManager::FDManager() {
    std::set<int> set = Config::getAllListen();
    std::set<int>::iterator begin= set.begin();
    std::set<int>::iterator end= set.end();
    for (std::set<int>::iterator iter = begin; iter != end; ++iter) {
        std::cout << *iter << std::endl;
        int port = *iter;
        Socket soc;
        soc.set_port(port);
        sockets_.push_back(soc);
    }

    // 処理用のファイルディスクリプタを初期化する
    max_fd_ = -1;

    // 接続待ちのディスクリプタをディスクリプタ集合に設定する
    FD_ZERO(&received_fd_collection_);
    FD_ZERO(&sendable_fd_collection_);
}

FDManager::~FDManager() {
}

FDManager::FDManager(const FDManager &obj) {
    *this = obj;
}

FDManager &FDManager::operator=(const FDManager &obj) {
    max_fd_ = obj.max_fd_;
    return *this;
}

bool FDManager::select_active_socket() {
    select_prepare_();
    // 接続＆受信を待ち受ける
    if (!select_fd_()) {
        // やり直し
        return false;
    }
    sockets_it_ = sockets_.end();
    // 接続されたソケットを確定
    for (std::vector<Socket>::iterator it = sockets_.begin();
        it != sockets_.end();
        it++)
    {
        if (FD_ISSET((*it).get_listen_fd(), &received_fd_collection_)) {
            sockets_it_ = it;
            std::cout << "socket: " << (*it).get_listen_fd();
            std::cout << "[" << (*it).get_port() << "]" << std::endl;
            break;
        }
    }
    return true;
}

void FDManager::select_prepare_() {
    // 全てのソケットのFDを読み込み集合に追加
    for (
        std::vector<Socket>::iterator it = sockets_.begin();
        it < sockets_.end();
        it++)
    {
        FD_SET((*it).get_listen_fd(), &received_fd_collection_);
        if (max_fd_ < (*it).get_listen_fd()) {
            max_fd_ = (*it).get_listen_fd();
        }
    }

    // 全ての受け入れ済みのFDを読み込み集合に追加
    for (
        std::vector<T_Connection>::iterator it = connections_.begin();
        it < connections_.end();
        it++)
    {
        FD_SET((*it).accepted_fd, &received_fd_collection_);
        if (max_fd_ < (*it).accepted_fd) {
            max_fd_ = (*it).accepted_fd;
        }
    }
    
    // selectのタイムアウト時間(ここは毎回初期化しないと0になる)
    select_time_.tv_sec  = FDManager::SELECT_TIME_SECOND;
    select_time_.tv_usec = FDManager::SELECT_TIME_U_SECOND;
}

bool FDManager::select_fd_() {
    // 接続＆受信を待ち受ける
    int count = ::select(max_fd_+1,
        &received_fd_collection_,
        NULL,
        NULL,
        &select_time_);
    if (count < 0) {
        if (errno == EINTR) {
            // シグナル受信によるselect終了の場合、再度待ち受けに戻る
            std::cout << "Interrupted system call." << std::endl;
            return false;
        }
        // それ以外はexit
        std::cerr << "select() failed." << std::endl;
        _exit(1);
    } else if (count == 0) {
        // タイムアウトした場合、再度待ち受けに戻る
        std::cout << "resources released." << std::endl;
        return false;
    } else {
        std::cout << "FDManager::select ";
        std::cout << count;
        std::cout << " connection gotten ready,";
        std::cout << std::endl;
        return true;
    }
}

enum E_Event FDManager::check_event() {
    // ソケット
    if (sockets_it_ != sockets_.end()) {
        for (
            std::vector<Socket>::iterator it = sockets_.begin();
            it < sockets_.end();
            it++)
        {
            if (FD_ISSET((*sockets_it_).get_listen_fd(), &received_fd_collection_)) {
                std::cout << "socket:" << (*sockets_it_).get_listen_fd();
                std::cout << " is connected to accept." << std::endl;
                return Connect;
            }
        }
    }

    // 受け入れ済みのFD
    for (
        std::vector<T_Connection>::iterator it = connections_.begin();
        it < connections_.end();
        it++)
    {
        if (FD_ISSET((*it).accepted_fd, &received_fd_collection_)) {
            std::cout << "socket:" << (*it).accepted_fd;
            std::cout << " is connected to accept." << std::endl;
            return Read;
        }
    }
    return Write;
}

void FDManager::accept() {
    // 接続されたならクライアントからの接続を確立する
    // 受信待ちのディスクリプタをディスクリプタ集合に設定する
    T_Connection connection;
    connection.accepted_fd = (*sockets_it_).accept();
    connection.last_time   = time(NULL);
    connections_.push_back(connection);
    connections_it_ = connections_.end() - 1;
    FD_SET((*connections_it_).accepted_fd, &received_fd_collection_);
    if ((*connections_it_).accepted_fd > max_fd_) {
        max_fd_ = (*connections_it_).accepted_fd;
    }
    std::cout << "connected_fds_:" << (*connections_it_).accepted_fd;
    std::cout << " accept connection from socket:" << (*sockets_it_).get_listen_fd();
    std::cout << "." << std::endl;
}

int FDManager::receive(char *buf) {
    memset(buf, 0, sizeof(char) * BUF_SIZE);
    int read_size = -1;
    // クライアントから受信する
    read_size = ::recv((*connections_it_).accepted_fd,
        buf,
        sizeof(char) * BUF_SIZE - 1,
        0);
    if (read_size <= 0) {
        // 切断された場合、クローズする
        return -1;
    }
    // 受信成功の場合
    FD_SET((*connections_it_).accepted_fd, &sendable_fd_collection_);
    if ((*connections_it_).accepted_fd > max_fd_) {
        max_fd_ = (*connections_it_).accepted_fd;
    }
    std::cout << "connected_fds_:" << (*connections_it_).accepted_fd;
    std::cout << " received." << std::endl;
    return read_size;
}

void FDManager::search_connected_fds_it_() {
    for (
        std::vector<T_Connection>::iterator it = connections_.begin();
        it < connections_.end();
        it++)
    {
        if (FD_ISSET((*it).accepted_fd, &received_fd_collection_)) {
            connections_it_ = it;
            return ;
        }
    }
    connections_it_ = connections_.end();
}

bool FDManager::send(const std::string &str) {
    if (::send((*connections_it_).accepted_fd, str.c_str(),
        str.length(), 0) == -1) {
        std::cout << "FDManager::send failed." << std::endl;
        return false;
    }
    std::cout << "FDManager::send success to fd: ";
    std::cout << (*connections_it_).accepted_fd << std::endl;
    return true;
}

void FDManager::disconnect() {
    // クライアントとの接続を切断する
    if (!connections_.size()) {
        return ;
    }
    std::cout << "connected_fds_:" << (*connections_it_).accepted_fd;
    std::cout << " disconnected." << std::endl;
    close((*connections_it_).accepted_fd);
    FD_CLR((*connections_it_).accepted_fd, &sendable_fd_collection_);
    FD_CLR((*connections_it_).accepted_fd, &received_fd_collection_);
    connections_.erase(connections_it_);
    sockets_it_ = sockets_.end();
}

void FDManager::release() {
    if (!connections_.size()) {
        return ;
    }
    std::vector<T_Connection>::iterator it = connections_.begin();
    while (it != connections_.end())
    {
        if (time(NULL) - (*it).last_time > TIMEOUT_CONNECTION) {
            std::cout << "connected_fds_:" << (*it).accepted_fd;
            std::cout << " disconnected." << std::endl;
            close((*it).accepted_fd);
            FD_CLR((*it).accepted_fd, &sendable_fd_collection_);
            FD_CLR((*it).accepted_fd, &received_fd_collection_);
            it = connections_.erase(it);
            continue;
        }
        it++;
    }
    sockets_it_ = sockets_.end();
}

void FDManager::update_time() {
    (*connections_it_).last_time = time(NULL);
}

void FDManager::create_socket() {
    for (size_t i = 0; i < sockets_.size(); i++) {
        sockets_[i].prepare();
    }
}

void FDManager::destory_socket() {
    for (size_t i = 0; i < sockets_.size(); i++) {
        sockets_[i].cleanup();
    }
}

struct sockaddr_in FDManager::get_client_addr() {
    return (*sockets_it_).get_client_addr();
}
